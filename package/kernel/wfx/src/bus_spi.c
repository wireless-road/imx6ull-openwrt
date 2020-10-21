// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI interface.
 *
 * Copyright (c) 2017-2020, Silicon Laboratories, Inc.
 * Copyright (c) 2011, Sagrad Inc.
 * Copyright (c) 2010, ST-Ericsson
 */
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/gpio/consumer.h>
#include <linux/spi/spi.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/of.h>

#include "bus.h"
#include "wfx.h"
#include "hwio.h"
#include "main.h"
#include "bh.h"

#define DETECT_INVALID_CTRL_ACCESS

#define SET_WRITE 0x7FFF        /* usage: and operation */
#define SET_READ 0x8000         /* usage: or operation */

#define WFX_RESET_INVERTED 1

static const struct wfx_platform_data wfx_spi_pdata = {
	.file_fw = "wfm_wf200",
	.file_pds = "wf200.pds",
	.use_rising_clk = true,
};

struct wfx_spi_priv {
	struct spi_device *func;
	struct wfx_dev *core;
	struct gpio_desc *gpio_reset;
	bool need_swab;
};

#if (KERNEL_VERSION(4, 19, 14) > LINUX_VERSION_CODE)
/*
 * Read of control register need a particular attention because it should be
 * done only after an IRQ raise. We can detect if this event happens by reading
 * control register twice (it is safe to read twice since we can garantee that
 * no data acess was done since IRQ raising). In add, this function optimize it
 * by doing only one SPI request.
 */
static int wfx_spi_read_ctrl_reg(struct wfx_spi_priv *bus, u16 *dst)
{
	int i, ret = 0;
	u16 tx_buf[4] = { };
	u16 rx_buf[4] = { };
	u16 tmp[2] = { };
	struct spi_message m;
	struct spi_transfer t = {
		.rx_buf = rx_buf,
		.tx_buf = tx_buf,
		.len = sizeof(tx_buf),
	};
	u16 regaddr = (WFX_REG_CONTROL << 12) | (sizeof(u16) / 2) | SET_READ;

	cpu_to_le16s(&regaddr);
	if (bus->need_swab)
		swab16s(&regaddr);

	tx_buf[0] = tx_buf[2] = regaddr;
	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	for (i = 0, tmp[0] = tmp[1] + 1; tmp[0] != tmp[1] && i < 3; i++) {
		ret = spi_sync(bus->func, &m);
		// Changes of gpio-wakeup can occur during control register
		// access. In this case, CTRL_WLAN_READY may differs.
		tmp[0] = rx_buf[1] & cpu_to_le16(~CTRL_WLAN_READY);
		tmp[1] = rx_buf[3] & cpu_to_le16(~CTRL_WLAN_READY);
	}
	if (tmp[0] != tmp[1])
		ret = -ETIMEDOUT;
	else if (i > 1)
		dev_info(bus->core->dev, "success read after %d failures\n",
			 i - 1);

	*dst = rx_buf[1];
	return ret;
}
#endif

/*
 * WFx chip read data 16bits at time and place them directly into (little
 * endian) CPU register. So, chip expect byte order like "B1 B0 B3 B2" (while
 * LE is "B0 B1 B2 B3" and BE is "B3 B2 B1 B0")
 *
 * A little endian host with bits_per_word == 16 should do the right job
 * natively. The code below to support big endian host and commonly used SPI
 * 8bits.
 */
