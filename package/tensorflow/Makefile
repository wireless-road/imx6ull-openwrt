#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tensorflow
PKG_VERSION:=1
PKG_RELEASE:=
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_BUILD_PARALLEL:=1


CMAKE_BINARY_SUBDIR=tensorflow-sdk-build
SOURCE_SUBDIR=tensorflow-sdk-source
CMAKE_SOURCE_SUBDIR=tensorflow-sdk-source/tensorflow/lite/examples/minimal

CMAKE_OPTIONS=-DCMAKE_BUILD_TYPE=MINSIZEREL -DCMAKE_SYSTEM_PROCESSOR=armv7 #-DTFLITE_ENABLE_XNNPACK=OFF

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/tensorflow
  SECTION:=tensorflow
  CATEGORY:=Examples
  TITLE:=tensorflow
  DEPENDS:=+libstdcpp
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)
	mkdir -p $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/
	$(MAKE) --version
	cmake --version
	whereis cmake
endef

define Package/tensorflow/description
 Tensorflow-lite for embedded linux minimal working example.
endef

define Build/Configure
	$(Build/Configure/Default)
	$(Build/Patch)
endef

define Build/Compile
	$(MAKE_VARS) $(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR) $(MAKE_FLAGS)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/tensorflow $(1)/usr/include/tensorflow/lite
	$(CP) $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/tensorflow/lite/*.h $(1)/usr/include/tensorflow/lite
	$(INSTALL_DIR) $(1)/usr/include/tensorflow/lite/core
	$(CP) $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/tensorflow/lite/core/*.h $(1)/usr/include/tensorflow/lite/core
	$(INSTALL_DIR) $(1)/usr/include/tensorflow/lite/core/api
	$(CP) $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/tensorflow/lite/core/api/*.h $(1)/usr/include/tensorflow/lite/core/api
	$(INSTALL_DIR) $(1)/usr/include/tensorflow/lite/c/
	$(CP) $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/tensorflow/lite/c/*.h $(1)/usr/include/tensorflow/lite/c/
	$(INSTALL_DIR) $(1)/usr/include/tensorflow/lite/experimental/resource
	$(CP) $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/tensorflow/lite/experimental/resource/*.h $(1)/usr/include/tensorflow/lite/experimental/resource
	$(INSTALL_DIR) $(1)/usr/include/tensorflow/lite/kernels
	$(CP) $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/tensorflow/lite/kernels/*.h $(1)/usr/include/tensorflow/lite/kernels
	$(INSTALL_DIR) $(1)/usr/include/tensorflow/lite/kernels/internal
	$(CP) $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/tensorflow/lite/kernels/internal/*.h $(1)/usr/include/tensorflow/lite/kernels/internal
	$(INSTALL_DIR) $(1)/usr/include/tensorflow/lite/schema
	$(CP) $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/tensorflow/lite/schema/*.h $(1)/usr/include/tensorflow/lite/schema
	$(INSTALL_DIR) $(1)/usr/include/flatbuffers
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/flatbuffers/include/flatbuffers/*.h $(1)/usr/include/flatbuffers/
	$(INSTALL_DIR) $(1)/usr/include/absl/synchronization
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/synchronization/*.h $(1)/usr/include/absl/synchronization
	$(INSTALL_DIR) $(1)/usr/include/absl/synchronization/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/synchronization/internal/*.h $(1)/usr/include/absl/synchronization/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/hash
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/hash/*.h $(1)/usr/include/absl/hash
	$(INSTALL_DIR) $(1)/usr/include/absl/hash/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/hash/internal/*.h $(1)/usr/include/absl/hash/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/cleanup
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/cleanup/*.h $(1)/usr/include/absl/cleanup
	$(INSTALL_DIR) $(1)/usr/include/absl/cleanup/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/cleanup/internal/*.h $(1)/usr/include/absl/cleanup/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/status
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/status/*.h $(1)/usr/include/absl/status
	$(INSTALL_DIR) $(1)/usr/include/absl/status/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/status/internal/*.h $(1)/usr/include/absl/status/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/types
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/types/*.h $(1)/usr/include/absl/types
	$(INSTALL_DIR) $(1)/usr/include/absl/types/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/types/internal/*.h $(1)/usr/include/absl/types/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/strings
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/strings/*.h $(1)/usr/include/absl/strings
	$(INSTALL_DIR) $(1)/usr/include/absl/strings/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/strings/internal/*.h $(1)/usr/include/absl/strings/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/strings/internal/str_format
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/strings/internal/str_format/*.h $(1)/usr/include/absl/strings/internal/str_format
	$(INSTALL_DIR) $(1)/usr/include/absl/flags
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/flags/*.h $(1)/usr/include/absl/flags
	$(INSTALL_DIR) $(1)/usr/include/absl/base
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/base/*.h $(1)/usr/include/absl/base
	$(INSTALL_DIR) $(1)/usr/include/absl/base/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/base/internal/*.h $(1)/usr/include/absl/base/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/base/internal/*.inc $(1)/usr/include/absl/base/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/time
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/time/*.h $(1)/usr/include/absl/time
	$(INSTALL_DIR) $(1)/usr/include/absl/time/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/time/internal/*.h $(1)/usr/include/absl/time/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/time/internal/*.inc $(1)/usr/include/absl/time/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/time/internal/cctz/include/cctz/
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/time/internal/cctz/include/cctz/*.h $(1)/usr/include/absl/time/internal/cctz/include/cctz
	$(INSTALL_DIR) $(1)/usr/include/absl/random
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/random/*.h $(1)/usr/include/absl/random
	$(INSTALL_DIR) $(1)/usr/include/absl/random/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/random/internal/*.h $(1)/usr/include/absl/random/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/flags
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/flags/*.h $(1)/usr/include/absl/flags
	$(INSTALL_DIR) $(1)/usr/include/absl/flags/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/flags/internal/*.h $(1)/usr/include/absl/flags/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/random
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/random/*.h $(1)/usr/include/absl/random
	$(INSTALL_DIR) $(1)/usr/include/absl/random/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/random/internal/*.h $(1)/usr/include/absl/random/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/container
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/container/*.h $(1)/usr/include/absl/container
	$(INSTALL_DIR) $(1)/usr/include/absl/container/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/container/internal/*.h $(1)/usr/include/absl/container/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/utility
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/utility/*.h $(1)/usr/include/absl/utility
	$(INSTALL_DIR) $(1)/usr/include/absl/algorithm
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/algorithm/*.h $(1)/usr/include/absl/algorithm
	$(INSTALL_DIR) $(1)/usr/include/absl/memory
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/memory/*.h $(1)/usr/include/absl/memory
	$(INSTALL_DIR) $(1)/usr/include/absl/meta
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/meta/*.h $(1)/usr/include/absl/meta
	$(INSTALL_DIR) $(1)/usr/include/absl/debugging
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/debugging/*.h $(1)/usr/include/absl/debugging
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/debugging/*.inc $(1)/usr/include/absl/debugging
	$(INSTALL_DIR) $(1)/usr/include/absl/debugging/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/debugging/internal/*.h $(1)/usr/include/absl/debugging/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/debugging/internal/*.inc $(1)/usr/include/absl/debugging/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/functional
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/functional/*.h $(1)/usr/include/absl/functional
	$(INSTALL_DIR) $(1)/usr/include/absl/functional/internal
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/functional/internal/*.h $(1)/usr/include/absl/functional/internal
	$(INSTALL_DIR) $(1)/usr/include/absl/numeric
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/numeric/*.h $(1)/usr/include/absl/numeric
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/numeric/*.inc $(1)/usr/include/absl/numeric
	$(INSTALL_DIR) $(1)/usr/include/absl/numeric/internal
	$(INSTALL_DIR) $(1)/usr/src/tensorflow/lite/c
	$(CP) $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/tensorflow/lite/c/common.c $(1)/usr/src/tensorflow/lite/c
	$(CP) $(PKG_BUILD_DIR)/$(SOURCE_SUBDIR)/tensorflow/lite/util.cc $(1)/usr/src/tensorflow/lite/
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/abseil-cpp/absl/numeric/internal/*.h $(1)/usr/include/absl/numeric/internal
	$(INSTALL_DIR) $(1)/etc/tensorflow $(1)/usr/lib $(1)/bin
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/tensorflow-lite/libtensorflow-lite.a $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/_deps/xnnpack-build/libXNNPACK.a $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/cpuinfo/libcpuinfo.a $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/clog/libclog.a $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/pthreadpool/libpthreadpool.a $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/_deps/ruy-build/libruy.a $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/_deps/flatbuffers-build/libflatbuffers.a $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/_deps/fft2d-build/libfft2d_fftsg*.a $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/_deps/farmhash-build/libfarmhash.a $(1)/usr/lib
	$(FIND) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/_deps/abseil-cpp-build/absl/ -name '*.a' -exec $(CP) {} $(1)/usr/lib/ \;
#	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
	$(CP) ./files/* $(1)/
	$(STRIP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/minimal
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/minimal $(1)/bin/
endef

define Package/tensorflow/install
	$(INSTALL_DIR) $(1)/etc/tensorflow $(1)/usr/lib $(1)/bin
#	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
	$(CP) ./files/* $(1)/
	$(STRIP) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/minimal
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(CMAKE_BINARY_SUBDIR)/minimal $(1)/bin/
endef

$(eval $(call BuildPackage,tensorflow,+libstdcpp))
