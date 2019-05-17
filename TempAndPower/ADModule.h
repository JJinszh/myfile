// ADModule.h: interface for the CADModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADMODULE_H__DB48DA82_CBC7_44C8_AAF2_1B03BC4F54BE__INCLUDED_)
#define AFX_ADMODULE_H__DB48DA82_CBC7_44C8_AAF2_1B03BC4F54BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CADModule  
{
public:
	BOOL UninitAdModule();
	BOOL IsInitModule();
	BOOL ReadAdVoltage();
	enum{ADPORTS = 8, RBUFLEN = 128,};
	
	float operator [](int index);
	BOOL InitAdModule(int icomport, BOOL AdPortEn[CADModule::ADPORTS]);
	CADModule();
	virtual ~CADModule();

private:
	HANDLE				m_hcom;

	float				m_advoltage[ADPORTS];

	char	Rebuff[RBUFLEN];

	static const char		ReadAllCh[];
	static const int		ADDatalen;				//AD采集数据的长度
};

#endif // !defined(AFX_ADMODULE_H__DB48DA82_CBC7_44C8_AAF2_1B03BC4F54BE__INCLUDED_)