static int wfx_spi_copy_from_io(void *priv, unsigned int addr,
				void *dst, size_t count)
{
	struct wfx_spi_priv *bus = priv;
	u16 regaddr = (addr << 12) | (count / 2) | SET_READ;
	struct spi_message      m;
	struct spi_transfer     t_addr = {
		.tx_buf         = &regaddr,
		.len            = sizeof(regaddr),
	};
	struct spi_transfer     t_msg = {
		.rx_buf         = dst,
		.len            = count,
	};
	u16 *dst16 = dst;
#if (KERNEL_VERSION(4, 19, 14) > LINUX_VERSION_CODE)
	u8 *dst8 = dst;
#endif
	int ret, i;

	WARN(count % 2, "buffer size must be a multiple of 2");

#if (KERNEL_VERSION(4, 19, 14) > LINUX_VERSION_CODE)
	/* Some SPI driver (and especially Raspberry one) have race conditions
	 * during SPI transfers. It impact last byte of transfer. Work around
	 * bellow try to detect and solve them.
	 * See https://github.com/raspberrypi/linux/issues/2200
	 */
	if (addr == WFX_REG_IN_OUT_QUEUE)
		dst8[count - 1] = 0xFF;
#endif

	cpu_to_le16s(&regaddr);
	if (bus->need_swab)
		swab16s(&regaddr);

	spi_message_init(&m);
	spi_message_add_tail(&t_addr, &m);
	spi_message_add_tail(&t_msg, &m);
	ret = spi_sync(bus->func, &m);

#if (KERNEL_VERSION(4, 19, 14) > LINUX_VERSION_CODE)
	/* If last byte has not been overwritten, read ctrl_reg manually
	 */
	if (addr == WFX_REG_IN_OUT_QUEUE && !ret && dst8[count - 1] == 0xFF) {
		dev_warn(bus->core->dev, "SPI DMA error detected (and resolved)\n");
		ret = wfx_spi_read_ctrl_reg(bus, (u16 *)(dst8 + count - 2));
	}
#endif

	if (bus->need_swab && addr == WFX_REG_CONFIG)
		for (i = 0; i < count / 2; i++)
			swab16s(&dst16[i]);
	return ret;
}

static int wfx_spi_copy_to_io(void *priv, unsigned int addr,
			      const void *src, size_t count)
{
	struct wfx_spi_priv *bus = priv;
	u16 regaddr = (addr << 12) | (count / 2);
	// FIXME: use a bounce buffer
	u16 *src16 = (void *)src;
	int ret, i;
	struct spi_message      m;
	struct spi_transfer     t_addr = {
		.tx_buf         = &regaddr,
		.len            = sizeof(regaddr),
	};
	struct spi_transfer     t_msg = {
		.tx_buf         = src,
		.len            = count,
	};

	WARN(count % 2, "buffer size must be a multiple of 2");
	WARN(regaddr & SET_READ, "bad addr or size overflow");

	cpu_to_le16s(&regaddr);

	// Register address and CONFIG content always use 16bit big endian
	// ("BADC" order)
	if (bus->need_swab)
		swab16s(&regaddr);
	if (bus->need_swab && addr == WFX_REG_CONFIG)
		for (i = 0; i < count / 2; i++)
			swab16s(&src16[i]);

	spi_message_init(&m);
	spi_message_add_tail(&t_addr, &m);
	spi_message_add_tail(&t_msg, &m);
	ret = spi_sync(bus->func, &m);

	if (bus->need_swab && addr == WFX_REG_CONFIG)
		for (i = 0; i < count / 2; i++)
			swab16s(&src16[i]);
	return ret;
}

static void wfx_spi_lock(void *priv)
{
}

static void wfx_spi_unlock(void *priv)
{
}

static irqreturn_t wfx_spi_irq_handler(int irq, void *priv)
{
	struct wfx_spi_priv *bus = priv;

	wfx_bh_request_rx(bus->core);
	return IRQ_HANDLED;
}

static int wfx_spi_irq_subscribe(void *priv)
{
	struct wfx_spi_priv *bus = priv;
	u32 flags;

	flags = irq_get_trigger_type(bus->func->irq);
	if (!flags)
		flags = IRQF_TRIGGER_HIGH;
	flags |= IRQF_ONESHOT;
	return devm_request_threaded_irq(&bus->func->dev, bus->func->irq, NULL,
					 wfx_spi_irq_handler, IRQF_ONESHOT,
					 "wfx", bus);
}

static int wfx_spi_irq_unsubscribe(void *priv)
{
	struct wfx_spi_priv *bus = priv;

	devm_free_irq(&bus->func->dev, bus->func->irq, bus);
	return 0;
}

static size_t wfx_spi_align_size(void *priv, size_t size)
{
	// Most of SPI controllers avoid DMA if buffer size is not 32bit aligned
	return ALIGN(size, 4);
}

static const struct hwbus_ops wfx_spi_hwbus_ops = {
	.copy_from_io = wfx_spi_copy_from_io,
	.copy_to_io = wfx_spi_copy_to_io,
	.irq_subscribe = wfx_spi_irq_subscribe,
	.irq_unsubscribe = wfx_spi_irq_unsubscribe,
	.lock			= wfx_spi_lock,
	.unlock			= wfx_spi_unlock,
	.align_size		= wfx_spi_align_size,
};

