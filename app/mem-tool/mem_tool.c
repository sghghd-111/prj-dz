#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MEM_FILE				"/dev/mem"
#define MEM_SIZE				1024
#define RD_FUN(addr)			(*((volatile int*)(addr)))
#define WR_FUN(addr, val)		(*((volatile int*)(addr)) = val)


#define CMD_WR					"wr"
#define CMD_RD					"rd"

typedef struct _cmd_info_
{
	int is_wr;
	unsigned long base_addr;
	unsigned long offset;
//	int cyc;
	int wr_val;
}tpCMD_INFO, *pCMD_INFO;

static void help(void)
{
	printf("mem-tool [wr|rd] [(0x)phy-base] [(0x)offset] <wr-data>\n");
}

static int str_parse(int argc, char *argv[], pCMD_INFO cmd)
{
	int ret = 0;

	if (3 > argc)
	{
		printf("param num not enough...\n");
		return -1;
	}
	if (!strcmp(CMD_WR, argv[0]))
		cmd->is_wr = 1;
	else if (!strcmp(CMD_RD, argv[0]))
		cmd->is_wr = 0;
	else
	{
//		printf("");
		help();
		return -2;
	}

	if (cmd->is_wr && (4>argc))
	{
		help();
		return -5;
	}
	ret = sscanf(argv[1], "0x%x", &cmd->base_addr);
	if (!ret)
	{
		help();
		return -3;
	}

	ret = sscanf(argv[2], "0x%x", &cmd->offset);
	if (!ret)
	{
		help();
		return -4;
	}
#if 0
	ret = sscanf(argv[3], "%d", &cmd->cyc);
	if (!ret)
	{
		help();
		return -7;
	}
#endif

#if 1
	if (cmd->is_wr && !sscanf(argv[3], "0x%x", &cmd->wr_val))
	{
		help();
		return -6;
	}
#else
	if (sscanf(argv[3], "0x%x", &cmd->wr_val))
		printf("6666661, %s, %d\n", argv[3], cmd->wr_val);
	if (sscanf(argv[3], "%d", &cmd->wr_val))
		printf("6666662, %s, %d\n", argv[3], cmd->wr_val);
#endif
	printf("666666, %s, %d\n", argv[3], cmd->wr_val);

	return 0;
}

//mm-tool [wr|rd] [base] [offset] <wr-data>
int main(int argc, char *argv[])
{
	int fd = 0;
	void *mem_base;
	tpCMD_INFO cmd_info;
	unsigned int ret;

	if (str_parse(argc-1, &argv[1], &cmd_info))
	{
		printf("cmd error...\n");
		goto OUT;
	}
	fd = open(MEM_FILE, O_RDWR | O_SYNC);
	if (0 >= fd)
	{
		printf("open %s fail...\n", MEM_FILE);
		goto OUT;
	}

	mem_base = mmap(0, MEM_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, cmd_info.base_addr);
	if (cmd_info.is_wr)
	{
	//	printf("addr[%x], value[%x]\n", (cmd_info.base_addr+cmd_info.offset), cmd_info.wr_val);
		WR_FUN(mem_base+cmd_info.offset, cmd_info.wr_val);
		printf("WR:addr[%x], value[%x]\n", (cmd_info.base_addr+cmd_info.offset), cmd_info.wr_val);
	}
//	else
	{
		ret = RD_FUN(mem_base+cmd_info.offset);
		printf("RD:addr[%x], value[%x]\n", (cmd_info.base_addr+cmd_info.offset), ret);
	}
	munmap(mem_base, MEM_SIZE);
	close(fd);
OUT:
	return 0;
}


