TARGET = Mednafen-Ngp

SFO_TITLE_NAME   := Mednafen NeoPop
SFO_TITLE_ID     := NGP000001

APP_NAME         := Mednafen\ NeoPop
APP_NAME_EXT     := 
APP_DIR_NAME     := MednafenNgp
CORE_FILE_NAME   := mednafen_ngp

CORE_SOFTWARE    := SNK\ -\ Neo\ Geo\ Pocket\ /\ Color

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
	-DNGP_BUILD

include $(VITA_DIR)/Makefile.common