static int wfx_spi_probe(struct spi_device *func)
{
#if (KERNEL_VERSION(5, 5, 5) > LINUX_VERSION_CODE)
	bool invert = spi_get_device_id(func)->driver_data & WFX_RESET_INVERTED;
#endif
	struct wfx_spi_priv *bus;
	int ret;

	if (!func->bits_per_word)
		func->bits_per_word = 16;
	ret = spi_setup(func);
	if (ret)
		return ret;
	// Trace below is also displayed by spi_setup() if compiled with DEBUG
	dev_dbg(&func->dev, "SPI params: CS=%d, mode=%d bits/word=%d speed=%d\n",
		func->chip_select, func->mode, func->bits_per_word,
		func->max_speed_hz);
	if (func->bits_per_word != 16 && func->bits_per_word != 8)
		dev_warn(&func->dev, "unusual bits/word value: %d\n",
			 func->bits_per_word);
	if (func->max_speed_hz > 50000000)
		dev_warn(&func->dev, "%dHz is a very high speed\n",
			 func->max_speed_hz);

	bus = devm_kzalloc(&func->dev, sizeof(*bus), GFP_KERNEL);
	if (!bus)
		return -ENOMEM;
	bus->func = func;
	if (func->bits_per_word == 8 || IS_ENABLED(CONFIG_CPU_BIG_ENDIAN))
		bus->need_swab = true;
	spi_set_drvdata(func, bus);

	bus->gpio_reset = devm_gpiod_get_optional(&func->dev, "reset",
						  GPIOD_OUT_LOW);
	if (IS_ERR(bus->gpio_reset))
		return PTR_ERR(bus->gpio_reset);
	if (!bus->gpio_reset) {
		dev_warn(&func->dev,
			 "gpio reset is not defined, trying to load firmware anyway\n");
	} else {
#if (KERNEL_VERSION(4, 19, 0) <= LINUX_VERSION_CODE)
		gpiod_set_consumer_name(bus->gpio_reset, "wfx reset");
#endif
#if (KERNEL_VERSION(5, 5, 5) > LINUX_VERSION_CODE)
		gpiod_set_value_cansleep(bus->gpio_reset, invert ? 0 : 1);
		usleep_range(100, 150);
		gpiod_set_value_cansleep(bus->gpio_reset, invert ? 1 : 0);
#else
		if (spi_get_device_id(func)->driver_data & WFX_RESET_INVERTED)
			gpiod_toggle_active_low(bus->gpio_reset);
		gpiod_set_value_cansleep(bus->gpio_reset, 1);
		usleep_range(100, 150);
		gpiod_set_value_cansleep(bus->gpio_reset, 0);
#endif
		usleep_range(2000, 2500);
	}

	bus->core = wfx_init_common(&func->dev, &wfx_spi_pdata,
				    &wfx_spi_hwbus_ops, bus);
	if (!bus->core)
		return -EIO;

	return wfx_probe(bus->core);
}

static int wfx_spi_remove(struct spi_device *func)
{
	struct wfx_spi_priv *bus = spi_get_drvdata(func);

	wfx_release(bus->core);
	return 0;
}

/*
 * For dynamic driver binding, kernel does not use OF to match driver. It only
 * use modalias and modalias is a copy of 'compatible' DT node with vendor
 * stripped.
 */
static const struct spi_device_id wfx_spi_id[] = {
	{ "wfx-spi", WFX_RESET_INVERTED },
	{ "wf200", 0 },
	{ },
};
MODULE_DEVICE_TABLE(spi, wfx_spi_id);

#ifdef CONFIG_OF
static const struct of_device_id wfx_spi_of_match[] = {
	{ .compatible = "silabs,wfx-spi", .data = (void *)WFX_RESET_INVERTED },
	{ .compatible = "silabs,wf200" },
	{ },
};
MODULE_DEVICE_TABLE(of, wfx_spi_of_match);
#endif

struct spi_driver wfx_spi_driver = {
	.driver = {
		.name = "wfx-spi",
		.of_match_table = of_match_ptr(wfx_spi_of_match),
	},
	.id_table = wfx_spi_id,
	.probe = wfx_spi_probe,
	.remove = wfx_spi_remove,
};
