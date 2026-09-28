#pragma once

class CMMTimeSpan
{
private:
	__time64_t m_timeSpan;

public:
	inline CMMTimeSpan() throw() :
		m_timeSpan(0)
	{
	}

	inline CMMTimeSpan(_In_ __time64_t time) throw() :
		m_timeSpan(time)
	{
	}

	inline CMMTimeSpan(
		_In_ LONG lDays,
		_In_ int nHours,
		_In_ int nMins,
		_In_ int nSecs) throw()
	{
		m_timeSpan = nSecs + 60 * (nMins + 60 * (nHours + __int64(24) * lDays));
	}

	inline LONGLONG GetDays() const throw()
	{
		return(m_timeSpan / (24 * 3600));
	}

	inline LONGLONG GetTotalHours() const throw()
	{
		return(m_timeSpan / 3600);
	}

	inline LONG GetHours() const throw()
	{
		return(LONG(GetTotalHours() - (GetDays() * 24)));
	}

	inline LONGLONG GetTotalMinutes() const throw()
	{
		return(m_timeSpan / 60);
	}

	inline LONG GetMinutes() const throw()
	{
		return(LONG(GetTotalMinutes() - (GetTotalHours() * 60)));
	}

	inline LONGLONG GetTotalSeconds() const throw()
	{
		return(m_timeSpan);
	}

	inline LONG GetSeconds() const throw()
	{
		return(LONG(GetTotalSeconds() - (GetTotalMinutes() * 60)));
	}

	inline __time64_t GetTimeSpan() const throw()
	{
		return(m_timeSpan);
	}

	inline CMMTimeSpan operator+(_In_ CMMTimeSpan span) const throw()
	{
		return(CMMTimeSpan(m_timeSpan + span.m_timeSpan));
	}

	inline CMMTimeSpan operator-(_In_ CMMTimeSpan span) const throw()
	{
		return(CMMTimeSpan(m_timeSpan - span.m_timeSpan));
	}

	inline CMMTimeSpan &operator+=(_In_ CMMTimeSpan span) throw()
	{
		m_timeSpan += span.m_timeSpan;
		return(*this);
	}

	inline CMMTimeSpan &operator-=(_In_ CMMTimeSpan span) throw()
	{
		m_timeSpan -= span.m_timeSpan;
		return(*this);
	}

	inline bool operator==(_In_ CMMTimeSpan span) const throw()
	{
		return(m_timeSpan == span.m_timeSpan);
	}

	inline bool operator!=(_In_ CMMTimeSpan span) const throw()
	{
		return(m_timeSpan != span.m_timeSpan);
	}

	inline bool operator<(_In_ CMMTimeSpan span) const throw()
	{
		return(m_timeSpan < span.m_timeSpan);
	}

	inline bool operator>(_In_ CMMTimeSpan span) const throw()
	{
		return(m_timeSpan > span.m_timeSpan);
	}

	inline bool operator<=(_In_ CMMTimeSpan span) const throw()
	{
		return(m_timeSpan <= span.m_timeSpan);
	}

	inline bool operator>=(_In_ CMMTimeSpan span) const throw()
	{
		return(m_timeSpan >= span.m_timeSpan);
	}
};

/////////////////////////////////////////////////////////////////////////////
// CMMTime
/////////////////////////////////////////////////////////////////////////////
class CMMTime
{
private:
	__time64_t m_time;

public:
	inline CMMTime WINAPI GetCurrentTime() throw()
	{
		return(CMMTime(::_time64(NULL)));
	}

	inline CMMTime() throw() :
		m_time(0)
	{
	}

	inline CMMTime(_In_ __time64_t time)  throw() :
		m_time(time)
	{
	}

	inline CMMTime(
		_In_ int nYear,
		_In_ int nMonth,
		_In_ int nDay,
		_In_ int nHour,
		_In_ int nMin,
		_In_ int nSec,
		_In_ int nDST = -1)
	{
#pragma warning (push)
#pragma warning (disable: 4127)  // conditional expression constant

		assert(nYear >= 1970);
		assert(nMonth >= 1 && nMonth <= 12);
		assert(nDay >= 1 && nDay <= 31);
		assert(nHour >= 0 && nHour <= 23);
		assert(nMin >= 0 && nMin <= 59);
		assert(nSec >= 0 && nSec <= 59);

#pragma warning (pop)

		struct tm atm;

		atm.tm_sec = nSec;
		atm.tm_min = nMin;
		atm.tm_hour = nHour;
		atm.tm_mday = nDay;
		atm.tm_mon = nMonth - 1;        // tm_mon is 0 based
		atm.tm_year = nYear - 1900;     // tm_year is 1900 based
		atm.tm_isdst = nDST;

		m_time = _mktime64(&atm);
		assert(m_time != -1);       // indicates an illegal input time
		if (m_time == -1)
		{
			throw(_T("time is -1"));
		}
	}

	inline CMMTime(
		_In_ WORD wDosDate,
		_In_ WORD wDosTime,
		_In_ int nDST = -1)
	{
		struct tm atm;
		atm.tm_sec = (wDosTime & ~0xFFE0) << 1;
		atm.tm_min = (wDosTime & ~0xF800) >> 5;
		atm.tm_hour = wDosTime >> 11;

		atm.tm_mday = wDosDate & ~0xFFE0;
		atm.tm_mon = ((wDosDate & ~0xFE00) >> 5) - 1;
		atm.tm_year = (wDosDate >> 9) + 80;
		atm.tm_isdst = nDST;
		m_time = _mktime64(&atm);
		assert(m_time != -1);       // indicates an illegal input time
		if (m_time == -1)
		{
			throw(_T("time is -1"));
		}
	}

