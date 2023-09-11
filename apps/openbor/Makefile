TARGET = OpenBOR

APP_NAME         := OpenBOR
APP_NAME_EXT     := 
APP_DIR_NAME     := OpenBOR
CORE_FILE_NAME   := openbor

SFO_TITLE_NAME   := OpenBOR
SFO_TITLE_ID     := OPENBOR01

CORE_SOFTWARE    := OpenBOR

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))

APP_DIR            := $(patsubst %/,%, $(MAKEFILE_DIR))
SOURCE_DIR         := $(APP_DIR)/../../source
CORE_DIR           := $(APP_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)/engine/libretro
CORE_MAKEFILE_NAME := Makefile.libretro

WANT_DISPLAY_ROTATE :=

CORE_ARGS := 

CORE_DEFINES := \
	-DOPENBOR_BUILD

include $(SOURCE_DIR)/Makefile.common
