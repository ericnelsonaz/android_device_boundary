# -------@block_kernel_bootimg-------

KERNEL_NAME := Image
TARGET_KERNEL_ARCH := arm64

#Enable this to disable product partition build.
IMX_NO_PRODUCT_PARTITION := true

# QCA9377 wifi driver module
BOARD_HAS_QCACLD_MODULE := true
BOARD_VENDOR_KERNEL_MODULES += \
    $(TARGET_OUT_INTERMEDIATES)/QCACLD_OBJ/wlan.ko

# Dummy battery module
BOARD_VENDOR_KERNEL_MODULES += \
    $(KERNEL_OUT)/drivers/power/supply/dummy_battery.ko

# -------@block_security-------
#Enable this to include trusty support
PRODUCT_IMX_TRUSTY := false
