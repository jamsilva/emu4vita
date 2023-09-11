TARGET = PicoDrive

APP_NAME         := PicoDrive
APP_NAME_EXT     := 
APP_DIR_NAME     := PicoDrive
CORE_FILE_NAME   := picodrive

SFO_TITLE_NAME   := PicoDrive
SFO_TITLE_ID     := PRCODRIVE

CORE_SOFTWARE    := Sega\ -\ MS/MD/CD/32x

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))

APP_DIR            := $(patsubst %/,%, $(MAKEFILE_DIR))
SOURCE_DIR         := $(APP_DIR)/../../source
CORE_DIR           := $(APP_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)
CORE_MAKEFILE_NAME := Makefile.libretro

WANT_DISPLAY_ROTATE :=

CORE_ARGS := \
	PLATFORM_ZLIB=0

CORE_DEFINES := \
	-DSMS_BUILD \
	-DMD_BUILD \
	-DMDCD_BUILD \
	-DMD32X_BUILD \
	-DUSE_INTE_SYSTEM_DIR \
	-DHAVE_ZIP_ROM

include $(SOURCE_DIR)/Makefile.common
