TARGET = Snes9x2005Plus

SFO_TITLE_NAME   := Snes9x 2005 Plus
SFO_TITLE_ID     := SNEX9X05P

APP_NAME         := Snes9x\ 2005\ Plus
APP_NAME_EXT     := 
APP_DIR_NAME     := Snes9x2005Plus
CORE_FILE_NAME   := snes9x2005_plus

CORE_SOFTWARE    := Nintendo\ -\ SNES\ /\ SFC

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))
ROOT_DIR           := $(patsubst %/,%, $(MAKEFILE_DIR))

VITA_DIR           := $(ROOT_DIR)/../../vita
CORE_DIR           := $(ROOT_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)
CORE_MAKEFILE_NAME := Makefile

WANT_DISPLAY_ROTATE :=

CORE_ARGS := \
	USE_BLARGG_APU=1

CORE_DEFINES := \
	-DSFC_BUILD

include $(VITA_DIR)/Makefile.common
