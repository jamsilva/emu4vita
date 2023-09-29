TARGET = Snes9x2005Plus

APP_NAME         := Snes9x\ 2005\ Plus
APP_NAME_EXT     := 
APP_DIR_NAME     := Snes9x2005Plus
CORE_FILE_NAME   := snes9x2005_plus

SFO_TITLE_NAME   := Snes9x 2005 Plus
SFO_TITLE_ID     := SNEX9X05P

CORE_SOFTWARE    := Nintendo\ -\ SNES\ /\ SFC

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))

APP_DIR            := $(patsubst %/,%, $(MAKEFILE_DIR))
SOURCE_DIR         := $(APP_DIR)/../../source
CORE_DIR           := $(APP_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)
CORE_MAKEFILE_NAME := Makefile

WANT_DISPLAY_ROTATE :=

CORE_ARGS := \
	USE_BLARGG_APU=1

CORE_DEFINES := \
	-DSNES9X2005_PLUS_BUILD \
	-DSFC_BUILD

include $(SOURCE_DIR)/Makefile.common
