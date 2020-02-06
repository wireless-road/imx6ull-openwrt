/*
 * Copyright (C) 2012-2015 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/regulator/consumer.h>
#include <linux/v4l2-mediabus.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>

#define OV5640_VOLTAGE_ANALOG               2800000
#define OV5640_VOLTAGE_DIGITAL_CORE         1500000
#define OV5640_VOLTAGE_DIGITAL_IO           1800000

#define MIN_FPS 15
#define MAX_FPS 30
#define DEFAULT_FPS 30

#define OV5640_XCLK_MIN 6000000
#define OV5640_XCLK_MAX 24000000

#define OV5640_CHIP_ID_HIGH_BYTE        0x300A
#define OV5640_CHIP_ID_LOW_BYTE         0x300B


#define OV5640_DEFAULT_SLAVE_ID 0x3c

#define OV5640_REG_CHIP_ID		0x300a
#define OV5640_REG_PAD_OUTPUT00		0x3019
#define OV5640_REG_SC_PLL_CTRL0		0x3034
#define OV5640_REG_SC_PLL_CTRL1		0x3035
#define OV5640_REG_SC_PLL_CTRL2		0x3036
#define OV5640_REG_SC_PLL_CTRL3		0x3037
#define OV5640_REG_SLAVE_ID		0x3100
#define OV5640_REG_SYS_ROOT_DIVIDER	0x3108
#define OV5640_REG_AWB_R_GAIN		0x3400
#define OV5640_REG_AWB_G_GAIN		0x3402
#define OV5640_REG_AWB_B_GAIN		0x3404
#define OV5640_REG_AWB_MANUAL_CTRL	0x3406
#define OV5640_REG_AEC_PK_EXPOSURE_HI	0x3500
#define OV5640_REG_AEC_PK_EXPOSURE_MED	0x3501
#define OV5640_REG_AEC_PK_EXPOSURE_LO	0x3502
#define OV5640_REG_AEC_PK_MANUAL	0x3503
#define OV5640_REG_AEC_PK_REAL_GAIN	0x350a
#define OV5640_REG_AEC_PK_VTS		0x350c
#define OV5640_REG_TIMING_HTS		0x380c
#define OV5640_REG_TIMING_VTS		0x380e
#define OV5640_REG_TIMING_TC_REG21	0x3821
#define OV5640_REG_AEC_CTRL00		0x3a00
#define OV5640_REG_AEC_B50_STEP		0x3a08
#define OV5640_REG_AEC_B60_STEP		0x3a0a
#define OV5640_REG_AEC_CTRL0D		0x3a0d
#define OV5640_REG_AEC_CTRL0E		0x3a0e
#define OV5640_REG_AEC_CTRL0F		0x3a0f
#define OV5640_REG_AEC_CTRL10		0x3a10
#define OV5640_REG_AEC_CTRL11		0x3a11
#define OV5640_REG_AEC_CTRL1B		0x3a1b
#define OV5640_REG_AEC_CTRL1E		0x3a1e
#define OV5640_REG_AEC_CTRL1F		0x3a1f
#define OV5640_REG_HZ5060_CTRL00	0x3c00
#define OV5640_REG_HZ5060_CTRL01	0x3c01
#define OV5640_REG_SIGMADELTA_CTRL0C	0x3c0c
#define OV5640_REG_FRAME_CTRL01		0x4202
#define OV5640_REG_MIPI_CTRL00		0x4800
#define OV5640_REG_DEBUG_MODE		0x4814
#define OV5640_REG_PRE_ISP_TEST_SET1	0x503d
#define OV5640_REG_SDE_CTRL0		0x5580
#define OV5640_REG_SDE_CTRL1		0x5581
#define OV5640_REG_SDE_CTRL3		0x5583
#define OV5640_REG_SDE_CTRL4		0x5584
#define OV5640_REG_SDE_CTRL5		0x5585
#define OV5640_REG_AVG_READOUT		0x56a1


enum ov5640_mode_id {
	OV5640_MODE_MIN = 0,
	OV5640_MODE_QCIF_176_144,
	OV5640_MODE_QVGA_320_240,
	OV5640_MODE_VGA_640_480,
	OV5640_MODE_NTSC_720_480,
	OV5640_MODE_PAL_720_576,
	OV5640_MODE_XGA_1024_768,
	OV5640_MODE_720P_1280_720,
	OV5640_MODE_1080P_1920_1080,
	OV5640_MODE_QSXGA_2592_1944,
	OV5640_NUM_MODES,
	/*
	ov5640_mode_MIN = 0,
	ov5640_mode_VGA_640_480 = 0,
	ov5640_mode_QVGA_320_240 = 1,
	ov5640_mode_NTSC_720_480 = 2,
	ov5640_mode_PAL_720_576 = 3,
	ov5640_mode_720P_1280_720 = 4,
	ov5640_mode_1080P_1920_1080 = 5,
	ov5640_mode_QSXGA_2592_1944 = 6,
	ov5640_mode_QCIF_176_144 = 7,
	ov5640_mode_XGA_1024_768 = 8,
	ov5640_mode_MAX = 8
	*/
};

enum ov5640_frame_rate {
	OV5640_15_FPS = 0,
	OV5640_30_FPS,
	OV5640_NUM_FRAMERATES,
};

static int ov5640_framerates[] = {
	[OV5640_15_FPS] = 15,
	[OV5640_30_FPS] = 30,
};

struct ov5640_datafmt {
	u32	code;
	enum v4l2_colorspace		colorspace;
};

struct reg_value {
	u16 reg_addr;
	u8 val;
	u8 mask;
	u32 delay_ms;
/*
	u16 u16RegAddr;
	u8 u8Val;
	u8 u8Mask;
	u32 u32Delay_ms;
*/
};

/*
 * Image size under 1280 * 960 are SUBSAMPLING
 * Image size upper 1280 * 960 are SCALING
 */
enum ov5640_downsize_mode {
	SUBSAMPLING,
	SCALING,
};

struct ov5640_mode_info {
	enum ov5640_mode_id mode;
	enum ov5640_downsize_mode dn_mode;
	u32 width;
	u32 height;
	enum ov5640_frame_rate framerate;
	struct reg_value *init_data_ptr;
	u32 reg_data_size;
};

struct ov5640_ctrls {
	struct v4l2_ctrl_handler handler;
	struct {
		struct v4l2_ctrl *auto_exp;
		struct v4l2_ctrl *exposure;
	};
	struct {
		struct v4l2_ctrl *auto_wb;
		struct v4l2_ctrl *blue_balance;
		struct v4l2_ctrl *red_balance;
	};
	struct {
		struct v4l2_ctrl *auto_gain;
		struct v4l2_ctrl *gain;
	};
	struct v4l2_ctrl *brightness;
	struct v4l2_ctrl *saturation;
	struct v4l2_ctrl *contrast;
	struct v4l2_ctrl *hue;
	struct v4l2_ctrl *test_pattern;
};

struct ov5640_dev {
	struct i2c_client *i2c_client;
	struct v4l2_subdev		sd;

	//const struct ov5640_datafmt	*fmt;
	struct v4l2_captureparm streamcap;
	bool on;
	
	struct mutex lock;
	/* control settings */
	struct ov5640_ctrls ctrls;
	struct v4l2_mbus_framefmt fmt;
	u32 mclk;
	u8 mclk_source;
	struct clk *sensor_clk;
	int csi;
	
	const struct ov5640_mode_info *current_mode;
	enum ov5640_frame_rate current_fr;
	struct v4l2_fract frame_interval;
	
	u32 ae_low, ae_high, ae_target;
	
	bool pending_mode_change;
	bool streaming;

	void (*io_init)(void);
};

/*!
 * Maintains the information on the current state of the sesor.
 */
static struct ov5640_dev ov5640_data;
static int pwn_gpio, rst_gpio;
static int prev_sysclk;
static int night_mode;
static int prev_HTS;

static struct reg_value ov5640_global_init_setting[] = {
	{0x3008, 0x42, 0, 0},
	{0x3103, 0x03, 0, 0}, {0x3017, 0xff, 0, 0}, {0x3018, 0xf0, 0, 0},
	{0x3034, 0x1a, 0, 0}, {0x3037, 0x13, 0, 0}, {0x3108, 0x01, 0, 0},
	{0x3630, 0x36, 0, 0}, {0x3631, 0x0e, 0, 0}, {0x3632, 0xe2, 0, 0},
	{0x3633, 0x12, 0, 0}, {0x3621, 0xe0, 0, 0}, {0x3704, 0xa0, 0, 0},
	{0x3703, 0x5a, 0, 0}, {0x3715, 0x78, 0, 0}, {0x3717, 0x01, 0, 0},
	{0x370b, 0x60, 0, 0}, {0x3705, 0x1a, 0, 0}, {0x3905, 0x02, 0, 0},
	{0x3906, 0x10, 0, 0}, {0x3901, 0x0a, 0, 0}, {0x3731, 0x12, 0, 0},
	{0x3600, 0x08, 0, 0}, {0x3601, 0x33, 0, 0}, {0x302d, 0x60, 0, 0},
	{0x3620, 0x52, 0, 0}, {0x371b, 0x20, 0, 0}, {0x471c, 0x50, 0, 0},
	{0x3a13, 0x43, 0, 0}, {0x3a18, 0x00, 0, 0}, {0x3a19, 0x7c, 0, 0},
	{0x3635, 0x13, 0, 0}, {0x3636, 0x03, 0, 0}, {0x3634, 0x40, 0, 0},
	{0x3622, 0x01, 0, 0}, {0x3c01, 0x34, 0, 0}, {0x3c04, 0x28, 0, 0},
	{0x3c05, 0x98, 0, 0}, {0x3c06, 0x00, 0, 0}, {0x3c07, 0x07, 0, 0},
	{0x3c08, 0x00, 0, 0}, {0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0},
	{0x3c0b, 0x40, 0, 0}, {0x3810, 0x00, 0, 0}, {0x3811, 0x10, 0, 0},
	{0x3812, 0x00, 0, 0}, {0x3708, 0x64, 0, 0}, {0x4001, 0x02, 0, 0},
	{0x4005, 0x1a, 0, 0}, {0x3000, 0x00, 0, 0}, {0x3004, 0xff, 0, 0},
	{0x300e, 0x58, 0, 0}, {0x302e, 0x00, 0, 0}, {0x4300, 0x32, 0, 0},
	{0x501f, 0x00, 0, 0}, {0x440e, 0x00, 0, 0}, {0x5000, 0xa7, 0, 0},
	{0x3008, 0x02, 0, 0},
};

