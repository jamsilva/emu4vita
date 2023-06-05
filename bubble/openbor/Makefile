TARGET = OpenBOR

SFO_TITLE_NAME   :=OpenBOR
SFO_TITLE_ID     := OPENBOR01

APP_NAME         := OpenBOR
APP_DIR_NAME     := OpenBOR
CORE_FILE_NAME   := openbor

CORE_SOFTWARE    := OpenBOR

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))
ROOT_DIR           := $(patsubst %/,%, $(MAKEFILE_DIR))

VITA_DIR           := $(ROOT_DIR)/../../vita
CORE_DIR           := $(ROOT_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)/engine/libretro
CORE_MAKEFILE_NAME := Makefile.libretro

WANT_DISPLAY_ROTATE :=

CORE_ARGS := 

CORE_DEFINES := \
	-DOPENBOR_BUILD

include $(VITA_DIR)/Makefile.common
