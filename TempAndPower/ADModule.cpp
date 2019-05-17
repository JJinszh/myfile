// ADModule.cpp: implementation of the CADModule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ADModule.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const char		CADModule::ReadAllCh[] = _T("#01\r");	//读所有通道的命令
const int		CADModule::ADDatalen = 7;				//AD采集数据的长度
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CADModule::CADModule()
{
	int i;

	for(i=0; i < ADPORTS; i++)
		m_advoltage[i] = 0.0f;

	m_hcom = INVALID_HANDLE_VALUE;
}

CADModule::~CADModule()
{

}

BOOL CADModule::InitAdModule(int icomport, BOOL AdPortEn[CADModule::ADPORTS])
{

	CString			szPort;
	COMMTIMEOUTS timeout;
	DCB dcb;
	DWORD ch = 0;
	CString sz;
	int i;

	if(m_hcom != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hcom);
		m_hcom = INVALID_HANDLE_VALUE;
	}

	szPort.Format(_T("\\\\.\\COM%d"), icomport);

	if((m_hcom = CreateFile((LPCTSTR)szPort,
								GENERIC_READ | GENERIC_WRITE,
								0, NULL, OPEN_EXISTING, 0, 0))==INVALID_HANDLE_VALUE)
	{
		szPort = _T("不能打开串口\n") + szPort;
		TRACE(szPort);
		return FALSE;
	}

	memset(&dcb,0,sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	::BuildCommDCB("baud=9600 parity=N data=8 stop=1 rts=on",&dcb);
	::SetCommState(m_hcom,&dcb);
	SetupComm(m_hcom,1024,512);
	SetCommMask(m_hcom,EV_RXCHAR);

	memset(&timeout,0,sizeof(COMMTIMEOUTS));
	//timeout.ReadTotalTimeoutMultiplier = 20;
	timeout.ReadIntervalTimeout = 0;
	timeout.ReadTotalTimeoutConstant = 100;
	::SetCommTimeouts(m_hcom,&timeout);

	//  选择使用的通道
	for(i=0; i < ADPORTS; i++)
		ch |= AdPortEn[i] ? (1 << i) : 0;

	sz.Format(_T("$015%02X\r"), ch);

	WriteFile(m_hcom, (LPCTSTR)sz, sz.GetLength(), &ch, NULL);

	return TRUE;
}

float CADModule::operator [](int index)
{
	ASSERT(index >=0 && index < ADPORTS);

	return m_advoltage[index];

}

BOOL CADModule::ReadAdVoltage()
{
	char ConvertB[10];
	DWORD dwlen;
	int i, ic;

	if(m_hcom == INVALID_HANDLE_VALUE)
		return FALSE;
	
	::PurgeComm(m_hcom, PURGE_TXCLEAR | PURGE_RXCLEAR);

	WriteFile(m_hcom, ReadAllCh, strlen(ReadAllCh), &dwlen, NULL);

		//Sleep(10);
	if(ReadFile(m_hcom, Rebuff, RBUFLEN -1, &dwlen, NULL))
	{
		if(dwlen > 0)
		{
			Rebuff[dwlen] = 0;
			//TRACE0(Rebuff);
			//TRACE0("\n");

			for(i=0, ic = 1; i < 8; i++)
			{
				//ic += i * 7;
				if(ic < (int)dwlen)
				{
					strncpy(ConvertB, Rebuff + ic, ADDatalen);
					ConvertB[ADDatalen] = 0;
					if(sscanf(ConvertB, "%7f", m_advoltage + i)<0)
					{
						m_advoltage[i] = 0;
					}
				}else
					break;
				ic += 7;
			}
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CADModule::IsInitModule()
{
	if(m_hcom == 0 || m_hcom == INVALID_HANDLE_VALUE)
		return FALSE;
	return TRUE;
}

BOOL CADModule::UninitAdModule()
{
	if(m_hcom && m_hcom != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hcom);
		m_hcom = INVALID_HANDLE_VALUE;
	}
	return TRUE;
}
