#ifndef _PLAT_CONFIG_H_
#define _PLAT_CONFIG_H_

typedef struct _sys_config_
{
	int station_type;					//station type
	int daem;							//is daem
	int hardware;						//hardware version
}tpSYS_CONFIG, *pSYS_CONFIG;

extern int plat_conf_parse(int argc, char *argv[]);
extern int plat_station_type(void);
extern int plat_hardware(void);
extern int plat_daem(void);
extern char *plat_get_process_name(char *argv0);

#endif
