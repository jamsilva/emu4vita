TARGET := Emu4VitaArch

SFO_TITLE_ID      := EMUARCH01
SFO_TITLE_NAME    := Emu4Vita Arch
SFO_APP_VER       := 01.04

APP_NAME_STR      := Emu4Vita\ Arch
APP_VER_STR       := 1.04
APP_DIR_NAME      := Emu4VitaArch
BUILD_DATE        := $(shell date +"%Y-%m-%d")

MAKEFILE_PATH     := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR      := $(dir $(MAKEFILE_PATH))
ROOT_DIR          := $(patsubst %/,%, $(MAKEFILE_DIR))

DEPS_DIR          := $(ROOT_DIR)/deps
DEPS_INCLUDE_DIR  := $(DEPS_DIR)/include
DEPS_LIB_DIR      := $(DEPS_DIR)/lib

CORES_DIR         := $(ROOT_DIR)/cores
PKG_DIR           := $(ROOT_DIR)/pkg
SRC_DIR           := $(ROOT_DIR)/src
BUILD_DIR         := $(ROOT_DIR)/../build/arch
BUBBLE_DIR        := $(ROOT_DIR)/../apps

RELEASES_DIR      := $(ROOT_DIR)/../releases

VPK_NAME          := $(TARGET)_v$(APP_VER_STR).vpk
VPK_PATH          := $(BUILD_DIR)/$(VPK_NAME)
EBOOT_PATH        := $(PKG_DIR)/eboot.bin
SFO_PATH          := $(PKG_DIR)/sce_sys/param.sfo

ifeq ($(main_build), 1)
	RELEASE_DIR   := $(RELEASES_DIR)/Emu4Vita_Arch
	VPK_PATH      := $(RELEASE_DIR)/$(VPK_NAME)
endif

MAKE_ARGS         := arch_build=1

INCDIRS := \
	-ISRC_DIR \
	-I$(DEPS_INCLUDE_DIR)

LIBDIRS := \
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

CFLAGS   := -O3 $(WARNINGS) $(INCDIRS) $(DEFINES)
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS  := -Wl,-q $(LIBDIRS)

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

build-vita: $(EBOOT_PATH) $(SFO_PATH)
	@rm -rf $(VPK_PATH)
	@mkdir -p $(dir $(VPK_PATH))
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

clean: clean-vita

clean-all: clean-cores clean-vita

clean-vita:
	rm -rf $(BUILD_DIR) $(VPK_PATH) $(EBOOT_PATH) $(SFO_PATH)

build-cores:
	cd $(BUBBLE_DIR)/fba_lite && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/fceumm && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/gambatte && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/genesis_plus_gx && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/gpsp && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_ngp && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_pce_fast && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_supergrafx && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_wswan && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/nestopia && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/pcsx_rearmed && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/picodrive && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2002 && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2005 && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2005_plus && make build-self $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/vba_next && make build-self $(MAKE_ARGS)
	
clean-cores:
	cd $(BUBBLE_DIR)/fba_lite && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/fceumm && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/gambatte && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/genesis_plus_gx && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/gpsp && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_ngp && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_pce_fast && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_supergrafx && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_wswan && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/nestopia && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/pcsx_rearmed && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/picodrive && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2002 && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2005 && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2005_plus && make clean $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/vba_next && make clean $(MAKE_ARGS)
