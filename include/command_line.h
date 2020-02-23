#ifndef _COMMAND_LINE_H_
#define _COMMAND_LINE_H_

typedef int (*CMD_FUNC)(void *, int, char **);

typedef struct _cmd_node_
{
	char *name;
	char *tip;
	CMD_FUNC func;
}tpCMD_NODE, *pCMD_NODE;

typedef struct _cmd_para_
{
	pCMD_NODE nodes;
	int size;
	void *usr;
}tpCMD_PARA, *pCMD_PARA;

extern int cmdline_init(pCMD_PARA para, pCMD_NODE nodes, int size, void *usr);
extern int cmdline_parse(pCMD_PARA para, int argc, char **argv);
#endif

