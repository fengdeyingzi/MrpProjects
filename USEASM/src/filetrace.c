#include "filetrace.h"
#include "mrc_base.h"


int32 trace_file_fd;


void filetraceInit(const char * filename){
	if(trace_file_fd)
		mrc_close(trace_file_fd);
	trace_file_fd = mrc_open(filename, MR_FILE_RDWR|MR_FILE_CREATE);
	mrc_seek(trace_file_fd, 0, MR_SEEK_END);
}

void filetraceClose(void){
	mrc_close(trace_file_fd);
	trace_file_fd = 0;
}

void filetrace(void *data, int len, E_TRACE_TYPE type){
	mr_datetime t;
	char buf[64];
	int l;
	char * str = "";

	mrc_getDatetime(&t);
	switch(type)
	{
	case TRACETYPE_ERROR: str = "error"; break;
	case TRACETYPE_WARING: str = "waring"; break;
	case TRACETYPE_INFO: str = "info"; break;
	}
	l = mrc_sprintf(buf, "[%s] [%d/%d/%d %d:%d]\r\n", str, t.year, t.month, t.day, t.hour, t.day, t.minute);
	mrc_write(trace_file_fd, buf, l);
	mrc_write(trace_file_fd, data, len);
	mrc_write(trace_file_fd, "\r\n", 2);

#ifdef WIN32
	mrc_printf("%s", buf);
#endif
}