	inline CMMTime(
		_In_ const SYSTEMTIME &sysTime,
		_In_ int nDST = -1)
	{
		if (sysTime.wYear < 1900)
		{
			__time64_t time0 = 0L;
			CMMTime timeT(time0);
			*this = timeT;
		}
		else
		{
			CMMTime timeT(
				(int)sysTime.wYear, (int)sysTime.wMonth, (int)sysTime.wDay,
				(int)sysTime.wHour, (int)sysTime.wMinute, (int)sysTime.wSecond,
				nDST);
			*this = timeT;
		}
	}

	inline CMMTime &operator=(_In_ __time64_t time) throw()
	{
		m_time = time;

		return(*this);
	}

	inline CMMTime &operator+=(_In_ CMMTimeSpan span) throw()
	{
		m_time += span.GetTimeSpan();

		return(*this);
	}

	inline CMMTime &operator-=(_In_ CMMTimeSpan span) throw()
	{
		m_time -= span.GetTimeSpan();

		return(*this);
	}

	inline CMMTimeSpan operator-(_In_ CMMTime time) const throw()
	{
		return(CMMTimeSpan(m_time - time.m_time));
	}

	inline CMMTime operator-(_In_ CMMTimeSpan span) const throw()
	{
		return(CMMTime(m_time - span.GetTimeSpan()));
	}

	inline CMMTime operator+(_In_ CMMTimeSpan span) const throw()
	{
		return(CMMTime(m_time + span.GetTimeSpan()));
	}

	inline bool operator==(_In_ CMMTime time) const throw()
	{
		return(m_time == time.m_time);
	}

	inline bool operator!=(_In_ CMMTime time) const throw()
	{
		return(m_time != time.m_time);
	}

	inline bool operator<(_In_ CMMTime time) const throw()
	{
		return(m_time < time.m_time);
	}

	inline bool operator>(_In_ CMMTime time) const throw()
	{
		return(m_time > time.m_time);
	}

	inline bool operator<=(_In_ CMMTime time) const throw()
	{
		return(m_time <= time.m_time);
	}

	inline bool operator>=(_In_ CMMTime time) const throw()
	{
		return(m_time >= time.m_time);
	}

	inline _Success_(return != NULL) struct tm *GetGmtTm(_Out_ struct tm *ptm) const
	{
		// Ensure ptm is valid
		assert(ptm != NULL);

		if (ptm != NULL)
		{
			struct tm ptmTemp;
			errno_t err = _gmtime64_s(&ptmTemp, &m_time);

			// Be sure the call succeeded
			if (err != 0) { return NULL; }

			*ptm = ptmTemp;
			return ptm;
		}

		return NULL;
	}

	inline _Success_(return != NULL) struct tm *GetLocalTm(_Out_ struct tm *ptm) const
	{
		// Ensure ptm is valid
		assert(ptm != NULL);

		if (ptm != NULL)
		{
			struct tm ptmTemp;
			errno_t err = _localtime64_s(&ptmTemp, &m_time);

			if (err != 0)
			{
				return NULL;    // indicates that m_time was not initialized!
			}

			*ptm = ptmTemp;
			return ptm;
		}

		return NULL;
	}

	inline _Success_(return != false) bool GetAsSystemTime(_Out_ SYSTEMTIME &timeDest) const throw()
	{
		struct tm ttm;
		struct tm *ptm;

		ptm = GetLocalTm(&ttm);
		if (!ptm)
		{
			return false;
		}

		timeDest.wYear = (WORD)(1900 + ptm->tm_year);
		timeDest.wMonth = (WORD)(1 + ptm->tm_mon);
		timeDest.wDayOfWeek = (WORD)ptm->tm_wday;
		timeDest.wDay = (WORD)ptm->tm_mday;
		timeDest.wHour = (WORD)ptm->tm_hour;
		timeDest.wMinute = (WORD)ptm->tm_min;
		timeDest.wSecond = (WORD)ptm->tm_sec;
		timeDest.wMilliseconds = 0;

		return true;
	}

	inline __time64_t GetTime() const throw()
	{
		return(m_time);
	}

	inline int GetYear() const throw()
	{
		struct tm ttm;
		struct tm *ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? (ptm->tm_year) + 1900 : 0;
	}

	inline int GetMonth() const throw()
	{
		struct tm ttm;
		struct tm *ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_mon + 1 : 0;
	}

	inline int GetDay() const throw()
	{
		struct tm ttm;
		struct tm *ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_mday : 0;
	}

	inline int GetHour() const throw()
	{
		struct tm ttm;
		struct tm *ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_hour : -1;
	}

	inline int GetMinute() const throw()
	{
		struct tm ttm;
		struct tm *ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_min : -1;
	}

	inline int GetSecond() const throw()
	{
		struct tm ttm;
		struct tm *ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_sec : -1;
	}

	inline int GetDayOfWeek() const throw()
	{
		struct tm ttm;
		struct tm *ptm;

		ptm = GetLocalTm(&ttm);
		return ptm ? ptm->tm_wday + 1 : 0;
	}
};