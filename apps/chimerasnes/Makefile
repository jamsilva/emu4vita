TARGET = ChimeraSNES

APP_NAME         := ChimeraSNES
APP_NAME_EXT     := 
APP_DIR_NAME     := chimerasnes
CORE_FILE_NAME   := chimerasnes

SFO_TITLE_NAME   := ChimeraSNES
SFO_TITLE_ID     := CHIMRSNES

CORE_SOFTWARE    := Nintendo\ -\ SNES\ /\ SFC

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))

APP_DIR            := $(patsubst %/,%, $(MAKEFILE_DIR))
SOURCE_DIR         := $(APP_DIR)/../../source
CORE_DIR           := $(APP_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)
CORE_MAKEFILE_NAME := Makefile

WANT_DISPLAY_ROTATE :=

CORE_ARGS := 

CORE_DEFINES := \
	-DCHIMERASNES_BUILD \
	-DSFC_BUILD

include $(SOURCE_DIR)/Makefile.common
