MAKE_ARGS = main_build=1

BUBBLE_DIR  = ./bubble
VITA_DIR    = ./vita
ARCH_DIR    = ./arch

all: build-vita build-arch

clean: clean-vita clean-arch

build-deps:
	cd $(VITA_DIR) && make -f Makefile.deps

clean-deps:
	cd $(VITA_DIR) && make -f Makefile.deps clean

build-vita:
	cd $(BUBBLE_DIR)/fba_lite && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/fceumm && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/gambatte && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/genesis_plus_gx && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/gpsp && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_ngp && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_pce_fast && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_supergrafx && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_wswan && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/nestopia && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/pcsx_rearmed && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/picodrive && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2002 && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2005 && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2005_plus && make $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/vba_next && make $(MAKE_ARGS)

clean-vita:
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

clean-all:
	cd $(BUBBLE_DIR)/fba_lite && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/fceumm && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/gambatte && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/genesis_plus_gx && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/gpsp && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_ngp && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_pce_fast && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_supergrafx && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/mednafen_wswan && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/nestopia && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/pcsx_rearmed && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/picodrive && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2002 && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2005 && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/snes9x2005_plus && make clean-all $(MAKE_ARGS)
	cd $(BUBBLE_DIR)/vba_next && make clean-all $(MAKE_ARGS)

build-arch:
	cd $(ARCH_DIR) && make $(MAKE_ARGS)

clean-arch:
	cd $(ARCH_DIR) && make clean $(MAKE_ARGS)