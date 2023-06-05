TARGET = Gambatte

SFO_TITLE_NAME   :=Gambatte
SFO_TITLE_ID     := GAMEBATTE

APP_NAME         := Gambatte
APP_DIR_NAME     := Gambatte
CORE_FILE_NAME   := gambatte

CORE_SOFTWARE    := Nintendo\ -\ Game\ Boy\ /\ Color

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))
ROOT_DIR           := $(patsubst %/,%, $(MAKEFILE_DIR))

VITA_DIR           := $(ROOT_DIR)/../../vita
CORE_DIR           := $(ROOT_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)
CORE_MAKEFILE_NAME := Makefile.libretro

WANT_DISPLAY_ROTATE :=

CORE_ARGS := 

CORE_DEFINES := \
	-DGBC_BUILD

include $(VITA_DIR)/Makefile.common
