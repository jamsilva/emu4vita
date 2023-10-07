char *about_texts[] = {
    APP_NAME_STR " " APP_NAME_EXT_STR,
    " * Frontend author: yizhigai",
#if defined(CHIMERASNES_BUILD)
    " * Core author: jamsilva",
#else
	" * Core authors: libretro team",
#endif
    " * Version: " APP_VER_STR "    Date: " BUILD_DATE,
    "",
    "Platform:",
    " * " CORE_SOFTWARE,
    "",
    "BIOS / Firmware:",
#if defined(FBA_LITE_BUILD)
    " * neogeo.zip (Neo Geo BIOS)",
    " * pgm.zip (PGM System BIOS)",
#elif defined(FCEUMM_BUILD)
    " * disksys.rom (Family Computer Disk System BIOS)",
    " * nes.pal (custom NES Palette)",
    " * gamegenie.nes (Game Genie add-on cartridge)",
#elif defined(NESTOPIA_BUILD)
    " * disksys.rom (Family Computer Disk System BIOS)",
#elif defined(GENESIS_PLUS_GX_BUILD)
    " * bios_MD.bin (Mega Drive startup ROM)",
    " * bios_CD_E.bin (MegaCD EU BIOS)",
    " * bios_CD_U.bin (SegaCD US BIOS)",
    " * bios_CD_J.bin (MegaCD JP BIOS)",
    " * bios_E.sms (MasterSystem EU BIOS)",
    " * bios_U.sms (MasterSystem US BIOS)",
    " * bios_J.sms (MasterSystem JP BIOS)",
    " * bios.gg (GameGear BIOS)",
    " * sk.bin (Sonic & Knuckles ROM)",
    " * sk2chip.bin (Sonic & Knuckles UPMEM ROM)",
    " * areplay.bin (Action Replay ROM)",
    " * ggenie.bin (Game Genie ROM)",
#elif defined(PICODRIVE_BUILD)
    " * bios_CD_E.bin (MegaCD EU BIOS)",
    " * bios_CD_U.bin (SegaCD US BIOS)",
    " * bios_CD_J.bin (MegaCD JP BIOS)",
#elif defined(GAMEBATTE_BUILD)
    " * gb_bios.bin (Game Boy BIOS)",
    " * gbc_bios.bin (Game Boy Color BIOS)",
#elif defined(GPSP_BUILD) || defined(VBA_NEXT_BUILD)
    " * gba_bios.bin (Game Boy Advance BIOS)",
#elif defined(MEDNAFEN_PCE_FAST_BUILD) || defined(MEDNAFEN_SUPERGRAFX_BUILD)
    " * syscard3.pce (PCE-CD BIOS)",
    " * syscard2.pce (PCE-CD BIOS)",
    " * syscard1.pce (PCE-CD BIOS)",
    " * gexpress.pce (PCE-CD BIOS)",
#elif defined(PCSX_REARMED_BUILD)
    " * scph5500.bin (PS1 JP BIOS)",
    " * scph5501.bin (PS1 US BIOS)",
    " * scph5502.bin (PS1 EU BIOS)",
#elif defined(SFC_BUILD)
    " * BS-X.bin (BSX/Satellaview BIOS)",
	" * STBIOS.bin (SuFami Turbo BIOS)",
#else
    " * None required",
#endif
    "",
    "Thanks to:",
    " * Team Molecule (HENkaku)",
    " * xerpi (libvita2d)",
    " * TheFlow (VitaShell)",
    " * frangarcj (vita shader)",
	" * Arisa Mizuhara (icons and overlays)",
    " * ...",
};

#define N_ABOUT_TEXTS (sizeof(about_texts) / sizeof(char *))
