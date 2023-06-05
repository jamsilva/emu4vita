TARGET = Nestopia

SFO_TITLE_NAME   := Nestopia
SFO_TITLE_ID     := NESTOPIA1

APP_NAME         := Nestopia
APP_NAME_EXT     := 
APP_DIR_NAME     := Nestopia
CORE_FILE_NAME   := nestopia

CORE_SOFTWARE    := Nintendo\ -\ NES\ /\ Famicom

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))
ROOT_DIR           := $(patsubst %/,%, $(MAKEFILE_DIR))

VITA_DIR           := $(ROOT_DIR)/../../vita
CORE_DIR           := $(ROOT_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)/libretro
CORE_MAKEFILE_NAME := Makefile

WANT_DISPLAY_ROTATE :=

CORE_ARGS := 

CORE_DEFINES := \
	-DFC_BUILD

include $(VITA_DIR)/Makefile.common
