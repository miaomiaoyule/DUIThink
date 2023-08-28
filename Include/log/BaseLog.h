#pragma once
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif

//定义一个在日志后添加 文件名 函数名 行号 的宏定义
#ifndef suffix
#define suffix(msg)  std::string().append("[")\
        .append(__FILENAME__).append(" ").append(__func__)\
        .append(": ").append(std::to_string(__LINE__))\
        .append("] ").append(msg).c_str()
#endif

//在  spdlog.h   之前定义，才有效
#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"

class CXZBaseLog
{
private:
	CXZBaseLog() {}
	virtual ~CXZBaseLog(){}

public:
	static CXZBaseLog* GetInstance() {
		static CXZBaseLog m_instance;
		return &m_instance;
	}

	void InitLogger(const std::string& strPath)
	{
		char szPath[MAX_PATH] = { 0 };
		strcpy_s(szPath, strPath.c_str());
		PathRemoveExtensionA(szPath);
		std::string strFileName = PathFindFileNameA(szPath);
		PathRemoveFileSpecA(szPath);
		RemoveOldLog(szPath, strFileName.c_str());

		std::vector<spdlog::sink_ptr> sinkList;
#ifdef _CONSOLE
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consoleSink->set_level(spdlog::level::debug);
		consoleSink->set_pattern("[%m-%d %H:%M:%S.%e][%^%L%$] %v");
		sinkList.push_back(consoleSink);
#endif
		std::shared_ptr<spdlog::sinks::daily_file_sink_mt> basicSink = nullptr;
		//这里要捕获构造函数抛出的异常
		try
		{
			basicSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(strPath, 23, 59);
		}
		catch (spdlog::spdlog_ex & ex)
		{
			OutputDebugStringA(ex.what());
		}
		catch (...)                                                                                                                            \
		{                                                                                                                                      \
			OutputDebugStringA("Unknown exception in InitLogger");   
		}
		if (basicSink == nullptr)
		{
			return;
		}
		//basicSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
		sinkList.push_back(basicSink);
		nml_logger = std::make_shared<spdlog::logger>("both", begin(sinkList), end(sinkList));
		spdlog::register_logger(nml_logger);

		// 设置日志记录级别
#ifdef _DEBUG
		nml_logger->set_level(spdlog::level::trace);
#else
		nml_logger->set_level(spdlog::level::info);
#endif
		//设置 logger 格式[%^%L%$]
		nml_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
		//设置当出发 err 或更严重的错误时立刻刷新日志到  disk 
		nml_logger->flush_on(spdlog::level::info);
	}

	auto GetLogger() { return nml_logger; }

protected:
	void RemoveOldLog(LPCSTR szPath, LPCSTR szFileName)
	{
		SYSTEMTIME sys; 
		GetLocalTime(&sys); 
		char tmp[MAX_PATH] = {0};
		sprintf_s( tmp, "%s_%d-%02d-%02d.log", szFileName, sys.wYear, sys.wMonth, sys.wDay);
		std::string strCurrent_FileName = tmp;

		WIN32_FIND_DATAA FindData;
		HANDLE hError;
		char szFilePathName[MAX_PATH] = { 0 };
		char szFullPath[MAX_PATH] = { 0 };
		strcpy_s(szFilePathName, szPath);
		strcat_s(szFilePathName, "\\*.*");
		hError = FindFirstFileA(szFilePathName, &FindData);
		if (hError == INVALID_HANDLE_VALUE)
		{
			printf("搜索失败!");
			return;
		}

		while (::FindNextFileA(hError, &FindData))
		{
			// 过虑.和..
			if (strcmp(FindData.cFileName, ".") == 0
				|| strcmp(FindData.cFileName, "..") == 0)
			{
				continue;
			}
			std::string strFileName = FindData.cFileName;
			if (strCurrent_FileName == strFileName)
			{
				///< 遍历,检测到是当天的日志,不删除.
				continue;
			}
			strFileName = strFileName.substr(0, strFileName.find("_"));
			if (strFileName.compare(szFileName) == 0)
			{
				sprintf_s(szFullPath, "%s\\%s", szPath, FindData.cFileName);
				::DeleteFileA(szFullPath);
			}
		}
	}
private:
	std::shared_ptr<spdlog::logger> nml_logger = nullptr;
};

#define INITLOG(path)	CXZBaseLog::GetInstance()->InitLogger(path)

#define TRACELOG(msg,...) \
	if (CXZBaseLog::GetInstance()->GetLogger() != nullptr) \
	{ \
		CXZBaseLog::GetInstance()->GetLogger()->trace(suffix(msg), __VA_ARGS__); \
	}

#define DEBUGLOG(msg,...)	\
	if (CXZBaseLog::GetInstance()->GetLogger() != nullptr) \
	{ \
		CXZBaseLog::GetInstance()->GetLogger()->debug(suffix(msg), __VA_ARGS__); \
	} 	

#define ERRORLOG(msg,...)	\
	if (CXZBaseLog::GetInstance()->GetLogger() != nullptr) \
	{ \
		CXZBaseLog::GetInstance()->GetLogger()->error(suffix(msg), __VA_ARGS__); \
	}

#define INFOLOG(msg,...)	\
	if (CXZBaseLog::GetInstance()->GetLogger() != nullptr) \
	{ \
		CXZBaseLog::GetInstance()->GetLogger()->info(suffix(msg), __VA_ARGS__); \
	}

#define WARNLOG(...)		\
	if (CXZBaseLog::GetInstance()->GetLogger() != nullptr) \
	{ \
		CXZBaseLog::GetInstance()->GetLogger()->warn(__VA_ARGS__); \
	}
	
#define CRITICALLOG(...)	\
	if (CXZBaseLog::GetInstance()->GetLogger() != nullptr) \
	{ \
		CXZBaseLog::GetInstance()->GetLogger()->critical(__VA_ARGS__); \
	}