static struct reg_value ov5640_init_setting_30fps_VGA[] = {
	{0x3008, 0x42, 0, 0},
	{0x3103, 0x03, 0, 0}, {0x3017, 0xff, 0, 0}, {0x3018, 0xf0, 0, 0},
	{0x3034, 0x1a, 0, 0}, {0x3035, 0x11, 0, 0}, {0x3036, 0x46, 0, 0},
	{0x3037, 0x13, 0, 0}, {0x3108, 0x01, 0, 0}, {0x3630, 0x36, 0, 0},
	{0x3631, 0x0e, 0, 0}, {0x3632, 0xe2, 0, 0}, {0x3633, 0x12, 0, 0},
	{0x3621, 0xe0, 0, 0}, {0x3704, 0xa0, 0, 0}, {0x3703, 0x5a, 0, 0},
	{0x3715, 0x78, 0, 0}, {0x3717, 0x01, 0, 0}, {0x370b, 0x60, 0, 0},
	{0x3705, 0x1a, 0, 0}, {0x3905, 0x02, 0, 0}, {0x3906, 0x10, 0, 0},
	{0x3901, 0x0a, 0, 0}, {0x3731, 0x12, 0, 0}, {0x3600, 0x08, 0, 0},
	{0x3601, 0x33, 0, 0}, {0x302d, 0x60, 0, 0}, {0x3620, 0x52, 0, 0},
	{0x371b, 0x20, 0, 0}, {0x471c, 0x50, 0, 0}, {0x3a13, 0x43, 0, 0},
	{0x3a18, 0x00, 0, 0}, {0x3a19, 0xf8, 0, 0}, {0x3635, 0x13, 0, 0},
	{0x3636, 0x03, 0, 0}, {0x3634, 0x40, 0, 0}, {0x3622, 0x01, 0, 0},
	{0x3c01, 0x34, 0, 0}, {0x3c04, 0x28, 0, 0}, {0x3c05, 0x98, 0, 0},
	{0x3c06, 0x00, 0, 0}, {0x3c07, 0x08, 0, 0}, {0x3c08, 0x00, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9b, 0, 0},
	{0x3808, 0x02, 0, 0}, {0x3809, 0x80, 0, 0}, {0x380a, 0x01, 0, 0},
	{0x380b, 0xe0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x68, 0, 0},
	{0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x06, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x02, 0, 0}, {0x3000, 0x00, 0, 0},
	{0x3002, 0x1c, 0, 0}, {0x3004, 0xff, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x300e, 0x58, 0, 0}, {0x302e, 0x00, 0, 0}, {0x4300, 0x32, 0, 0},
	{0x501f, 0x00, 0, 0}, {0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0},
	{0x440e, 0x00, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0}, {0x5000, 0xa7, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x5180, 0xff, 0, 0}, {0x5181, 0xf2, 0, 0},
	{0x5182, 0x00, 0, 0}, {0x5183, 0x14, 0, 0}, {0x5184, 0x25, 0, 0},
	{0x5185, 0x24, 0, 0}, {0x5186, 0x09, 0, 0}, {0x5187, 0x09, 0, 0},
	{0x5188, 0x09, 0, 0}, {0x5189, 0x88, 0, 0}, {0x518a, 0x54, 0, 0},
	{0x518b, 0xee, 0, 0}, {0x518c, 0xb2, 0, 0}, {0x518d, 0x50, 0, 0},
	{0x518e, 0x34, 0, 0}, {0x518f, 0x6b, 0, 0}, {0x5190, 0x46, 0, 0},
	{0x5191, 0xf8, 0, 0}, {0x5192, 0x04, 0, 0}, {0x5193, 0x70, 0, 0},
	{0x5194, 0xf0, 0, 0}, {0x5195, 0xf0, 0, 0}, {0x5196, 0x03, 0, 0},
	{0x5197, 0x01, 0, 0}, {0x5198, 0x04, 0, 0}, {0x5199, 0x6c, 0, 0},
	{0x519a, 0x04, 0, 0}, {0x519b, 0x00, 0, 0}, {0x519c, 0x09, 0, 0},
	{0x519d, 0x2b, 0, 0}, {0x519e, 0x38, 0, 0}, {0x5381, 0x1e, 0, 0},
	{0x5382, 0x5b, 0, 0}, {0x5383, 0x08, 0, 0}, {0x5384, 0x0a, 0, 0},
	{0x5385, 0x7e, 0, 0}, {0x5386, 0x88, 0, 0}, {0x5387, 0x7c, 0, 0},
	{0x5388, 0x6c, 0, 0}, {0x5389, 0x10, 0, 0}, {0x538a, 0x01, 0, 0},
	{0x538b, 0x98, 0, 0}, {0x5300, 0x08, 0, 0}, {0x5301, 0x30, 0, 0},
	{0x5302, 0x10, 0, 0}, {0x5303, 0x00, 0, 0}, {0x5304, 0x08, 0, 0},
	{0x5305, 0x30, 0, 0}, {0x5306, 0x08, 0, 0}, {0x5307, 0x16, 0, 0},
	{0x5309, 0x08, 0, 0}, {0x530a, 0x30, 0, 0}, {0x530b, 0x04, 0, 0},
	{0x530c, 0x06, 0, 0}, {0x5480, 0x01, 0, 0}, {0x5481, 0x08, 0, 0},
	{0x5482, 0x14, 0, 0}, {0x5483, 0x28, 0, 0}, {0x5484, 0x51, 0, 0},
	{0x5485, 0x65, 0, 0}, {0x5486, 0x71, 0, 0}, {0x5487, 0x7d, 0, 0},
	{0x5488, 0x87, 0, 0}, {0x5489, 0x91, 0, 0}, {0x548a, 0x9a, 0, 0},
	{0x548b, 0xaa, 0, 0}, {0x548c, 0xb8, 0, 0}, {0x548d, 0xcd, 0, 0},
	{0x548e, 0xdd, 0, 0}, {0x548f, 0xea, 0, 0}, {0x5490, 0x1d, 0, 0},
	{0x5580, 0x02, 0, 0}, {0x5583, 0x40, 0, 0}, {0x5584, 0x10, 0, 0},
	{0x5589, 0x10, 0, 0}, {0x558a, 0x00, 0, 0}, {0x558b, 0xf8, 0, 0},
	{0x5800, 0x23, 0, 0}, {0x5801, 0x14, 0, 0}, {0x5802, 0x0f, 0, 0},
	{0x5803, 0x0f, 0, 0}, {0x5804, 0x12, 0, 0}, {0x5805, 0x26, 0, 0},
	{0x5806, 0x0c, 0, 0}, {0x5807, 0x08, 0, 0}, {0x5808, 0x05, 0, 0},
	{0x5809, 0x05, 0, 0}, {0x580a, 0x08, 0, 0}, {0x580b, 0x0d, 0, 0},
	{0x580c, 0x08, 0, 0}, {0x580d, 0x03, 0, 0}, {0x580e, 0x00, 0, 0},
	{0x580f, 0x00, 0, 0}, {0x5810, 0x03, 0, 0}, {0x5811, 0x09, 0, 0},
	{0x5812, 0x07, 0, 0}, {0x5813, 0x03, 0, 0}, {0x5814, 0x00, 0, 0},
	{0x5815, 0x01, 0, 0}, {0x5816, 0x03, 0, 0}, {0x5817, 0x08, 0, 0},
	{0x5818, 0x0d, 0, 0}, {0x5819, 0x08, 0, 0}, {0x581a, 0x05, 0, 0},
	{0x581b, 0x06, 0, 0}, {0x581c, 0x08, 0, 0}, {0x581d, 0x0e, 0, 0},
	{0x581e, 0x29, 0, 0}, {0x581f, 0x17, 0, 0}, {0x5820, 0x11, 0, 0},
	{0x5821, 0x11, 0, 0}, {0x5822, 0x15, 0, 0}, {0x5823, 0x28, 0, 0},
	{0x5824, 0x46, 0, 0}, {0x5825, 0x26, 0, 0}, {0x5826, 0x08, 0, 0},
	{0x5827, 0x26, 0, 0}, {0x5828, 0x64, 0, 0}, {0x5829, 0x26, 0, 0},
	{0x582a, 0x24, 0, 0}, {0x582b, 0x22, 0, 0}, {0x582c, 0x24, 0, 0},
	{0x582d, 0x24, 0, 0}, {0x582e, 0x06, 0, 0}, {0x582f, 0x22, 0, 0},
	{0x5830, 0x40, 0, 0}, {0x5831, 0x42, 0, 0}, {0x5832, 0x24, 0, 0},
	{0x5833, 0x26, 0, 0}, {0x5834, 0x24, 0, 0}, {0x5835, 0x22, 0, 0},
	{0x5836, 0x22, 0, 0}, {0x5837, 0x26, 0, 0}, {0x5838, 0x44, 0, 0},
	{0x5839, 0x24, 0, 0}, {0x583a, 0x26, 0, 0}, {0x583b, 0x28, 0, 0},
	{0x583c, 0x42, 0, 0}, {0x583d, 0xce, 0, 0}, {0x5025, 0x00, 0, 0},
	{0x3a0f, 0x30, 0, 0}, {0x3a10, 0x28, 0, 0}, {0x3a1b, 0x30, 0, 0},
	{0x3a1e, 0x26, 0, 0}, {0x3a11, 0x60, 0, 0}, {0x3a1f, 0x14, 0, 0},
	{0x3008, 0x02, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x11, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_30fps_VGA_640_480[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9b, 0, 0}, {0x3808, 0x02, 0, 0}, {0x3809, 0x80, 0, 0},
	{0x380a, 0x01, 0, 0}, {0x380b, 0xe0, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x22, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x11, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0}, {0x3503, 0x00, 0, 0},
};

static struct reg_value ov5640_setting_15fps_VGA_640_480[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9b, 0, 0}, {0x3808, 0x02, 0, 0}, {0x3809, 0x80, 0, 0},
	{0x380a, 0x01, 0, 0}, {0x380b, 0xe0, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x22, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x21, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0}, {0x3503, 0x00, 0, 0},
};

