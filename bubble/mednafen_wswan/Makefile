TARGET = Mednafen-Wswan

SFO_TITLE_NAME   := Mednafen Wswan
SFO_TITLE_ID     := WSWAN0001

APP_NAME         := Mednafen\ WonderSwan
APP_DIR_NAME     := MednafenWswan
CORE_FILE_NAME   := mednafen_wswan

CORE_SOFTWARE    := Bandai\ -\ WonderSwan\ /\ Color

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
	-DWSC_BUILD

include $(VITA_DIR)/Makefile.common
