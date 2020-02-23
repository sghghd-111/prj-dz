#include "command_line.h"

int cmdline_init(pCMD_PARA para, pCMD_NODE nodes, int size, void *usr)
{
	if (!para)
		return -1;
	if (0 > size)
		return -2;
	para->nodes = nodes;
	para->size = size;
	para->usr = usr;

	return 0;
}

int cmdline_parse(pCMD_PARA para, int argc, char **argv)
{
	int j;
	int ret = -1;

	if (!para)
		return -1;
	if (!argv || !(*argv))
		return -2;
	
	for (j=0; j<para->size; j++)
	{
		if (strcmp(para->nodes[j].name, argv[0]))
			continue;

		if (para->nodes[j].func)
			ret = para->nodes[j].func(para->usr, argc-1, &argv[1]);
		break;
	}
	return ret;
}