static struct reg_value ov5640_setting_30fps_QVGA_320_240[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9b, 0, 0}, {0x3808, 0x01, 0, 0}, {0x3809, 0x40, 0, 0},
	{0x380a, 0x00, 0, 0}, {0x380b, 0xf0, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x22, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x11, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_15fps_QVGA_320_240[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9b, 0, 0}, {0x3808, 0x01, 0, 0}, {0x3809, 0x40, 0, 0},
	{0x380a, 0x00, 0, 0}, {0x380b, 0xf0, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x22, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x21, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_30fps_NTSC_720_480[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x06, 0, 0},
	{0x3807, 0xd4, 0, 0}, {0x3808, 0x02, 0, 0}, {0x3809, 0xd0, 0, 0},
	{0x380a, 0x01, 0, 0}, {0x380b, 0xe0, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x22, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x11, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_15fps_NTSC_720_480[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x06, 0, 0},
	{0x3807, 0xd4, 0, 0}, {0x3808, 0x02, 0, 0}, {0x3809, 0xd0, 0, 0},
	{0x380a, 0x01, 0, 0}, {0x380b, 0xe0, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x22, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x21, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_30fps_PAL_720_576[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x60, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x09, 0, 0}, {0x3805, 0x7e, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9b, 0, 0}, {0x3808, 0x02, 0, 0}, {0x3809, 0xd0, 0, 0},
	{0x380a, 0x02, 0, 0}, {0x380b, 0x40, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x22, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x11, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_15fps_PAL_720_576[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x60, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x09, 0, 0}, {0x3805, 0x7e, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9b, 0, 0}, {0x3808, 0x02, 0, 0}, {0x3809, 0xd0, 0, 0},
	{0x380a, 0x02, 0, 0}, {0x380b, 0x40, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x22, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x21, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_30fps_720P_1280_720[] = {
	{0x3035, 0x21, 0, 0}, {0x3036, 0x69, 0, 0}, {0x3c07, 0x07, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0xfa, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x06, 0, 0}, {0x3807, 0xa9, 0, 0},
	{0x3808, 0x05, 0, 0}, {0x3809, 0x00, 0, 0}, {0x380a, 0x02, 0, 0},
	{0x380b, 0xd0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x64, 0, 0},
	{0x380e, 0x02, 0, 0}, {0x380f, 0xe4, 0, 0}, {0x3813, 0x04, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3709, 0x52, 0, 0},
	{0x370c, 0x03, 0, 0}, {0x3a02, 0x02, 0, 0}, {0x3a03, 0xe0, 0, 0},
	{0x3a14, 0x02, 0, 0}, {0x3a15, 0xe0, 0, 0}, {0x4004, 0x02, 0, 0},
	{0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x37, 0, 0}, {0x460c, 0x20, 0, 0},
	{0x4837, 0x16, 0, 0}, {0x3824, 0x04, 0, 0}, {0x5001, 0x83, 0, 0},
	{0x3503, 0x00, 0, 0},
};

static struct reg_value ov5640_setting_15fps_720P_1280_720[] = {
	{0x3035, 0x41, 0, 0}, {0x3036, 0x69, 0, 0}, {0x3c07, 0x07, 0, 0},
	{0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0}, {0x3814, 0x31, 0, 0},
	{0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0xfa, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x06, 0, 0}, {0x3807, 0xa9, 0, 0},
	{0x3808, 0x05, 0, 0}, {0x3809, 0x00, 0, 0}, {0x380a, 0x02, 0, 0},
	{0x380b, 0xd0, 0, 0}, {0x380c, 0x07, 0, 0}, {0x380d, 0x64, 0, 0},
	{0x380e, 0x02, 0, 0}, {0x380f, 0xe4, 0, 0}, {0x3813, 0x04, 0, 0},
	{0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3709, 0x52, 0, 0},
	{0x370c, 0x03, 0, 0}, {0x3a02, 0x02, 0, 0}, {0x3a03, 0xe0, 0, 0},
	{0x3a14, 0x02, 0, 0}, {0x3a15, 0xe0, 0, 0}, {0x4004, 0x02, 0, 0},
	{0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x37, 0, 0}, {0x460c, 0x20, 0, 0},
	{0x4837, 0x16, 0, 0}, {0x3824, 0x04, 0, 0}, {0x5001, 0x83, 0, 0},
	{0x3503, 0x00, 0, 0},
};

static struct reg_value ov5640_setting_30fps_QCIF_176_144[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9b, 0, 0}, {0x3808, 0x00, 0, 0}, {0x3809, 0xb0, 0, 0},
	{0x380a, 0x00, 0, 0}, {0x380b, 0x90, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x22, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x11, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_15fps_QCIF_176_144[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9b, 0, 0}, {0x3808, 0x00, 0, 0}, {0x3809, 0xb0, 0, 0},
	{0x380a, 0x00, 0, 0}, {0x380b, 0x90, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x22, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x02, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x21, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_30fps_XGA_1024_768[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9b, 0, 0}, {0x3808, 0x04, 0, 0}, {0x3809, 0x00, 0, 0},
	{0x380a, 0x03, 0, 0}, {0x380b, 0x00, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x20, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x01, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x21, 0, 0},
	{0x3036, 0x69, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_15fps_XGA_1024_768[] = {
	{0x3c07, 0x08, 0, 0}, {0x3820, 0x41, 0, 0}, {0x3821, 0x07, 0, 0},
	{0x3814, 0x31, 0, 0}, {0x3815, 0x31, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x04, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9b, 0, 0}, {0x3808, 0x04, 0, 0}, {0x3809, 0x00, 0, 0},
	{0x380a, 0x03, 0, 0}, {0x380b, 0x00, 0, 0}, {0x380c, 0x07, 0, 0},
	{0x380d, 0x68, 0, 0}, {0x380e, 0x03, 0, 0}, {0x380f, 0xd8, 0, 0},
	{0x3813, 0x06, 0, 0}, {0x3618, 0x00, 0, 0}, {0x3612, 0x29, 0, 0},
	{0x3709, 0x52, 0, 0}, {0x370c, 0x03, 0, 0}, {0x3a02, 0x0b, 0, 0},
	{0x3a03, 0x88, 0, 0}, {0x3a14, 0x0b, 0, 0}, {0x3a15, 0x88, 0, 0},
	{0x4004, 0x02, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x03, 0, 0}, {0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0},
	{0x460c, 0x20, 0, 0}, {0x4837, 0x22, 0, 0}, {0x3824, 0x01, 0, 0},
	{0x5001, 0xa3, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x21, 0, 0},
	{0x3036, 0x46, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_30fps_1080P_1920_1080[] = {
	{0x3008, 0x42, 0, 0},
	{0x3035, 0x21, 0, 0}, {0x3036, 0x54, 0, 0}, {0x3c07, 0x08, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3820, 0x40, 0, 0}, {0x3821, 0x06, 0, 0}, {0x3814, 0x11, 0, 0},
	{0x3815, 0x11, 0, 0}, {0x3800, 0x00, 0, 0}, {0x3801, 0x00, 0, 0},
	{0x3802, 0x00, 0, 0}, {0x3803, 0x00, 0, 0}, {0x3804, 0x0a, 0, 0},
	{0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0}, {0x3807, 0x9f, 0, 0},
	{0x3808, 0x0a, 0, 0}, {0x3809, 0x20, 0, 0}, {0x380a, 0x07, 0, 0},
	{0x380b, 0x98, 0, 0}, {0x380c, 0x0b, 0, 0}, {0x380d, 0x1c, 0, 0},
	{0x380e, 0x07, 0, 0}, {0x380f, 0xb0, 0, 0}, {0x3810, 0x00, 0, 0},
	{0x3811, 0x10, 0, 0}, {0x3812, 0x00, 0, 0}, {0x3813, 0x04, 0, 0},
	{0x3618, 0x04, 0, 0}, {0x3612, 0x29, 0, 0}, {0x3708, 0x21, 0, 0},
	{0x3709, 0x12, 0, 0}, {0x370c, 0x00, 0, 0}, {0x3a02, 0x03, 0, 0},
	{0x3a03, 0xd8, 0, 0}, {0x3a08, 0x01, 0, 0}, {0x3a09, 0x27, 0, 0},
	{0x3a0a, 0x00, 0, 0}, {0x3a0b, 0xf6, 0, 0}, {0x3a0e, 0x03, 0, 0},
	{0x3a0d, 0x04, 0, 0}, {0x3a14, 0x03, 0, 0}, {0x3a15, 0xd8, 0, 0},
	{0x4001, 0x02, 0, 0}, {0x4004, 0x06, 0, 0}, {0x4713, 0x03, 0, 0},
	{0x4407, 0x04, 0, 0}, {0x460b, 0x35, 0, 0}, {0x460c, 0x22, 0, 0},
	{0x3824, 0x02, 0, 0}, {0x5001, 0x83, 0, 0}, {0x3035, 0x11, 0, 0},
	{0x3036, 0x54, 0, 0}, {0x3c07, 0x07, 0, 0}, {0x3c08, 0x00, 0, 0},
	{0x3c09, 0x1c, 0, 0}, {0x3c0a, 0x9c, 0, 0}, {0x3c0b, 0x40, 0, 0},
	{0x3800, 0x01, 0, 0}, {0x3801, 0x50, 0, 0}, {0x3802, 0x01, 0, 0},
	{0x3803, 0xb2, 0, 0}, {0x3804, 0x08, 0, 0}, {0x3805, 0xef, 0, 0},
	{0x3806, 0x05, 0, 0}, {0x3807, 0xf1, 0, 0}, {0x3808, 0x07, 0, 0},
	{0x3809, 0x80, 0, 0}, {0x380a, 0x04, 0, 0}, {0x380b, 0x38, 0, 0},
	{0x380c, 0x09, 0, 0}, {0x380d, 0xc4, 0, 0}, {0x380e, 0x04, 0, 0},
	{0x380f, 0x60, 0, 0}, {0x3612, 0x2b, 0, 0}, {0x3708, 0x64, 0, 0},
	{0x3a02, 0x04, 0, 0}, {0x3a03, 0x60, 0, 0}, {0x3a08, 0x01, 0, 0},
	{0x3a09, 0x50, 0, 0}, {0x3a0a, 0x01, 0, 0}, {0x3a0b, 0x18, 0, 0},
	{0x3a0e, 0x03, 0, 0}, {0x3a0d, 0x04, 0, 0}, {0x3a14, 0x04, 0, 0},
	{0x3a15, 0x60, 0, 0}, {0x4713, 0x02, 0, 0}, {0x4407, 0x04, 0, 0},
	{0x460b, 0x37, 0, 0}, {0x460c, 0x20, 0, 0}, {0x3824, 0x04, 0, 0},
	{0x4005, 0x1a, 0, 0}, {0x3008, 0x02, 0, 0},
	{0x3503, 0, 0, 0},
};

static struct reg_value ov5640_setting_15fps_1080P_1920_1080[] = {
	{0x3c07, 0x07, 0, 0}, {0x3820, 0x40, 0, 0}, {0x3821, 0x06, 0, 0},
	{0x3814, 0x11, 0, 0}, {0x3815, 0x11, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0xee, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x05, 0, 0},
	{0x3807, 0xc3, 0, 0}, {0x3808, 0x07, 0, 0}, {0x3809, 0x80, 0, 0},
	{0x380a, 0x04, 0, 0}, {0x380b, 0x38, 0, 0}, {0x380c, 0x0b, 0, 0},
	{0x380d, 0x1c, 0, 0}, {0x380e, 0x07, 0, 0}, {0x380f, 0xb0, 0, 0},
	{0x3813, 0x04, 0, 0}, {0x3618, 0x04, 0, 0}, {0x3612, 0x2b, 0, 0},
	{0x3709, 0x12, 0, 0}, {0x370c, 0x00, 0, 0}, {0x3a02, 0x07, 0, 0},
	{0x3a03, 0xae, 0, 0}, {0x3a14, 0x07, 0, 0}, {0x3a15, 0xae, 0, 0},
	{0x4004, 0x06, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x02, 0, 0}, {0x4407, 0x0c, 0, 0}, {0x460b, 0x37, 0, 0},
	{0x460c, 0x20, 0, 0}, {0x4837, 0x2c, 0, 0}, {0x3824, 0x01, 0, 0},
	{0x5001, 0x83, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x21, 0, 0},
	{0x3036, 0x69, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct reg_value ov5640_setting_15fps_QSXGA_2592_1944[] = {
	{0x3c07, 0x07, 0, 0}, {0x3820, 0x40, 0, 0}, {0x3821, 0x06, 0, 0},
	{0x3814, 0x11, 0, 0}, {0x3815, 0x11, 0, 0}, {0x3800, 0x00, 0, 0},
	{0x3801, 0x00, 0, 0}, {0x3802, 0x00, 0, 0}, {0x3803, 0x00, 0, 0},
	{0x3804, 0x0a, 0, 0}, {0x3805, 0x3f, 0, 0}, {0x3806, 0x07, 0, 0},
	{0x3807, 0x9f, 0, 0}, {0x3808, 0x0a, 0, 0}, {0x3809, 0x20, 0, 0},
	{0x380a, 0x07, 0, 0}, {0x380b, 0x98, 0, 0}, {0x380c, 0x0b, 0, 0},
	{0x380d, 0x1c, 0, 0}, {0x380e, 0x07, 0, 0}, {0x380f, 0xb0, 0, 0},
	{0x3813, 0x04, 0, 0}, {0x3618, 0x04, 0, 0}, {0x3612, 0x2b, 0, 0},
	{0x3709, 0x12, 0, 0}, {0x370c, 0x00, 0, 0}, {0x3a02, 0x07, 0, 0},
	{0x3a03, 0xae, 0, 0}, {0x3a14, 0x07, 0, 0}, {0x3a15, 0xae, 0, 0},
	{0x4004, 0x06, 0, 0}, {0x3002, 0x1c, 0, 0}, {0x3006, 0xc3, 0, 0},
	{0x4713, 0x02, 0, 0}, {0x4407, 0x0c, 0, 0}, {0x460b, 0x37, 0, 0},
	{0x460c, 0x20, 0, 0}, {0x4837, 0x2c, 0, 0}, {0x3824, 0x01, 0, 0},
	{0x5001, 0x83, 0, 0}, {0x3034, 0x1a, 0, 0}, {0x3035, 0x21, 0, 0},
	{0x3036, 0x69, 0, 0}, {0x3037, 0x13, 0, 0},
};

static struct ov5640_mode_info 
ov5640_mode_data[OV5640_NUM_FRAMERATES][OV5640_NUM_MODES] = {
		{
		{OV5640_MODE_QCIF_176_144, SUBSAMPLING, 176, 144, OV5640_15_FPS,
		 ov5640_setting_15fps_QCIF_176_144,
		 ARRAY_SIZE(ov5640_setting_15fps_QCIF_176_144)},
		{OV5640_MODE_QVGA_320_240, SUBSAMPLING, 320,  240, OV5640_15_FPS,
		 ov5640_setting_15fps_QVGA_320_240,
		 ARRAY_SIZE(ov5640_setting_15fps_QVGA_320_240)},
		{OV5640_MODE_VGA_640_480, SUBSAMPLING, 640,  480, OV5640_15_FPS,
		 ov5640_setting_15fps_VGA_640_480,
		 ARRAY_SIZE(ov5640_setting_15fps_VGA_640_480)},
		{OV5640_MODE_NTSC_720_480, SUBSAMPLING, 720, 480, OV5640_15_FPS,
		 ov5640_setting_15fps_NTSC_720_480,
		 ARRAY_SIZE(ov5640_setting_15fps_NTSC_720_480)},
		{OV5640_MODE_PAL_720_576, SUBSAMPLING, 720, 576, OV5640_15_FPS,
		 ov5640_setting_15fps_PAL_720_576,
		 ARRAY_SIZE(ov5640_setting_15fps_PAL_720_576)},
		{OV5640_MODE_XGA_1024_768, SUBSAMPLING, 1024, 768, OV5640_15_FPS,
		 ov5640_setting_15fps_XGA_1024_768,
		 ARRAY_SIZE(ov5640_setting_15fps_XGA_1024_768)},
		{OV5640_MODE_720P_1280_720, SUBSAMPLING, 1280, 720, OV5640_15_FPS,
		 ov5640_setting_15fps_720P_1280_720,
		 ARRAY_SIZE(ov5640_setting_15fps_720P_1280_720)},
		{OV5640_MODE_1080P_1920_1080, SCALING, 1920, 1080, OV5640_15_FPS,
		 ov5640_setting_15fps_1080P_1920_1080,
		 ARRAY_SIZE(ov5640_setting_15fps_1080P_1920_1080)},
		{OV5640_MODE_QSXGA_2592_1944, SCALING, 2592, 1944, OV5640_15_FPS,
		 ov5640_setting_15fps_QSXGA_2592_1944,
		 ARRAY_SIZE(ov5640_setting_15fps_QSXGA_2592_1944)},
	}, {
		{OV5640_MODE_QCIF_176_144, SUBSAMPLING, 176, 144,  OV5640_30_FPS,
		 ov5640_setting_30fps_QCIF_176_144,
		 ARRAY_SIZE(ov5640_setting_30fps_QCIF_176_144)},
		{OV5640_MODE_QVGA_320_240, SUBSAMPLING, 320,  240, OV5640_30_FPS,
		 ov5640_setting_30fps_QVGA_320_240,
		 ARRAY_SIZE(ov5640_setting_30fps_QVGA_320_240)},
		{OV5640_MODE_VGA_640_480, SUBSAMPLING, 640,  480, OV5640_30_FPS,
		 ov5640_setting_30fps_VGA_640_480,
		 ARRAY_SIZE(ov5640_setting_30fps_VGA_640_480)},
		{OV5640_MODE_NTSC_720_480, SUBSAMPLING, 720, 480, OV5640_30_FPS,
		 ov5640_setting_30fps_NTSC_720_480,
		 ARRAY_SIZE(ov5640_setting_30fps_NTSC_720_480)},
		{OV5640_MODE_PAL_720_576, SUBSAMPLING, 720, 576, OV5640_30_FPS,
		 ov5640_setting_30fps_PAL_720_576,
		 ARRAY_SIZE(ov5640_setting_30fps_PAL_720_576)},
		{OV5640_MODE_XGA_1024_768, SUBSAMPLING, 1024, 768, OV5640_30_FPS,
		 ov5640_setting_30fps_XGA_1024_768,
		 ARRAY_SIZE(ov5640_setting_30fps_XGA_1024_768)},
		{OV5640_MODE_720P_1280_720, SUBSAMPLING, 1280, 720, OV5640_30_FPS,
		 ov5640_setting_30fps_720P_1280_720,
		 ARRAY_SIZE(ov5640_setting_30fps_720P_1280_720)},
		{OV5640_MODE_1080P_1920_1080, SCALING, 1920, 1080, OV5640_30_FPS,
		 ov5640_setting_30fps_1080P_1920_1080,
		 ARRAY_SIZE(ov5640_setting_30fps_1080P_1920_1080)},
		{OV5640_MODE_QSXGA_2592_1944, -1, 0, 0,  OV5640_30_FPS, NULL, 0},
	},
};

static struct regulator *io_regulator;
static struct regulator *core_regulator;
static struct regulator *analog_regulator;

static int ov5640_probe(struct i2c_client *adapter,
				const struct i2c_device_id *device_id);
static int ov5640_remove(struct i2c_client *client);

static const struct i2c_device_id ov5640_id[] = {
	{"ov5640", 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, ov5640_id);

static struct i2c_driver ov5640_i2c_driver = {
	.driver = {
		  .owner = THIS_MODULE,
		  .name  = "ov5640",
		  },
	.probe  = ov5640_probe,
	.remove = ov5640_remove,
	.id_table = ov5640_id,
};

static const struct ov5640_datafmt ov5640_colour_fmts[] = {
	{MEDIA_BUS_FMT_YUYV8_2X8, V4L2_COLORSPACE_JPEG},
};

static inline struct ov5640_dev *to_ov5640_dev(struct v4l2_subdev *sd)
{
	return container_of(sd, struct ov5640_dev, sd);
}

static inline struct v4l2_subdev *ctrl_to_sd(struct v4l2_ctrl *ctrl)
{
	return &container_of(ctrl->handler, struct ov5640_dev,
			     ctrls.handler)->sd;
}

/* Find a data format by a pixel code in an array */
static const struct ov5640_datafmt
			*ov5640_find_datafmt(u32 code)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ov5640_colour_fmts); i++)
		if (ov5640_colour_fmts[i].code == code)
			return ov5640_colour_fmts + i;

	return NULL;
}

static inline void ov5640_power_down(int enable)
{
	gpio_set_value_cansleep(pwn_gpio, enable);

	msleep(2);
}

static inline void ov5640_reset(void)
{
	/* camera reset */
	gpio_set_value_cansleep(rst_gpio, 1);

	/* camera power down */
	gpio_set_value_cansleep(pwn_gpio, 1);
	msleep(5);
	gpio_set_value_cansleep(pwn_gpio, 0);
	msleep(5);
	gpio_set_value_cansleep(rst_gpio, 0);
	msleep(1);
	gpio_set_value_cansleep(rst_gpio, 1);
	msleep(5);
	gpio_set_value_cansleep(pwn_gpio, 1);
	msleep(10);
	gpio_set_value_cansleep(pwn_gpio, 0);
}

static int ov5640_regulator_enable(struct device *dev)
{
	int ret = 0;

	io_regulator = devm_regulator_get(dev, "DOVDD");
	if (!IS_ERR(io_regulator)) {
		regulator_set_voltage(io_regulator,
				      OV5640_VOLTAGE_DIGITAL_IO,
				      OV5640_VOLTAGE_DIGITAL_IO);
		ret = regulator_enable(io_regulator);
		if (ret) {
			dev_err(dev, "set io voltage failed\n");
			return ret;
		} else {
			dev_dbg(dev, "set io voltage ok\n");
		}
	} else {
		io_regulator = NULL;
		dev_warn(dev, "cannot get io voltage\n");
	}

	core_regulator = devm_regulator_get(dev, "DVDD");
	if (!IS_ERR(core_regulator)) {
		regulator_set_voltage(core_regulator,
				      OV5640_VOLTAGE_DIGITAL_CORE,
				      OV5640_VOLTAGE_DIGITAL_CORE);
		ret = regulator_enable(core_regulator);
		if (ret) {
			dev_err(dev, "set core voltage failed\n");
			return ret;
		} else {
			dev_dbg(dev, "set core voltage ok\n");
		}
	} else {
		core_regulator = NULL;
		dev_warn(dev, "cannot get core voltage\n");
	}

	analog_regulator = devm_regulator_get(dev, "AVDD");
	if (!IS_ERR(analog_regulator)) {
		regulator_set_voltage(analog_regulator,
				      OV5640_VOLTAGE_ANALOG,
				      OV5640_VOLTAGE_ANALOG);
		ret = regulator_enable(analog_regulator);
		if (ret) {
			dev_err(dev, "set analog voltage failed\n");
			return ret;
		} else {
			dev_dbg(dev, "set analog voltage ok\n");
		}
	} else {
		analog_regulator = NULL;
		dev_warn(dev, "cannot get analog voltage\n");
	}

	return ret;
}

static int ov5640_write_reg(struct ov5640_dev *sensor, u16 reg, u8 val)
{
#if 0
	u8 au8Buf[3] = {0};

	au8Buf[0] = reg >> 8;
	au8Buf[1] = reg & 0xff;
	au8Buf[2] = val;

	if (i2c_master_send(ov5640_data.i2c_client, au8Buf, 3) < 0) {
		pr_err("%s:write reg error:reg=%x,val=%x\n",
			__func__, reg, val);
		return -1;
	}

	return 0;
#endif
	struct i2c_client *client = sensor->i2c_client;
	struct i2c_msg msg;
	u8 buf[3] = {0};
	int ret;

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	buf[2] = val;

	msg.addr = client->addr;
	msg.flags = client->flags;
	msg.buf = buf;
	msg.len = sizeof(buf);

	ret = i2c_transfer(client->adapter, &msg, 1);
	if (ret < 0) {
		v4l2_err(&sensor->sd, "%s: error: reg=%x, val=%x\n",
			__func__, reg, val);
		return ret;
	}

	return 0;
	
}

static int ov5640_read_reg(struct ov5640_dev *sensor, u16 reg, u8 *val)
{
#if 0
	u8 au8RegBuf[2] = {0};
	u8 u8RdVal = 0;

	au8RegBuf[0] = reg >> 8;
	au8RegBuf[1] = reg & 0xff;

	if (2 != i2c_master_send(ov5640_data.i2c_client, au8RegBuf, 2)) {
		pr_err("%s:write reg error:reg=%x\n",
				__func__, reg);
		return -1;
	}

	if (1 != i2c_master_recv(ov5640_data.i2c_client, &u8RdVal, 1)) {
		pr_err("%s:read reg error:reg=%x,val=%x\n",
				__func__, reg, u8RdVal);
		return -1;
	}

	*val = u8RdVal;

	return u8RdVal;
#endif
	struct i2c_client *client = sensor->i2c_client;
	struct i2c_msg msg[2];
	u8 buf[2];
	int ret;

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;

	msg[0].addr = client->addr;
	msg[0].flags = client->flags;
	msg[0].buf = buf;
	msg[0].len = sizeof(buf);

	msg[1].addr = client->addr;
	msg[1].flags = client->flags | I2C_M_RD;
	msg[1].buf = buf;
	msg[1].len = 1;

	ret = i2c_transfer(client->adapter, msg, 2);
	if (ret < 0)
		return ret;

	*val = buf[0];
	return 0;
}

static int ov5640_read_reg16(struct ov5640_dev *sensor, u16 reg, u16 *val)
{
	u8 hi, lo;
	int ret;

	ret = ov5640_read_reg(sensor, reg, &hi);
	if (ret)
		return ret;
	ret = ov5640_read_reg(sensor, reg+1, &lo);
	if (ret)
		return ret;

	*val = ((u16)hi << 8) | (u16)lo;
	return 0;
}

static int ov5640_write_reg16(struct ov5640_dev *sensor, u16 reg, u16 val)
{
	int ret;

	ret = ov5640_write_reg(sensor, reg, val >> 8);
	if (ret)
		return ret;

	return ov5640_write_reg(sensor, reg + 1, val & 0xff);
}

static int ov5640_mod_reg(struct ov5640_dev *sensor, u16 reg,
			  u8 mask, u8 val)
{
	u8 readval;
	int ret;

	ret = ov5640_read_reg(sensor, reg, &readval);
	if (ret)
		return ret;

	readval &= ~mask;
	val &= mask;
	val |= readval;

	return ov5640_write_reg(sensor, reg, val);
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int ov5640_get_register(struct v4l2_subdev *sd,
					struct v4l2_dbg_register *reg)
{
	//struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;
	u8 val;

	if (reg->reg & ~0xffff)
		return -EINVAL;

	reg->size = 1;

	ret = ov5640_read_reg(&ov5640_data, reg->reg, &val);
	if (!ret)
		reg->val = (__u64)val;

	return ret;
}

static int ov5640_set_register(struct v4l2_subdev *sd,
					const struct v4l2_dbg_register *reg)
{
	//struct i2c_client *client = v4l2_get_subdevdata(sd);

	if (reg->reg & ~0xffff || reg->val & ~0xff)
		return -EINVAL;

	return ov5640_write_reg(&ov5640_data, reg->reg, reg->val);
}
#endif

static void ov5640_soft_reset(void)
{
	/* sysclk from pad */
	ov5640_write_reg(&ov5640_data, 0x3103, 0x11);

	/* software reset */
	ov5640_write_reg(&ov5640_data, 0x3008, 0x82);

	/* delay at least 5ms */
	msleep(10);
}

/* set sensor driver capability
 * 0x302c[7:6] - strength
	00     - 1x
	01     - 2x
	10     - 3x
	11     - 4x
 */
static int ov5640_driver_capability(int strength)
{
	u8 temp = 0;

	if (strength > 4 || strength < 1) {
		pr_err("The valid driver capability of ov5640 is 1x~4x\n");
		return -EINVAL;
	}

	ov5640_read_reg(&ov5640_data, 0x302c, &temp);

	temp &= ~0xc0;	/* clear [7:6] */
	temp |= ((strength - 1) << 6);	/* set [7:6] */

	ov5640_write_reg(&ov5640_data, 0x302c, temp);

	return 0;
}

/* calculate sysclk */
static int ov5640_get_sysclk(struct ov5640_dev *sensor)
{
	int xvclk = sensor->mclk / 10000;
	u32 multiplier, prediv, VCO, sysdiv, pll_rdiv;
	u32 sclk_rdiv_map[] = {1, 2, 4, 8};
	u32 bit_div2x = 1, sclk_rdiv, sysclk;
	u8 temp1, temp2;
	int ret;

	ret = ov5640_read_reg(sensor, OV5640_REG_SC_PLL_CTRL0, &temp1);
	if (ret)
		return ret;
	temp2 = temp1 & 0x0f;
	if (temp2 == 8 || temp2 == 10)
		bit_div2x = temp2 / 2;

	ret = ov5640_read_reg(sensor, OV5640_REG_SC_PLL_CTRL1, &temp1);
	if (ret)
		return ret;
	sysdiv = temp1 >> 4;
	if (sysdiv == 0)
		sysdiv = 16;

	ret = ov5640_read_reg(sensor, OV5640_REG_SC_PLL_CTRL2, &temp1);
	if (ret)
		return ret;
	multiplier = temp1;

	ret = ov5640_read_reg(sensor, OV5640_REG_SC_PLL_CTRL3, &temp1);
	if (ret)
		return ret;
	prediv = temp1 & 0x0f;
	pll_rdiv = ((temp1 >> 4) & 0x01) + 1;

	ret = ov5640_read_reg(sensor, OV5640_REG_SYS_ROOT_DIVIDER, &temp1);
	if (ret)
		return ret;
	temp2 = temp1 & 0x03;
	sclk_rdiv = sclk_rdiv_map[temp2];

	if (!prediv || !sysdiv || !pll_rdiv || !bit_div2x)
		return -EINVAL;

	VCO = xvclk * multiplier / prediv;

	sysclk = VCO / sysdiv / pll_rdiv * 2 / bit_div2x / sclk_rdiv;

	return sysclk;
}

/* read HTS from register settings */
static int ov5640_get_hts(struct ov5640_dev *sensor)
{
	/* read HTS from register settings */
	u16 hts;
	int ret;

	ret = ov5640_read_reg16(sensor, OV5640_REG_TIMING_HTS, &hts);
	if (ret)
		return ret;
	return hts;
}

/* read VTS from register settings */
static int ov5640_get_vts(struct ov5640_dev *sensor)
{
	u16 vts;
	int ret;

	ret = ov5640_read_reg16(sensor, OV5640_REG_TIMING_VTS, &vts);
	if (ret)
		return ret;
	return vts;
}

/* write VTS to registers */
static int ov5640_set_vts(struct ov5640_dev *sensor, int vts)
{
	return ov5640_write_reg16(sensor, OV5640_REG_TIMING_VTS, vts);
}

/* read shutter, in number of line period */
static int ov5640_get_exposure(struct ov5640_dev *sensor)
{
	int exp, ret;
	u8 temp;

	ret = ov5640_read_reg(sensor, OV5640_REG_AEC_PK_EXPOSURE_HI, &temp);
	if (ret)
		return ret;
	exp = ((int)temp & 0x0f) << 16;
	ret = ov5640_read_reg(sensor, OV5640_REG_AEC_PK_EXPOSURE_MED, &temp);
	if (ret)
		return ret;
	exp |= ((int)temp << 8);
	ret = ov5640_read_reg(sensor, OV5640_REG_AEC_PK_EXPOSURE_LO, &temp);
	if (ret)
		return ret;
	exp |= (int)temp;

	return exp >> 4;
}

/* write shutter, in number of line period */
static int ov5640_set_exposure(struct ov5640_dev *sensor, u32 exposure)
{
	int ret;

	exposure <<= 4;

	ret = ov5640_write_reg(sensor,
			       OV5640_REG_AEC_PK_EXPOSURE_LO,
			       exposure & 0xff);
	if (ret)
		return ret;
	ret = ov5640_write_reg(sensor,
			       OV5640_REG_AEC_PK_EXPOSURE_MED,
			       (exposure >> 8) & 0xff);
	if (ret)
		return ret;
	return ov5640_write_reg(sensor,
				OV5640_REG_AEC_PK_EXPOSURE_HI,
				(exposure >> 16) & 0x0f);
}

/* read gain, 16 = 1x */
static int ov5640_get_gain(struct ov5640_dev *sensor)
{
	u16 gain;
	int ret;

	ret = ov5640_read_reg16(sensor, OV5640_REG_AEC_PK_REAL_GAIN, &gain);
	if (ret)
		return ret;

	return gain & 0x3ff;
}

/* write gain, 16 = 1x */
static int ov5640_set_gain(struct ov5640_dev *sensor, int gain)
{
	return ov5640_write_reg16(sensor, OV5640_REG_AEC_PK_REAL_GAIN, gain & 0x3ff);
}

/* get banding filter value */
static int ov5640_get_light_freq(struct ov5640_dev *sensor)
{
	int ret, light_freq = 0;
	u8 temp, temp1;

	ret = ov5640_read_reg(sensor, OV5640_REG_HZ5060_CTRL01, &temp);
	if (ret)
		return ret;

	if (temp & 0x80) {
		/* manual */
		ret = ov5640_read_reg(sensor, OV5640_REG_HZ5060_CTRL00,
				      &temp1);
		if (ret)
			return ret;
		if (temp1 & 0x04) {
			/* 50Hz */
			light_freq = 50;
		} else {
			/* 60Hz */
			light_freq = 60;
		}
	} else {
		/* auto */
		ret = ov5640_read_reg(sensor, OV5640_REG_SIGMADELTA_CTRL0C,
				      &temp1);
		if (ret)
			return ret;

		if (temp1 & 0x01) {
			/* 50Hz */
			light_freq = 50;
		} else {
			/* 60Hz */
			light_freq = 60;
		}
	}

	return light_freq;
}

static void ov5640_set_bandingfilter(void)
{
	int prev_VTS;
	int band_step60, max_band60, band_step50, max_band50;

	/* read preview PCLK */
	prev_sysclk = ov5640_get_sysclk(&ov5640_data);

	/* read preview HTS */
	prev_HTS = ov5640_get_hts(&ov5640_data);

	/* read preview VTS */
	prev_VTS = ov5640_get_vts(&ov5640_data);

	/* calculate banding filter */
	/* 60Hz */
	band_step60 = prev_sysclk * 100/prev_HTS * 100/120;
	ov5640_write_reg(&ov5640_data, 0x3a0a, (band_step60 >> 8));
	ov5640_write_reg(&ov5640_data, 0x3a0b, (band_step60 & 0xff));

	max_band60 = (int)((prev_VTS-4)/band_step60);
	ov5640_write_reg(&ov5640_data, 0x3a0d, max_band60);

	/* 50Hz */
	band_step50 = prev_sysclk * 100/prev_HTS;
	ov5640_write_reg(&ov5640_data, 0x3a08, (band_step50 >> 8));
	ov5640_write_reg(&ov5640_data, 0x3a09, (band_step50 & 0xff));

	max_band50 = (int)((prev_VTS-4)/band_step50);
	ov5640_write_reg(&ov5640_data, 0x3a0e, max_band50);
}

/* stable in high */
static int ov5640_set_ae_target(struct ov5640_dev *sensor, int target)
{
	/* stable in high */
	u32 fast_high, fast_low;
	int ret;

	sensor->ae_low = target * 23 / 25;	/* 0.92 */
	sensor->ae_high = target * 27 / 25;	/* 1.08 */

	fast_high = sensor->ae_high << 1;
	if (fast_high > 255)
		fast_high = 255;

	fast_low = sensor->ae_low >> 1;

	ret = ov5640_write_reg(sensor, OV5640_REG_AEC_CTRL0F, sensor->ae_high);
	if (ret)
		return ret;
	ret = ov5640_write_reg(sensor, OV5640_REG_AEC_CTRL10, sensor->ae_low);
	if (ret)
		return ret;
	ret = ov5640_write_reg(sensor, OV5640_REG_AEC_CTRL1B, sensor->ae_high);
	if (ret)
		return ret;
	ret = ov5640_write_reg(sensor, OV5640_REG_AEC_CTRL1E, sensor->ae_low);
	if (ret)
		return ret;
	ret = ov5640_write_reg(sensor, OV5640_REG_AEC_CTRL11, fast_high);
	if (ret)
		return ret;
	return ov5640_write_reg(sensor, OV5640_REG_AEC_CTRL1F, fast_low);
}

/* enable = 0 to turn off night mode
   enable = 1 to turn on night mode */
static int ov5640_set_night_mode(int enable)
{
	u8 mode;

	ov5640_read_reg(&ov5640_data, 0x3a00, &mode);

	if (enable) {
		/* night mode on */
		mode |= 0x04;
		ov5640_write_reg(&ov5640_data, 0x3a00, mode);
	} else {
		/* night mode off */
		mode &= 0xfb;
		ov5640_write_reg(&ov5640_data, 0x3a00, mode);
	}

	return 0;
}

/* enable = 0 to turn off AEC/AGC
   enable = 1 to turn on AEC/AGC */
static void ov5640_turn_on_AE_AG(int enable)
{
	u8 ae_ag_ctrl;

	ov5640_read_reg(&ov5640_data, 0x3503, &ae_ag_ctrl);
	if (enable) {
		/* turn on auto AE/AG */
		ae_ag_ctrl = ae_ag_ctrl & ~(0x03);
	} else {
		/* turn off AE/AG */
		ae_ag_ctrl = ae_ag_ctrl | 0x03;
	}
	ov5640_write_reg(&ov5640_data, 0x3503, ae_ag_ctrl);
}

/* download ov5640 settings to sensor through i2c */
static int ov5640_download_firmware(struct reg_value *pModeSetting, s32 ArySize)
{
	register u32 Delay_ms = 0;
	register u16 RegAddr = 0;
	register u8 Mask = 0;
	register u8 Val = 0;
	u8 RegVal = 0;
	int i, retval = 0;

	for (i = 0; i < ArySize; ++i, ++pModeSetting) {
		Delay_ms = pModeSetting->delay_ms;
		RegAddr = pModeSetting->reg_addr;
		Val = pModeSetting->val;
		Mask = pModeSetting->mask;

		if (Mask) {
			retval = ov5640_read_reg(&ov5640_data, RegAddr, &RegVal);
			if (retval < 0)
				goto err;

			RegVal &= ~(u8)Mask;
			Val &= Mask;
			Val |= RegVal;
		}

		retval = ov5640_write_reg(&ov5640_data, RegAddr, Val);
		if (retval < 0)
			goto err;

		if (Delay_ms)
			msleep(Delay_ms);
	}
err:
	return retval;
}

static const struct ov5640_mode_info *
ov5640_find_mode(struct ov5640_dev *sensor, enum ov5640_frame_rate fr,
		 int width, int height, bool nearest)
{
	const struct ov5640_mode_info *mode = NULL;
	int i;

	for (i = OV5640_NUM_MODES - 1; i >= 0; i--) {
		mode = &ov5640_mode_data[fr][i];

		if (!mode->init_data_ptr)
			continue;

		if ((nearest && mode->width <= width &&
		     mode->height <= height) ||
		    (!nearest && mode->width == width &&
		     mode->height == height))
			break;
	}

	if (nearest && i < 0)
		mode = &ov5640_mode_data[fr][0];

	return mode;
}

static int ov5640_init_mode(void)
{
	struct reg_value *pModeSetting = NULL;
	int ArySize = 0, retval = 0;

	ov5640_soft_reset();

	pModeSetting = ov5640_global_init_setting;
	ArySize = ARRAY_SIZE(ov5640_global_init_setting);
	retval = ov5640_download_firmware(pModeSetting, ArySize);
	if (retval < 0)
		goto err;

	pModeSetting = ov5640_init_setting_30fps_VGA;
	ArySize = ARRAY_SIZE(ov5640_init_setting_30fps_VGA);
	retval = ov5640_download_firmware(pModeSetting, ArySize);
	if (retval < 0)
		goto err;

	/* change driver capability to 2x according to validation board.
	 * if the image is not stable, please increase the driver strength.
	 */
	ov5640_driver_capability(2);
	ov5640_set_bandingfilter();
	ov5640_set_ae_target(&ov5640_data, ov5640_data.ae_target);
	ov5640_set_night_mode(night_mode);

	/* skip 9 vysnc: start capture at 10th vsync */
	msleep(300);

	/* turn off night mode */
	night_mode = 0;
	ov5640_data.fmt.width = 640;
	ov5640_data.fmt.height = 480;
err:
	return retval;
}

/* change to or back to subsampling mode set the mode directly
 * image size below 1280 * 960 is subsampling mode */
static int ov5640_change_mode_direct(enum ov5640_frame_rate frame_rate,
			    enum ov5640_mode_id mode)
{
	struct reg_value *pModeSetting = NULL;
	s32 ArySize = 0;
	int retval = 0;

	if (mode > OV5640_NUM_MODES || mode < OV5640_MODE_MIN) {
		pr_err("Wrong ov5640 mode detected!\n");
		return -1;
	}

	pModeSetting = ov5640_mode_data[frame_rate][mode].init_data_ptr;
	ArySize = ov5640_mode_data[frame_rate][mode].reg_data_size;

	ov5640_data.fmt.width = ov5640_mode_data[frame_rate][mode].width;
	ov5640_data.fmt.height = ov5640_mode_data[frame_rate][mode].height;

	if (ov5640_data.fmt.width == 0 || ov5640_data.fmt.height == 0 ||
	    pModeSetting == NULL || ArySize == 0)
		return -EINVAL;

	/* set ov5640 to subsampling mode */
	retval = ov5640_download_firmware(pModeSetting, ArySize);

	/* turn on AE AG for subsampling mode, in case the firmware didn't */
	ov5640_turn_on_AE_AG(1);

	/* calculate banding filter */
	ov5640_set_bandingfilter();

	/* set AE target */
	ov5640_set_ae_target(&ov5640_data, ov5640_data.ae_target);

	/* update night mode setting */
	ov5640_set_night_mode(night_mode);

	/* skip 9 vysnc: start capture at 10th vsync */
	if (mode == OV5640_MODE_XGA_1024_768 && frame_rate == OV5640_30_FPS) {
		pr_warning("ov5640: actual frame rate of XGA is 22.5fps\n");
		/* 1/22.5 * 9*/
		msleep(400);
		return retval;
	}

	if (frame_rate == OV5640_15_FPS) {
		/* 1/15 * 9*/
		msleep(600);
	} else if (frame_rate == OV5640_30_FPS) {
		/* 1/30 * 9*/
		msleep(300);
	}

	return retval;
}

/* change to scaling mode go through exposure calucation
 * image size above 1280 * 960 is scaling mode */
static int ov5640_change_mode_exposure_calc(enum ov5640_frame_rate frame_rate,
			    enum ov5640_mode_id mode)
{
	int prev_shutter, prev_gain16, average;
	int cap_shutter, cap_gain16;
	int cap_sysclk, cap_HTS, cap_VTS;
	int light_freq, cap_bandfilt, cap_maxband;
	long cap_gain16_shutter;
	u8 temp;
	struct reg_value *pModeSetting = NULL;
	s32 ArySize = 0;
	int retval = 0;

	/* check if the input mode and frame rate is valid */
	pModeSetting = ov5640_mode_data[frame_rate][mode].init_data_ptr;
	ArySize = ov5640_mode_data[frame_rate][mode].reg_data_size;

	ov5640_data.fmt.width = ov5640_mode_data[frame_rate][mode].width;
	ov5640_data.fmt.height = ov5640_mode_data[frame_rate][mode].height;

	if (ov5640_data.fmt.width == 0 || ov5640_data.fmt.height == 0 ||
		pModeSetting == NULL || ArySize == 0)
		return -EINVAL;

	/* read preview shutter */
	prev_shutter = ov5640_get_exposure(&ov5640_data);

	/* read preview gain */
	prev_gain16 = ov5640_get_gain(&ov5640_data);

	/* get average */
	average = ov5640_read_reg(&ov5640_data, 0x56a1, &temp);

	/* turn off night mode for capture */
	ov5640_set_night_mode(0);

	/* turn off overlay */
	ov5640_write_reg(&ov5640_data, 0x3022, 0x06);

	/* Write capture setting */
	retval = ov5640_download_firmware(pModeSetting, ArySize);
	if (retval < 0)
		goto err;

	/* turn off AE AG when capture image. */
	ov5640_turn_on_AE_AG(0);

	/* read capture VTS */
	cap_VTS = ov5640_get_vts(&ov5640_data);
	cap_HTS = ov5640_get_hts(&ov5640_data);
	cap_sysclk = ov5640_get_sysclk(&ov5640_data);

	/* calculate capture banding filter */
	light_freq = ov5640_get_light_freq(&ov5640_data);
	if (light_freq == 60) {
		/* 60Hz */
		cap_bandfilt = cap_sysclk * 100 / cap_HTS * 100 / 120;
	} else {
		/* 50Hz */
		cap_bandfilt = cap_sysclk * 100 / cap_HTS;
	}
	cap_maxband = (int)((cap_VTS - 4)/cap_bandfilt);
	/* calculate capture shutter/gain16 */
	if (average > ov5640_data.ae_low && average < ov5640_data.ae_high) {
		/* in stable range */
		cap_gain16_shutter =
			prev_gain16 * prev_shutter * cap_sysclk/prev_sysclk *
			prev_HTS/cap_HTS * ov5640_data.ae_target / average;
	} else {
		cap_gain16_shutter =
			prev_gain16 * prev_shutter * cap_sysclk/prev_sysclk *
			prev_HTS/cap_HTS;
	}

	/* gain to shutter */
	if (cap_gain16_shutter < (cap_bandfilt * 16)) {
		/* shutter < 1/100 */
		cap_shutter = cap_gain16_shutter/16;
		if (cap_shutter < 1)
			cap_shutter = 1;
		cap_gain16 = cap_gain16_shutter/cap_shutter;
		if (cap_gain16 < 16)
			cap_gain16 = 16;
	} else {
		if (cap_gain16_shutter > (cap_bandfilt*cap_maxband*16)) {
			/* exposure reach max */
			cap_shutter = cap_bandfilt*cap_maxband;
			cap_gain16 = cap_gain16_shutter / cap_shutter;
		} else {
			/* 1/100 < cap_shutter =< max, cap_shutter = n/100 */
			cap_shutter =
				((int)(cap_gain16_shutter/16/cap_bandfilt))
				* cap_bandfilt;
			cap_gain16 = cap_gain16_shutter / cap_shutter;
		}
	}

	/* write capture gain */
	ov5640_set_gain(&ov5640_data, cap_gain16);

	/* write capture shutter */
	if (cap_shutter > (cap_VTS - 4)) {
		cap_VTS = cap_shutter + 4;
		ov5640_set_vts(&ov5640_data, cap_VTS);
	}

	ov5640_set_exposure(&ov5640_data, cap_shutter);

	/* skip 2 vysnc: start capture at 3rd vsync
	 * frame rate of QSXGA and 1080P is 7.5fps: 1/7.5 * 2
	 */
	pr_warning("ov5640: the actual frame rate of %s is 7.5fps\n",
		mode == OV5640_MODE_1080P_1920_1080 ? "1080P" : "QSXGA");
	msleep(267);
err:
	return retval;
}

static int ov5640_change_mode(enum ov5640_frame_rate frame_rate,
			    enum ov5640_mode_id mode)
{
	int retval = 0;

	if (mode > OV5640_NUM_MODES || mode < OV5640_MODE_MIN) {
		pr_err("Wrong ov5640 mode detected!\n");
		return -1;
	}

	if (mode == OV5640_MODE_1080P_1920_1080 ||
			mode == OV5640_MODE_QSXGA_2592_1944) {
		/* change to scaling mode go through exposure calucation
		 * image size above 1280 * 960 is scaling mode */
		retval = ov5640_change_mode_exposure_calc(frame_rate, mode);
	} else {
		/* change back to subsampling modem download firmware directly
		 * image size below 1280 * 960 is subsampling mode */
		retval = ov5640_change_mode_direct(frame_rate, mode);
	}

	return retval;
}

static int ov5640_try_fmt_internal(struct v4l2_subdev *sd,
				   struct v4l2_mbus_framefmt *fmt,
				   enum ov5640_frame_rate fr,
				   const struct ov5640_mode_info **new_mode)
{
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	const struct ov5640_mode_info *mode;

	mode = ov5640_find_mode(sensor, fr, fmt->width, fmt->height, true);
	if (!mode)
		return -EINVAL;

	fmt->width = mode->width;
	fmt->height = mode->height;
	fmt->code = sensor->fmt.code;

	if (new_mode)
		*new_mode = mode;
	return 0;
}

/*!
 * ov5640_s_power - V4L2 sensor interface handler for VIDIOC_S_POWER ioctl
 * @s: pointer to standard V4L2 device structure
 * @on: indicates power mode (on or off)
 *
 * Turns the power on or off, depending on the value of on and returns the
 * appropriate error code.
 */
static int ov5640_s_power(struct v4l2_subdev *sd, int on)
{
	struct ov5640_dev *sensor = to_ov5640_dev(sd);

	if (on)
		clk_enable(ov5640_data.sensor_clk);
	else
		clk_disable(ov5640_data.sensor_clk);

	sensor->on = on;

	return 0;
}

static int ov5640_try_frame_interval(struct ov5640_dev *sensor,
				     struct v4l2_fract *fi,
				     u32 width, u32 height)
{
	const struct ov5640_mode_info *mode;
	u32 minfps, maxfps, fps;
	int ret;

	minfps = ov5640_framerates[OV5640_15_FPS];
	maxfps = ov5640_framerates[OV5640_30_FPS];

	if (fi->numerator == 0) {
		fi->denominator = maxfps;
		fi->numerator = 1;
		return OV5640_30_FPS;
	}

	fps = DIV_ROUND_CLOSEST(fi->denominator, fi->numerator);

	fi->numerator = 1;
	if (fps > maxfps)
		fi->denominator = maxfps;
	else if (fps < minfps)
		fi->denominator = minfps;
	else if (2 * fps >= 2 * minfps + (maxfps - minfps))
		fi->denominator = maxfps;
	else
		fi->denominator = minfps;

	ret = (fi->denominator == minfps) ? OV5640_15_FPS : OV5640_30_FPS;

	mode = ov5640_find_mode(sensor, ret, width, height, false);
	return mode ? ret : -EINVAL;
}

/*!
 * ov5640_g_parm - V4L2 sensor interface handler for VIDIOC_G_PARM ioctl
 * @s: pointer to standard V4L2 sub device structure
 * @a: pointer to standard V4L2 VIDIOC_G_PARM ioctl structure
 *
 * Returns the sensor's video CAPTURE parameters.
 */
static int ov5640_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *a)
{
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	struct v4l2_captureparm *cparm = &a->parm.capture;
	int ret = 0;

	switch (a->type) {
	/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		memset(a, 0, sizeof(*a));
		a->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		cparm->capability = sensor->streamcap.capability;
		cparm->timeperframe = sensor->streamcap.timeperframe;
		cparm->capturemode = sensor->streamcap.capturemode;
		ret = 0;
		break;

	/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		ret = -EINVAL;
		break;

	default:
		pr_debug("   type is unknown - %d\n", a->type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

/*!
 * ov5460_s_parm - V4L2 sensor interface handler for VIDIOC_S_PARM ioctl
 * @s: pointer to standard V4L2 sub device structure
 * @a: pointer to standard V4L2 VIDIOC_S_PARM ioctl structure
 *
 * Configures the sensor to use the input parameters, if possible.  If
 * not possible, reverts to the old parameters and returns the
 * appropriate error code.
 */
static int ov5640_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *a)
{
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	struct v4l2_fract *timeperframe = &a->parm.capture.timeperframe;
	u32 tgt_fps;	/* target frames per secound */
	enum ov5640_frame_rate frame_rate;
	int ret = 0;

	switch (a->type) {
	/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		/* Check that the new frame rate is allowed. */
		if ((timeperframe->numerator == 0) ||
		    (timeperframe->denominator == 0)) {
			timeperframe->denominator = DEFAULT_FPS;
			timeperframe->numerator = 1;
		}

		tgt_fps = timeperframe->denominator /
			  timeperframe->numerator;

		if (tgt_fps > MAX_FPS) {
			timeperframe->denominator = MAX_FPS;
			timeperframe->numerator = 1;
		} else if (tgt_fps < MIN_FPS) {
			timeperframe->denominator = MIN_FPS;
			timeperframe->numerator = 1;
		}

		/* Actual frame rate we use */
		tgt_fps = timeperframe->denominator /
			  timeperframe->numerator;

		if (tgt_fps == 15)
			frame_rate = OV5640_15_FPS;
		else if (tgt_fps == 30)
			frame_rate = OV5640_30_FPS;
		else {
			pr_err(" The camera frame rate is not supported!\n");
			goto error;
		}

		ret = ov5640_change_mode(frame_rate,
				a->parm.capture.capturemode);
		if (ret < 0)
			goto error;

		sensor->streamcap.timeperframe = *timeperframe;
		sensor->streamcap.capturemode = a->parm.capture.capturemode;

		break;

	/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		pr_debug("   type is not " \
			"V4L2_BUF_TYPE_VIDEO_CAPTURE but %d\n",
			a->type);
		ret = -EINVAL;
		break;

	default:
		pr_debug("   type is unknown - %d\n", a->type);
		ret = -EINVAL;
		break;
	}

error:
	return ret;
}

static int ov5640_set_fmt(struct v4l2_subdev *sd,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_format *format)
{
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	const struct ov5640_mode_info *new_mode;
	int ret;

	if (format->pad != 0)
		return -EINVAL;

	mutex_lock(&sensor->lock);

	if (sensor->streaming) {
		ret = -EBUSY;
		goto out;
	}

	ret = ov5640_try_fmt_internal(sd, &format->format,
				      sensor->current_fr, &new_mode);
	if (ret)
		goto out;

	if (format->which == V4L2_SUBDEV_FORMAT_TRY) {
		struct v4l2_mbus_framefmt *fmt =
			v4l2_subdev_get_try_format(sd, cfg, 0);

		*fmt = format->format;
		goto out;
	}

	sensor->current_mode = new_mode;
	sensor->fmt = format->format;
	sensor->pending_mode_change = true;
out:
	mutex_unlock(&sensor->lock);
	return ret;
	
}

static int ov5640_get_fmt(struct v4l2_subdev *sd,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_format *format)
{
#if 0
	struct v4l2_mbus_framefmt *mf = &format->format;
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	const struct ov5640_datafmt *fmt = sensor->fmt;

	if (format->pad)
		return -EINVAL;

	mf->code	= fmt->code;
	mf->colorspace	= fmt->colorspace;
	mf->field	= V4L2_FIELD_NONE;

	return 0;
#endif
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	struct v4l2_mbus_framefmt *fmt;

	if (format->pad != 0)
		return -EINVAL;

	mutex_lock(&sensor->lock);

	if (format->which == V4L2_SUBDEV_FORMAT_TRY)
		fmt = v4l2_subdev_get_try_format(&sensor->sd, cfg,
						 format->pad);
	else
		fmt = &sensor->fmt;

	format->format = *fmt;

	mutex_unlock(&sensor->lock);

	return 0;
}

static int ov5640_enum_code(struct v4l2_subdev *sd,
			    struct v4l2_subdev_pad_config *cfg,
			    struct v4l2_subdev_mbus_code_enum *code)
{
#if 0
	if (code->pad || code->index >= ARRAY_SIZE(ov5640_colour_fmts))
		return -EINVAL;

	code->code = ov5640_colour_fmts[code->index].code;
	return 0;
#endif
	struct ov5640_dev *sensor = to_ov5640_dev(sd);

	if (code->pad != 0)
		return -EINVAL;
	if (code->index != 0)
		return -EINVAL;

	code->code = sensor->fmt.code;

	return 0;
}

/*!
 * ov5640_enum_framesizes - V4L2 sensor interface handler for
 *			   VIDIOC_ENUM_FRAMESIZES ioctl
 * @s: pointer to standard V4L2 device structure
 * @fsize: standard V4L2 VIDIOC_ENUM_FRAMESIZES ioctl structure
 *
 * Return 0 if successful, otherwise -EINVAL.
 */
static int ov5640_enum_framesizes(struct v4l2_subdev *sd,
			       struct v4l2_subdev_pad_config *cfg,
			       struct v4l2_subdev_frame_size_enum *fse)
{
	if (fse->index > OV5640_NUM_MODES)
		return -EINVAL;

	fse->max_width =
			max(ov5640_mode_data[0][fse->index].width,
			    ov5640_mode_data[1][fse->index].width);
	fse->min_width = fse->max_width;
	fse->max_height =
			max(ov5640_mode_data[0][fse->index].height,
			    ov5640_mode_data[1][fse->index].height);
	fse->min_height = fse->max_height;
	return 0;
}

/*!
 * ov5640_enum_frameinterval - V4L2 sensor interface handler for
 *			       VIDIOC_ENUM_FRAMEINTERVALS ioctl
 * @s: pointer to standard V4L2 device structure
 * @fival: standard V4L2 VIDIOC_ENUM_FRAMEINTERVALS ioctl structure
 *
 * Return 0 if successful, otherwise -EINVAL.
 */
static int ov5640_enum_frameinterval(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_frame_interval_enum *fie)
{
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	struct v4l2_fract tpf;
	int ret;

	if (fie->pad != 0)
		return -EINVAL;
	if (fie->index >= OV5640_NUM_FRAMERATES)
		return -EINVAL;

	tpf.numerator = 1;
	tpf.denominator = ov5640_framerates[fie->index];

	ret = ov5640_try_frame_interval(sensor, &tpf,
					fie->width, fie->height);
	if (ret < 0)
		return -EINVAL;

	fie->interval = tpf;
	return 0;
}

static int ov5640_set_mode(struct ov5640_dev *sensor,
			   const struct ov5640_mode_info *orig_mode)
{
	const struct ov5640_mode_info *mode = sensor->current_mode;
	enum ov5640_downsize_mode dn_mode, orig_dn_mode;
	enum ov5640_mode_id mode_id;
	enum ov5640_frame_rate fr;
	int ret;

	dn_mode = mode->dn_mode;
	orig_dn_mode = orig_mode->dn_mode;

	mode_id = mode->mode;
	fr = mode->framerate;
	
	/* auto gain and exposure must be turned off when changing modes */
	ret = __v4l2_ctrl_s_ctrl(sensor->ctrls.auto_gain, 0);
	if (ret)
		return ret;
	ret = __v4l2_ctrl_s_ctrl(sensor->ctrls.auto_exp, V4L2_EXPOSURE_MANUAL);
	if (ret)
		return ret;

	if ((dn_mode == SUBSAMPLING && orig_dn_mode == SCALING) ||
	    (dn_mode == SCALING && orig_dn_mode == SUBSAMPLING)) {
		/*
		 * change between subsampling and scaling
		 * go through exposure calucation
		 */
		//ret = ov5640_set_mode_exposure_calc(sensor, mode);
		ret = ov5640_change_mode_exposure_calc(fr, mode_id);
	} else {
		/*
		 * change inside subsampling or scaling
		 * download firmware directly
		 */
		//ret = ov5640_set_mode_direct(sensor, mode);
		ret = ov5640_change_mode_direct(fr, mode_id);
	}

	if (ret < 0)
		return ret;

	ret = ov5640_set_ae_target(sensor, sensor->ae_target);
	if (ret < 0)
		return ret;
	ret = ov5640_get_light_freq(sensor);
	if (ret < 0)
		return ret;
	ov5640_set_bandingfilter();
	
	sensor->pending_mode_change = false;

	return 0;
}

static int ov5640_set_stream(struct ov5640_dev *sensor, bool on)
{
	int ret;

	ret = ov5640_mod_reg(sensor, OV5640_REG_MIPI_CTRL00, BIT(5),
			     on ? 0 : BIT(5));
	if (ret)
		return ret;
	ret = ov5640_write_reg(sensor, OV5640_REG_PAD_OUTPUT00,
			       on ? 0x00 : 0x70);
	if (ret)
		return ret;

	return ov5640_write_reg(sensor, OV5640_REG_FRAME_CTRL01,
				on ? 0x00 : 0x0f);
}

static int ov5640_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	int ret = 0;

	mutex_lock(&sensor->lock);

	if (sensor->streaming == !enable) {
		if (enable && sensor->pending_mode_change) {
			ret = ov5640_set_mode(sensor, sensor->current_mode);
			if (ret)
				goto out;
		}

		ret = ov5640_set_stream(sensor, enable);
		if (!ret)
			sensor->streaming = enable;
	}
out:
	mutex_unlock(&sensor->lock);
	return ret;
}

static int ov5640_set_clk_rate(void)
{
	u32 tgt_xclk;	/* target xclk */
	int ret;

	/* mclk */
	tgt_xclk = ov5640_data.mclk;
	tgt_xclk = min(tgt_xclk, (u32)OV5640_XCLK_MAX);
	tgt_xclk = max(tgt_xclk, (u32)OV5640_XCLK_MIN);
	ov5640_data.mclk = tgt_xclk;

	pr_debug("   Setting mclk to %d MHz\n", tgt_xclk / 1000000);
	ret = clk_set_rate(ov5640_data.sensor_clk, ov5640_data.mclk);
	if (ret < 0)
		pr_debug("set rate filed, rate=%d\n", ov5640_data.mclk);
	return ret;
}

/*!
 * dev_init - V4L2 sensor init
 * @s: pointer to standard V4L2 device structure
 *
 */
static int init_device(void)
{
	u32 tgt_xclk;	/* target xclk */
	u32 tgt_fps;	/* target frames per secound */
	enum ov5640_frame_rate frame_rate;
	int ret;

	ov5640_data.on = true;

	/* mclk */
	tgt_xclk = ov5640_data.mclk;

	/* Default camera frame rate is set in probe */
	tgt_fps = ov5640_data.streamcap.timeperframe.denominator /
		  ov5640_data.streamcap.timeperframe.numerator;

	if (tgt_fps == 15)
		frame_rate = OV5640_15_FPS;
	else if (tgt_fps == 30)
		frame_rate = OV5640_30_FPS;
	else
		return -EINVAL; /* Only support 15fps or 30fps now. */

	ret = ov5640_init_mode();

	return ret;
}

/*
 * Sensor Controls.
 */

static int ov5640_set_ctrl_hue(struct ov5640_dev *sensor, int value)
{
	int ret;

	if (value) {
		ret = ov5640_mod_reg(sensor, OV5640_REG_SDE_CTRL0,
				     BIT(0), BIT(0));
		if (ret)
			return ret;
		ret = ov5640_write_reg16(sensor, OV5640_REG_SDE_CTRL1, value);
	} else {
		ret = ov5640_mod_reg(sensor, OV5640_REG_SDE_CTRL0, BIT(0), 0);
	}

	return ret;
}

static int ov5640_set_ctrl_contrast(struct ov5640_dev *sensor, int value)
{
	int ret;

	if (value) {
		ret = ov5640_mod_reg(sensor, OV5640_REG_SDE_CTRL0,
				     BIT(2), BIT(2));
		if (ret)
			return ret;
		ret = ov5640_write_reg(sensor, OV5640_REG_SDE_CTRL5,
				       value & 0xff);
	} else {
		ret = ov5640_mod_reg(sensor, OV5640_REG_SDE_CTRL0, BIT(2), 0);
	}

	return ret;
}

static int ov5640_set_ctrl_saturation(struct ov5640_dev *sensor, int value)
{
	int ret;

	if (value) {
		ret = ov5640_mod_reg(sensor, OV5640_REG_SDE_CTRL0,
				     BIT(1), BIT(1));
		if (ret)
			return ret;
		ret = ov5640_write_reg(sensor, OV5640_REG_SDE_CTRL3,
				       value & 0xff);
		if (ret)
			return ret;
		ret = ov5640_write_reg(sensor, OV5640_REG_SDE_CTRL4,
				       value & 0xff);
	} else {
		ret = ov5640_mod_reg(sensor, OV5640_REG_SDE_CTRL0, BIT(1), 0);
	}

	return ret;
}

static int ov5640_set_ctrl_white_balance(struct ov5640_dev *sensor, int awb)
{
	int ret;

	ret = ov5640_mod_reg(sensor, OV5640_REG_AWB_MANUAL_CTRL,
			     BIT(0), awb ? 0 : 1);
	if (ret)
		return ret;

	if (!awb) {
		u16 red = (u16)sensor->ctrls.red_balance->val;
		u16 blue = (u16)sensor->ctrls.blue_balance->val;

		ret = ov5640_write_reg16(sensor, OV5640_REG_AWB_R_GAIN, red);
		if (ret)
			return ret;
		ret = ov5640_write_reg16(sensor, OV5640_REG_AWB_B_GAIN, blue);
	}

	return ret;
}

static int ov5640_set_ctrl_exposure(struct ov5640_dev *sensor, int exp)
{
	struct ov5640_ctrls *ctrls = &sensor->ctrls;
	bool auto_exposure = (exp == V4L2_EXPOSURE_AUTO);
	int ret = 0;

	if (ctrls->auto_exp->is_new) {
		ret = ov5640_mod_reg(sensor, OV5640_REG_AEC_PK_MANUAL,
				     BIT(0), auto_exposure ? 0 : BIT(0));
		if (ret)
			return ret;
	}

	if (!auto_exposure && ctrls->exposure->is_new) {
		u16 max_exp;

		ret = ov5640_read_reg16(sensor, OV5640_REG_AEC_PK_VTS,
					&max_exp);
		if (ret)
			return ret;
		ret = ov5640_get_vts(sensor);
		if (ret < 0)
			return ret;
		max_exp += ret;

		if (ctrls->exposure->val < max_exp)
			ret = ov5640_set_exposure(sensor, ctrls->exposure->val);
	}

	return ret;
}

static int ov5640_set_ctrl_gain(struct ov5640_dev *sensor, int auto_gain)
{
	struct ov5640_ctrls *ctrls = &sensor->ctrls;
	int ret = 0;

	if (ctrls->auto_gain->is_new) {
		ret = ov5640_mod_reg(sensor, OV5640_REG_AEC_PK_MANUAL,
				     BIT(1), ctrls->auto_gain->val ? 0 : BIT(1));
		if (ret)
			return ret;
	}

	if (!auto_gain && ctrls->gain->is_new) {
		u16 gain = (u16)ctrls->gain->val;

		ret = ov5640_set_gain(sensor, gain);
	}

	return ret;
}

static int ov5640_set_ctrl_test_pattern(struct ov5640_dev *sensor, int value)
{
	return ov5640_mod_reg(sensor, OV5640_REG_PRE_ISP_TEST_SET1,
			      0xa4, value ? 0xa4 : 0);
}

static int ov5640_g_frame_interval(struct v4l2_subdev *sd,
				   struct v4l2_subdev_frame_interval *fi)
{
	struct ov5640_dev *sensor = to_ov5640_dev(sd);

	mutex_lock(&sensor->lock);
	fi->interval = sensor->frame_interval;
	mutex_unlock(&sensor->lock);

	return 0;
}

static int ov5640_s_frame_interval(struct v4l2_subdev *sd,
				   struct v4l2_subdev_frame_interval *fi)
{
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	const struct ov5640_mode_info *mode;
	int frame_rate, ret = 0;

	if (fi->pad != 0)
		return -EINVAL;

	mutex_lock(&sensor->lock);

	if (sensor->streaming) {
		ret = -EBUSY;
		goto out;
	}

	mode = sensor->current_mode;

	frame_rate = ov5640_try_frame_interval(sensor, &fi->interval,
					       mode->width, mode->height);
	if (frame_rate < 0)
		frame_rate = OV5640_15_FPS;

	sensor->current_fr = frame_rate;
	sensor->frame_interval = fi->interval;
	sensor->pending_mode_change = true;
out:
	mutex_unlock(&sensor->lock);
	return ret;
}

static int ov5640_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	struct v4l2_subdev *sd = ctrl_to_sd(ctrl);
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	int val;

	/* v4l2_ctrl_lock() locks our own mutex */

	switch (ctrl->id) {
	case V4L2_CID_AUTOGAIN:
		if (!ctrl->val)
			return 0;
		val = ov5640_get_gain(sensor);
		if (val < 0)
			return val;
		sensor->ctrls.gain->val = val;
		break;
	case V4L2_CID_EXPOSURE_AUTO:
		if (ctrl->val == V4L2_EXPOSURE_MANUAL)
			return 0;
		val = ov5640_get_exposure(sensor);
		if (val < 0)
			return val;
		sensor->ctrls.exposure->val = val;
		break;
	}

	return 0;
}
static int ov5640_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct v4l2_subdev *sd = ctrl_to_sd(ctrl);
	struct ov5640_dev *sensor = to_ov5640_dev(sd);
	int ret;

	/* v4l2_ctrl_lock() locks our own mutex */

	/*
	 * If the device is not powered up by the host driver do
	 * not apply any controls to H/W at this time. Instead
	 * the controls will be restored right after power-up.
	 */
	if (sensor->on == 0)
		return 0;

	switch (ctrl->id) {
	case V4L2_CID_AUTOGAIN:
		ret = ov5640_set_ctrl_gain(sensor, ctrl->val);
		break;
	case V4L2_CID_EXPOSURE_AUTO:
		ret = ov5640_set_ctrl_exposure(sensor, ctrl->val);
		break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
		ret = ov5640_set_ctrl_white_balance(sensor, ctrl->val);
		break;
	case V4L2_CID_HUE:
		ret = ov5640_set_ctrl_hue(sensor, ctrl->val);
		break;
	case V4L2_CID_CONTRAST:
		ret = ov5640_set_ctrl_contrast(sensor, ctrl->val);
		break;
	case V4L2_CID_SATURATION:
		ret = ov5640_set_ctrl_saturation(sensor, ctrl->val);
		break;
	case V4L2_CID_TEST_PATTERN:
		ret = ov5640_set_ctrl_test_pattern(sensor, ctrl->val);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static struct v4l2_subdev_video_ops ov5640_subdev_video_ops = {
	.g_parm = ov5640_g_parm,
	.s_parm = ov5640_s_parm,
	.g_frame_interval = ov5640_g_frame_interval,
	.s_frame_interval = ov5640_s_frame_interval,
	.s_stream = ov5640_s_stream,
};

static const struct v4l2_subdev_pad_ops ov5640_subdev_pad_ops = {
	.enum_frame_size       = ov5640_enum_framesizes,
	.enum_frame_interval   = ov5640_enum_frameinterval,
	.enum_mbus_code        = ov5640_enum_code,
	.set_fmt               = ov5640_set_fmt,
	.get_fmt               = ov5640_get_fmt,
};

static struct v4l2_subdev_core_ops ov5640_subdev_core_ops = {
	.s_power	= ov5640_s_power,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.g_register	= ov5640_get_register,
	.s_register	= ov5640_set_register,
#endif
};

static struct v4l2_subdev_ops ov5640_subdev_ops = {
	.core	= &ov5640_subdev_core_ops,
	.video	= &ov5640_subdev_video_ops,
	.pad	= &ov5640_subdev_pad_ops,
};

/* Ctrl ops just for probe function. Used in ov5640_init_controls().*/
static const struct v4l2_ctrl_ops ov5640_ctrl_ops = {
	.g_volatile_ctrl = ov5640_g_volatile_ctrl,
	.s_ctrl = ov5640_s_ctrl,
};

static const char * const test_pattern_menu[] = {
	"Disabled",
	"Color bars",
};

static int ov5640_init_controls(struct ov5640_dev *sensor)
{
	const struct v4l2_ctrl_ops *ops = &ov5640_ctrl_ops;
	struct ov5640_ctrls *ctrls = &sensor->ctrls;
	struct v4l2_ctrl_handler *hdl = &ctrls->handler;
	int ret;

	v4l2_ctrl_handler_init(hdl, 32);

	/* we can use our own mutex for the ctrl lock */
	hdl->lock = &sensor->lock;

	/* Auto/manual white balance */
	ctrls->auto_wb = v4l2_ctrl_new_std(hdl, ops,
					   V4L2_CID_AUTO_WHITE_BALANCE,
					   0, 1, 1, 1);
	ctrls->blue_balance = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_BLUE_BALANCE,
						0, 4095, 1, 0);
	ctrls->red_balance = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_RED_BALANCE,
					       0, 4095, 1, 0);
	/* Auto/manual exposure */
	ctrls->auto_exp = v4l2_ctrl_new_std_menu(hdl, ops,
						 V4L2_CID_EXPOSURE_AUTO,
						 V4L2_EXPOSURE_MANUAL, 0,
						 V4L2_EXPOSURE_AUTO);
	ctrls->exposure = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_EXPOSURE,
					    0, 65535, 1, 0);
	/* Auto/manual gain */
	ctrls->auto_gain = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_AUTOGAIN,
					     0, 1, 1, 1);
	ctrls->gain = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_GAIN,
					0, 1023, 1, 0);

	ctrls->saturation = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_SATURATION,
					      0, 255, 1, 64);
	ctrls->hue = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_HUE,
				       0, 359, 1, 0);
	ctrls->contrast = v4l2_ctrl_new_std(hdl, ops, V4L2_CID_CONTRAST,
					    0, 255, 1, 0);
	ctrls->test_pattern =
		v4l2_ctrl_new_std_menu_items(hdl, ops, V4L2_CID_TEST_PATTERN,
					     ARRAY_SIZE(test_pattern_menu) - 1,
					     0, 0, test_pattern_menu);

	if (hdl->error) {
		ret = hdl->error;
		goto free_ctrls;
	}

	ctrls->gain->flags |= V4L2_CTRL_FLAG_VOLATILE;
	ctrls->exposure->flags |= V4L2_CTRL_FLAG_VOLATILE;

	v4l2_ctrl_auto_cluster(3, &ctrls->auto_wb, 0, false);
	v4l2_ctrl_auto_cluster(2, &ctrls->auto_gain, 0, true);
	v4l2_ctrl_auto_cluster(2, &ctrls->auto_exp, 1, true);

	sensor->sd.ctrl_handler = hdl;
	return 0;

free_ctrls:
	v4l2_ctrl_handler_free(hdl);
	return ret;
}

/*!
 * ov5640 I2C probe function
 *
 * @param adapter            struct i2c_adapter *
 * @return  Error code indicating success or failure
 */
static int ov5640_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct pinctrl *pinctrl;
	struct device *dev = &client->dev;
	int retval;
	u8 chip_id_high, chip_id_low;

	/* ov5640 pinctrl */
	pinctrl = devm_pinctrl_get_select_default(dev);
	if (IS_ERR(pinctrl)) {
		dev_err(dev, "setup pinctrl failed\n");
		return PTR_ERR(pinctrl);
	}

	/* request power down pin */
	pwn_gpio = of_get_named_gpio(dev->of_node, "pwn-gpios", 0);
	if (!gpio_is_valid(pwn_gpio)) {
		dev_err(dev, "no sensor pwdn pin available\n");
		return -ENODEV;
	}
	retval = devm_gpio_request_one(dev, pwn_gpio, GPIOF_OUT_INIT_HIGH,
					"ov5640_pwdn");
	if (retval < 0)
		return retval;

	/* request reset pin */
	rst_gpio = of_get_named_gpio(dev->of_node, "rst-gpios", 0);
	if (!gpio_is_valid(rst_gpio)) {
		dev_err(dev, "no sensor reset pin available\n");
		return -EINVAL;
	}
	retval = devm_gpio_request_one(dev, rst_gpio, GPIOF_OUT_INIT_HIGH,
					"ov5640_reset");
	if (retval < 0)
		return retval;

	/* Set initial values for the sensor struct. */
	memset(&ov5640_data, 0, sizeof(ov5640_data));
	ov5640_data.sensor_clk = devm_clk_get(dev, "csi_mclk");
	if (IS_ERR(ov5640_data.sensor_clk)) {
		dev_err(dev, "get mclk failed\n");
		return PTR_ERR(ov5640_data.sensor_clk);
	}

	retval = of_property_read_u32(dev->of_node, "mclk",
					&ov5640_data.mclk);
	if (retval) {
		dev_err(dev, "mclk frequency is invalid\n");
		return retval;
	}

	retval = of_property_read_u32(dev->of_node, "mclk_source",
					(u32 *) &(ov5640_data.mclk_source));
	if (retval) {
		dev_err(dev, "mclk_source invalid\n");
		return retval;
	}

	retval = of_property_read_u32(dev->of_node, "csi_id",
					&(ov5640_data.csi));
	if (retval) {
		dev_err(dev, "csi_id invalid\n");
		return retval;
	}

	/* Set mclk rate before clk on */
	ov5640_set_clk_rate();

	clk_prepare_enable(ov5640_data.sensor_clk);

	ov5640_data.io_init = ov5640_reset;
	ov5640_data.i2c_client = client;
	ov5640_data.fmt.code = MEDIA_BUS_FMT_UYVY8_2X8;
	ov5640_data.fmt.width = 640;
	ov5640_data.fmt.height = 480;
	ov5640_data.fmt.field = V4L2_FIELD_NONE;
	ov5640_data.streamcap.capability = V4L2_MODE_HIGHQUALITY |
					   V4L2_CAP_TIMEPERFRAME;
	ov5640_data.streamcap.capturemode = 0;
	ov5640_data.streamcap.timeperframe.denominator = DEFAULT_FPS;
	ov5640_data.streamcap.timeperframe.numerator = 1;
	ov5640_data.current_mode =
		&ov5640_mode_data[OV5640_30_FPS][OV5640_MODE_VGA_640_480];
	ov5640_data.ae_target = 52;
	
	ov5640_regulator_enable(&client->dev);

	ov5640_reset();

	ov5640_power_down(0);

	retval = ov5640_read_reg(&ov5640_data, OV5640_CHIP_ID_HIGH_BYTE, &chip_id_high);
	if (retval < 0 || chip_id_high != 0x56) {
		clk_disable_unprepare(ov5640_data.sensor_clk);
		pr_warning("camera ov5640 is not found\n");
		return -ENODEV;
	}
	retval = ov5640_read_reg(&ov5640_data, OV5640_CHIP_ID_LOW_BYTE, &chip_id_low);
	if (retval < 0 || chip_id_low != 0x40) {
		clk_disable_unprepare(ov5640_data.sensor_clk);
		pr_warning("camera ov5640 is not found\n");
		return -ENODEV;
	}

	retval = init_device();
	if (retval < 0) {
		clk_disable_unprepare(ov5640_data.sensor_clk);
		pr_warning("camera ov5640 init failed\n");
		ov5640_power_down(1);
		return retval;
	}

	clk_disable(ov5640_data.sensor_clk);

	v4l2_i2c_subdev_init(&ov5640_data.sd, client, &ov5640_subdev_ops);

	mutex_init(&ov5640_data.lock);
	
	retval = ov5640_init_controls(&ov5640_data);
	if (retval){
		mutex_destroy(&ov5640_data.lock);
		dev_err(&client->dev,
					"%s Init controls failed, ret=%d\n", __func__, retval);
		return retval;
	}
	
	retval = v4l2_async_register_subdev(&ov5640_data.sd);
	if (retval < 0)
		dev_err(&client->dev,
					"%s--Async register failed, ret=%d\n", __func__, retval);

	pr_info("camera ov5640, is found\n");
	return retval;
}

/*!
 * ov5640 I2C detach function
 *
 * @param client            struct i2c_client *
 * @return  Error code indicating success or failure
 */
static int ov5640_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);

	v4l2_async_unregister_subdev(sd);

	clk_unprepare(ov5640_data.sensor_clk);

	ov5640_power_down(1);

	if (analog_regulator)
		regulator_disable(analog_regulator);

	if (core_regulator)
		regulator_disable(core_regulator);

	if (io_regulator)
		regulator_disable(io_regulator);

	return 0;
}

module_i2c_driver(ov5640_i2c_driver);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("OV5640 Camera Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
MODULE_ALIAS("CSI");
