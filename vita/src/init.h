#ifndef __M_INIT_H__
#define __M_INIT_H__

extern int is_safe_mode;
extern int is_vitatv_model;

extern int language, enter_button, date_format, time_format;

int AppInit(int argc, char *const argv[]);
int AppDeinit();
int AppExit();

#endif