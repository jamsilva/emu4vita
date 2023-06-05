TARGET = gpSP

SFO_TITLE_NAME   := gpSP
SFO_TITLE_ID     := GPSP00001

APP_NAME         := gpSP
APP_DIR_NAME     := gpSP
CORE_FILE_NAME   := gpsp

CORE_SOFTWARE    := Nintendo\ -\ Game\ Boy\ Advance

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))
ROOT_DIR           := $(patsubst %/,%, $(MAKEFILE_DIR))

VITA_DIR           := $(ROOT_DIR)/../../vita
CORE_DIR           := $(ROOT_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)
CORE_MAKEFILE_NAME := Makefile

WANT_DISPLAY_ROTATE :=

CORE_ARGS := 

CORE_DEFINES := \
	-DUSE_INTE_SYSTEM_DIR \
	-DGBA_BUILD

include $(VITA_DIR)/Makefile.common
