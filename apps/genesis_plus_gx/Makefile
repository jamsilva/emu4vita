TARGET = Genesis-Plus-GX

APP_NAME         := Genesis\ Plus\ GX
APP_NAME_EXT     := 
APP_DIR_NAME     := GenesisPlusGX
CORE_FILE_NAME   := genesis_plus_gx

SFO_TITLE_NAME   := Genesis Plus GX
SFO_TITLE_ID     := GENESIS01

CORE_SOFTWARE    := Sega\ -\ MS/GG/MD/CD

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))

APP_DIR            := $(patsubst %/,%, $(MAKEFILE_DIR))
SOURCE_DIR         := $(APP_DIR)/../../source
CORE_DIR           := $(APP_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)
CORE_MAKEFILE_NAME := Makefile.libretro

WANT_DISPLAY_ROTATE := 

CORE_ARGS := 

CORE_DEFINES := \
	-DGENESIS_PLUS_GX_BUILD \
	-DMD_BUILD

include $(SOURCE_DIR)/Makefile.common
