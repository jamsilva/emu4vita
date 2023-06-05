TARGET = Mednafen-PCE-Fast

SFO_TITLE_NAME   := Mednafen PCE Fast
SFO_TITLE_ID     := MPCEFAST1

APP_NAME         := Mednafen\ PCE\ Fast
APP_DIR_NAME     := MednafenPCEFast
CORE_FILE_NAME   := mednafen_pce_fast

CORE_SOFTWARE    := NEC\ -\ PC\ Engine\ /\ CD

MAKEFILE_PATH      := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR       := $(dir $(MAKEFILE_PATH))
ROOT_DIR           := $(patsubst %/,%, $(MAKEFILE_DIR))

VITA_DIR           := $(ROOT_DIR)/../../vita
CORE_DIR           := $(ROOT_DIR)/../../cores/$(CORE_FILE_NAME)
CORE_MAKEFILE_DIR  := $(CORE_DIR)
CORE_MAKEFILE_NAME := Makefile

WANT_DISPLAY_ROTATE :=

CORE_ARGS := \
	SYSTEM_ZLIB=1

CORE_DEFINES := \
	-DPCE_BUILD

include $(VITA_DIR)/Makefile.common
