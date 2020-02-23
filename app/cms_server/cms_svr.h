#ifndef _CMS_SVR_H_
#define _CMS_SVR_H_
#include "pulse.h"
#include "bc_func.h"

typedef struct _server_info_
{
	void *pulse_hdl;
	void *bc_hdl;
}tpSERVER_INFO, *pSERVER_INFO;

#endif
