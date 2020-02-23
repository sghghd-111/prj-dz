#include "plat_config.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static tpSYS_CONFIG conf;

typedef enum
{
	ARG_MIN = 0x111,
	ARG_STATION,
	ARG_DAEM,
	ARG_HARDWARE,
	ARG_MAX,
}ARG_TYPE;


static const struct option opts[] = 
{
	{"station", 1, 0, ARG_STATION},
	{"daem", 1, 0, ARG_DAEM},
	{"hw", 1, 0, ARG_HARDWARE},
};


int plat_conf_parse(int argc, char *argv[])
{
	int c;
	
	memset(&conf, 0, sizeof(tpSYS_CONFIG));

	conf.daem = 1;
	conf.hardware = 1;
	conf.station_type = 1;

	while (-1 != (c=getopt_long(argc, argv, "", opts, NULL)))
	{
		switch (c)
		{
		case ARG_STATION:
			conf.station_type = atoi(optarg);
			break;
		case ARG_DAEM:
			conf.daem = atoi(optarg);
			break;
		case ARG_HARDWARE:
			conf.hardware = atoi(optarg);
			break;
		default:
			break;
		}
	}
	return 0;
}

int plat_station_type(void)
{
	return conf.station_type;
}

int plat_hardware(void)
{
	return conf.hardware;
}

int plat_daem(void)
{
	return conf.daem;
}

char *plat_get_process_name(char *argv0)
{
	char *pt;

	pt = strrchr(argv0, '/');
	if (!pt)
		return argv0;
	else
		return &pt[1];
}


