APPS_DIR    := ./apps
SOURCE_DIR  := ./source
ARCH_DIR    := ./arch

MAKE_ARGS   := 
ifeq ($(nightly), 1)
	MAKE_ARGS := nightly=1
endif

all: build-app build-arch

clean: clean-app clean-arch

build-deps:
	cd $(SOURCE_DIR) && make -f Makefile.deps

clean-deps:
	cd $(SOURCE_DIR) && make -f Makefile.deps clean

build-app:
	cd $(APPS_DIR)/fba_lite && make $(MAKE_ARGS)
	cd $(APPS_DIR)/fceumm && make $(MAKE_ARGS)
	cd $(APPS_DIR)/gambatte && make $(MAKE_ARGS)
	cd $(APPS_DIR)/genesis_plus_gx && make $(MAKE_ARGS)
	cd $(APPS_DIR)/gpsp && make $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_ngp && make $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_pce_fast && make $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_supergrafx && make $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_wswan && make $(MAKE_ARGS)
	cd $(APPS_DIR)/nestopia && make $(MAKE_ARGS)
	cd $(APPS_DIR)/pcsx_rearmed && make $(MAKE_ARGS)
	cd $(APPS_DIR)/picodrive && make $(MAKE_ARGS)
	cd $(APPS_DIR)/snes9x2002 && make $(MAKE_ARGS)
	cd $(APPS_DIR)/snes9x2005 && make $(MAKE_ARGS)
	cd $(APPS_DIR)/snes9x2005_plus && make $(MAKE_ARGS)
	cd $(APPS_DIR)/vba_next && make $(MAKE_ARGS)
	cd $(APPS_DIR)/chimerasnes && make $(MAKE_ARGS)

clean-app:
	cd $(APPS_DIR)/fba_lite && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/fceumm && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/gambatte && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/genesis_plus_gx && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/gpsp && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_ngp && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_pce_fast && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_supergrafx && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_wswan && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/nestopia && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/pcsx_rearmed && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/picodrive && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/snes9x2002 && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/snes9x2005 && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/snes9x2005_plus && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/vba_next && make clean $(MAKE_ARGS)
	cd $(APPS_DIR)/chimerasnes && make clean $(MAKE_ARGS)

clean-all:
	cd $(APPS_DIR)/fba_lite && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/fceumm && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/gambatte && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/genesis_plus_gx && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/gpsp && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_ngp && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_pce_fast && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_supergrafx && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/mednafen_wswan && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/nestopia && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/pcsx_rearmed && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/picodrive && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/snes9x2002 && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/snes9x2005 && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/snes9x2005_plus && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/vba_next && make clean-all $(MAKE_ARGS)
	cd $(APPS_DIR)/chimerasnes && make clean-all $(MAKE_ARGS)

build-arch:
	cd $(ARCH_DIR) && make $(MAKE_ARGS)

clean-arch:
	cd $(ARCH_DIR) && make clean $(MAKE_ARGS)
