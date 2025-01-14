# inherit from the proprietary version
-include vendor/alps/hct6580/BoardConfigVendor.mk

TARGET_SPECIFIC_HEADER_PATH := device/alps/hct6580/include

# Platform
TARGET_BOARD_PLATFORM := mt6580
TARGET_NO_BOOTLOADER := true

# Architecture
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_CPU_VARIANT := cortex-a7

# Bootloader
TARGET_BOOTLOADER_BOARD_NAME := mt6580

TARGET_USERIMAGES_USE_EXT4 := true
TARGET_USERIMAGES_SPARSE_EXT_DISABLED := false

# Kernel
TARGET_PREBUILT_KERNEL := device/alps/hct6580/prebuilt/kernel
BOARD_MKBOOTIMG_ARGS := --kernel_offset 0x00008000 --ramdisk_offset 0x04000000 --tags_offset 0x0e000000

BOARD_KERNEL_CMDLINE += \
	bootopt=64S3,32S1,32S1 \
	androidboot.selinux=permissive
BOARD_KERNEL_BASE := 0x80000000
BOARD_KERNEL_PAGESIZE := 2048

BOARD_CUSTOM_BOOTIMG := true
TARGET_KMODULES := true

# Partitions
BOARD_BOOTIMAGE_PARTITION_SIZE := 16777216
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 16777216
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1853882368
BOARD_USERDATAIMAGE_PARTITION_SIZE := 1559756800
BOARD_CACHEIMAGE_PARTITION_SIZE := 268435456
BOARD_FLASH_BLOCK_SIZE := 131072

# Assert
TARGET_OTA_ASSERT_DEVICE := "hct6580,w363,b6580_weg_a_m"

TARGET_RUNNING_WITHOUT_SYNC_FRAMEWORK := true

# Disable memcpy opt (for audio libraries)
TARGET_CPU_MEMCPY_OPT_DISABLE := true

# EGL
BOARD_EGL_CFG := device/alps/hct6580/configs/egl.cfg
USE_OPENGL_RENDERER := true
BOARD_EGL_WORKAROUND_BUG_10194508 := true
TARGET_REQUIRES_SYNCHRONOUS_SETSURFACE := true

# MTK Hardware
BOARD_HAS_MTK_HARDWARE := true
BOARD_USES_MTK_HARDWARE := true
MTK_HARDWARE := true

# GPS
BOARD_GPS_LIBRARIES := true
BOARD_MEDIATEK_USES_GPS := true

# RIL 
BOARD_RIL_CLASS := ../../../device/alps/hct6580/ril/

BOARD_CONNECTIVITY_VENDOR := MediaTek
BOARD_CONNECTIVITY_MODULE := conn_soc

# WIFI
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_mt66xx
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_mt66xx
WIFI_DRIVER_FW_PATH_PARAM :="/dev/wmtWifi"
WIFI_DRIVER_FW_PATH_STA :=STA
WIFI_DRIVER_FW_PATH_AP :=AP
WIFI_DRIVER_FW_PATH_P2P :=P2P

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_MTK := true
BOARD_BLUETOOTH_DOES_NOT_USE_RFKILL := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/alps/hct6580/bluetooth

# Offline charging
BOARD_CHARGING_MODE_BOOTING_LPM := /sys/class/BOOT/BOOT/boot/boot_mode
BACKLIGHT_PATH := /sys/class/leds/lcd-backlight/brightness
BOARD_CHARGER_ENABLE_SUSPEND := true
BOARD_CHARGER_SHOW_PERCENTAGE := true

# Sensors
TARGET_NO_SENSOR_PERMISSION_CHECK := true

# CWM
TARGET_RECOVERY_FSTAB := device/alps/hct6580/rootdir/root/recovery.fstab
TARGET_PREBUILT_RECOVERY_KERNEL := device/alps/hct6580/prebuilt/kernel
BOARD_HAS_NO_SELECT_BUTTON := true

# TWRP stuff
TW_THEME := portrait_hdpi
RECOVERY_GRAPHICS_USE_LINELENGTH := true
TW_NO_REBOOT_BOOTLOADER := true
TW_BRIGHTNESS_PATH := /sys/devices/platform/leds-mt65xx/leds/lcd-backlight/brightness
TARGET_USE_CUSTOM_LUN_FILE_PATH := /sys/devices/platform/mt_usb/musb-hdrc.0.auto/gadget/lun%d/file
TW_MAX_BRIGHTNESS := 255
TW_EXCLUDE_SUPERSU := true
TW_INCLUDE_FB2PNG := true
TW_NO_CPU_TEMP := true
TW_REBOOT_BOOTLOADER := true
TW_REBOOT_RECOVERY := true
TW_HAS_DOWNLOAD_MODE := true
TW_EXCLUDE_SUPERSU := true
TW_USE_TOOLBOX := true

TARGET_USE_CUSTOM_LUN_FILE_PATH := "/sys/devices/virtual/android_usb/android0/f_mass_storage/lun%d/file"

BOARD_SEPOLICY_DIRS := \
       device/alps/hct6580/sepolicy

# Use old sepolicy version
POLICYVERS := 29

# Seccomp filter
BOARD_SECCOMP_POLICY += device/alps/hct6580/seccomp

# build old-style zip files (required for ota updater)
BLOCK_BASED_OTA := true

TARGET_LDPRELOAD += libxlog.so:libmtk_symbols.so

##################################
#    **Odex Configuration**
#      true = Odexed Rom
#      false = Deodexed Rom
#      null = Rom source decides
##################################
ODEX := false

ifeq ($(ODEX),true)
    WITH_DEXPREOPT := true
    DISABLE_DEXPREOPT := false
endif

ifeq ($(ODEX),false)
    WITH_DEXPREOPT := false
    DISABLE_DEXPREOPT := true
endif

