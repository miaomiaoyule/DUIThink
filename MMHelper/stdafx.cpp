// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// MMHelper.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H �������κ�����ĸ���ͷ�ļ���
//�������ڴ��ļ�������

//////////////////////////////////////////////////////////////////////////
void MMHELPER_API MMTrace(LPCTSTR pstrFormat, ...)
{
#ifdef _DEBUG
	CString strMsg;
	va_list Args;

	va_start(Args, pstrFormat);
	strMsg.FormatV(pstrFormat, Args);
	va_end(Args);

	strMsg += _T("\n");
	OutputDebugString(strMsg.GetBuffer(0));

#endif
}