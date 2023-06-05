TARGET = mame2003-plus

SFO_TITLE_NAME   := mame2003 plus
SFO_TITLE_ID     := MAME2003P

APP_NAME         := mame2003\ plus
APP_NAME_EXT     := 
APP_DIR_NAME     := mame2003-plus
CORE_FILE_NAME   := mame2003_plus

CORE_SOFTWARE    := Arcade

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))
ROOT_DIR           := $(patsubst %/,%, $(MAKEFILE_DIR))

VITA_DIR           := $(ROOT_DIR)/../../vita
CORE_DIR           := $(ROOT_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)
CORE_MAKEFILE_NAME := Makefile

WANT_DISPLAY_ROTATE := 1

CORE_ARGS := 

CORE_DEFINES := \
	-DFBA_BUILD

include $(VITA_DIR)/Makefile.common