TARGET := Emu4VitaArch

SFO_TITLE_ID   := EMUARCH01
SFO_TITLE_NAME := Emu4Vita Arch
SFO_APP_VER    := 01.03

APP_NAME_STR := Emu4Vita\ Arch
APP_VER_STR  := 1.03
APP_DIR_NAME := Emu4VitaArch
BUILD_DATE   := $(shell date +"%Y-%m-%d")

MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR  := $(dir $(MAKEFILE_PATH))
ROOT_DIR      := $(patsubst %/,%, $(MAKEFILE_DIR))

DEPS_DIR          := $(ROOT_DIR)/deps
DEPS_INCLUDE_DIR  := $(DEPS_DIR)/include
DEPS_LIB_DIR      := $(DEPS_DIR)/lib

CORES_DIR     := $(ROOT_DIR)/cores
PKG_DIR       := $(ROOT_DIR)/pkg
SRC_DIR       := $(ROOT_DIR)/src
BUILD_DIR     := $(ROOT_DIR)/../build/arch

VPK_NAME      := $(TARGET)_v$(APP_VER_STR).vpk
VPK_PATH      := $(BUILD_DIR)/$(VPK_NAME)
EBOOT_PATH    := $(PKG_DIR)/eboot.bin
SFO_PATH      := $(PKG_DIR)/sce_sys/param.sfo

INCDIRS := \
	-ISRC_DIR \
	-I$(DEPS_INCLUDE_DIR)

LINKS := \
	-L$(DEPS_LIB_DIR)

SRC_C := \
	browser.c \
	config.c \
	file.c \
	graphics.c \
	init.c \
	main.c \
	ui.c \
	utils.c \
	boot.c

OBJS := $(SRC_C:.c=.o)
OBJS := $(addprefix $(BUILD_DIR)/, $(OBJS))

LIBS := \
	-lvita2d \
	-lfreetype \
	-lpng \
	-ljpeg \
	-lz \
	-lm \
	-lc \
	-lSceDisplay_stub \
	-lSceGxm_stub \
	-lSceCtrl_stub \
	-lSceTouch_stub \
	-lScePower_stub \
	-lSceAudio_stub \
	-lSceRtc_stub \
	-lScePgf_stub \
	-lSceCommonDialog_stub \
	-lSceSysmodule_stub \
	-lSceAppUtil_stub \
	-lSceAppMgr_stub \
	-lSceShellSvc_stub

DEFINES := \
	-DAPP_NAME_STR=\"$(APP_NAME_STR)\" \
	-DAPP_NAME_EXT_STR=\"$(APP_NAME_EXT)\" \
	-DAPP_VER_STR=\"$(APP_VER_STR)\" \
	-DAPP_DIR_NAME=\"$(APP_DIR_NAME)\" \
	-DBUILD_DATE=\"$(BUILD_DATE)\"

WARNINGS := \
	-Wall \
	-Wno-unused-variable \
	-Wno-unused-but-set-variable \
	-Wno-format-truncation

FLAGS    := $(WARNINGS) $(INCDIRS) $(DEFINES)
CFLAGS   := -O3 $(FLAGS)
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS  := -Wl,-q $(LINKS)

PREFIX  := arm-vita-eabi
CC      := $(PREFIX)-gcc
CXX     := $(PREFIX)-g++
AS      := $(PREFIX)-as
AR      := $(PREFIX)-ar
OBJCOPY := $(PREFIX)-objcopy
STRIP   := $(PREFIX)-strip
NM      := $(PREFIX)-nm
LD      := $(CXX)

all: clean_files create_dirs build-cores build-vita

clean_files:
	@rm -rf $(VPK_PATH) $(EBOOT_PATH) $(SFO_PATH) $(BUILD_DIR)/$(TARGET).velf $(BUILD_DIR)/$(TARGET).elf

create_dirs:
	@mkdir -p $(BUILD_DIR)

build-cores:
	make -f Makefile.cores

build-vita: $(EBOOT_PATH) $(SFO_PATH)
	@rm -rf $(VPK_PATH)
	cd $(PKG_DIR) && zip -r $(VPK_PATH) ./*
	cd $(CORES_DIR) && zip -r $(VPK_PATH) ./*

$(SFO_PATH):
	@mkdir -p $(dir $@)
	vita-mksfoex -s APP_VER="$(SFO_APP_VER)" \
		-s TITLE_ID="$(SFO_TITLE_ID)" "$(SFO_TITLE_NAME)" \
		-d ATTRIBUTE2=12 $@

$(EBOOT_PATH): $(BUILD_DIR)/$(TARGET).velf
	vita-make-fself -c -a 0x2800000000000001 $< $@

$(BUILD_DIR)/$(TARGET).velf: $(BUILD_DIR)/$(TARGET).elf
	$(STRIP) -g $<
	vita-elf-create $< $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean-cores:
	make -f Makefile.cores clean

clean-vita:
	rm -rf $(BUILD_DIR) $(EBOOT_PATH) $(SFO_PATH)

clean: clean-vita

clean-all: clean-cores clean-vita
