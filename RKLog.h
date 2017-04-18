#ifndef RKLOG_HEADER
#define RKLOG_HEADER
#include "DefineHeader.h"

#define GET_FMT_STRING(fmt, buf) \
	{ \
	va_list args; \
	va_start(args, fmt); \
	vsnprintf(buf, sizeof(buf)-1, fmt, args); \
	va_end(args); \
	buf[sizeof(buf)-1] = 0x00; \
};

class CRKLog
{
public:
	string GetLogSavePath();
 	bool GetEnableLog();
 	void SetEnableLog(bool bEnable);
 	property<CRKLog, string, READ_ONLY> LogSavePath;
 	property<CRKLog, bool, READ_WRITE> EnableLog;
	CRKLog(string logFilePath, string logFileName, bool enable = false);
	~CRKLog();
	bool SaveBuffer(string fileName, PBYTE lpBuffer, DWORD dwSize);
	void PrintBuffer(string &strOutput, PBYTE lpBuffer, DWORD dwSize, UINT uiLineCount = 16);
	void Record(const char *lpFmt, ...);
	
protected:
private:
	string m_path;
	string  m_name;
	bool    m_enable;
	bool Write(string text);
};
typedef enum {
	STAT_NOT_EXIST = 0,
	STAT_FILE,
	STAT_DIR
} ENUM_FILE_STAT;
int file_stat(string strPath);
#endif
