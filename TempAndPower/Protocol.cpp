
#include "StdAfx.h"

#include "Protocol.h"

// STX(1) + Node Number(2) + Sub Address(2) + SID(1) + EXT(1) + BCC(1)
static const short FrameLen = 8;
static const short ReturnFrameLen = 9;

cmdframe::~cmdframe()
{
	if(m_frametext)
		delete[] m_frametext;
}

cmdframe::cmdframe()
{
	NodeNumber = 0;
	SubAddress = 0;
	SID = 0;
	m_frametext = 0;
	m_frametextlen = 0;
}

char cmdframe::CountCRC(char *mbuff, int len)
{
	char mcrc = 0;
	for(int i = 0; i < len; i++){
		mcrc ^= mbuff[i];
	}
	return mcrc;
}


char* cmdframe::CreateReport(char *commandtext)
{
	char m = 0;

	if(m_frametext)
		delete [] m_frametext;

	m_frametextlen = FrameLen + strlen(commandtext);

	m_frametext = new char[m_frametextlen];
	
	if(!m_frametext) {
		m_frametextlen = 0;
		return m_frametext;
	}
	
	m_frametext[0] = 0x2;
	sprintf(m_frametext + 1, "%02d"	"%02d"	"%1d"	"%s", 
			NodeNumber, 0, 0,
			commandtext);
	m_frametext[m_frametextlen - 2] = 0x3;
	
	m_frametext[m_frametextlen - 1] = CountCRC(m_frametext + 1, m_frametextlen-2);
	
	return m_frametext;
}

char* cmdframe::GetFrameText(int *textlen)
{
	*textlen = m_frametextlen;
	return m_frametext;
}

int cmdframe::GetGrameTextLen()
{
	return m_frametextlen;
}

int cmdframe::AnalyzeReport(char *reporttext, int len)
{
	char mNode, mSubAddress;
	if(len < 9) 
		return ReportLen_Error;

	char mcrc = CountCRC(reporttext+1, len -2);
	if(mcrc != reporttext[len-1])
		return CRC_Error;

	sscanf(reporttext + 1, "%2x"	"%2x"	"%2x",
						&mNode, &mSubAddress, &EndCode);

	if(mNode != NodeNumber) 
		return Number_Error;

	if(mSubAddress != SubAddress)
		return SubAddress_Error;

	if(EndCode)
		return Return_Error;
	
	return No_Error;
}

/*-----------------------------------------------------------------
	从返回的数据帧中提取出有效的命令字符串
	返回值为命令串的起始位置
	textlen返回的是命令串的长度
-----------------------------------------------------------------*/
char* cmdframe::PickupCmdText(char *report, int len, int *textlen)
{
	if(AnalyzeReport(report,len) == No_Error){
		*textlen = len - ReturnFrameLen;
		return report + ReturnFrameLen - 2;		//-2表示结尾的结束符同校验码
	}
	*textlen = 0;
	return NULL;
}


cmdtext::cmdtext()
{
	m_cmdtextlen = 0;
	m_cmdtext = 0;
	Address = 0;
	Elements = 0;
	m_data1 = 0;
	m_data2 = 0;
	MSres = 0;
	MSrc = 0;
	TypeCode = 0;
}

cmdtext::~cmdtext()
{
	if(m_cmdtext)
		delete[] m_cmdtext;
}


void cmdtext::DeleteCmdText()
{
	if(m_cmdtext)
		delete[] m_cmdtext;

	m_cmdtext = 0;
	m_cmdtextlen = 0;
}

char* cmdtext::GetCommandReport(int cmdCode)
{
	//int textlen;// = FrmHeadLen;
	//char *sbuff;

	return 0;
	//DeleteCmdText();
	
	/*
	m_CommunicateCode = cmdCode;

	switch(cmdCode){
	case Read_Attribute:
		textlen = 4;
		sbuff = new char[5];
		sprintf(sbuff,"%04X", R_ATTRIBUTE);
		break;

	case Read_TempStatus:
		textlen = 16;
		TypeCode = RO_AREA0;
		sbuff = new char[textlen + 1];
		sprintf(sbuff,	"%04x" 	"%02x"	"%04x"	"00"	"0002",
					R_VARIABLE,
					RO_AREA0,
					Address);
		break;
	default:

		return 0;
	}

	if(!CreateCmdText(textlen, sbuff)){
		if(textlen) delete[] sbuff;
		return 0;
	}
	//InitFrameText(sbuff);
	if(textlen) delete[] sbuff;

	return m_cmdtext;
	*/
}



int cmdtext::GetCmdtextlen()
{
	return m_cmdtextlen;
}

int cmdtext::GetReportResult(char *report, int len)
{
	//char	mtext[50];
	//char*	mtext;
	unsigned short mMSrc, mMSres;
	//union ReturnResult *result;
	sscanf(report,"%4X%4X", &mMSrc, &mMSres);

	if(mMSrc != MSrc)
		return MSrc_Error;
	if(mMSres)
		return Responese_Error;
	MSres = mMSres;

	CString sz;

	if(len > 8){
		sz.Format("Hello2 %X", (unsigned)m_rdata);
		//AfxMessageBox(sz);
		strncpy(m_rdata, report + 8, len - 8);
		m_rdata[len - 8] = 0;
	}else{
		m_rdata[0] = 0;
	}
	return No_Error;
}

void cmdtext::DeleteReportResult(cmdtext *cmd)
{
	delete cmd;
}



char* cmdtext::CreateCmdText(int mMSrc, int mValType, int mAddress, int mElements, char* mdata)
{
	int x;

	if(m_cmdtext)
	{
		delete[] m_cmdtext;
		m_cmdtextlen = 0;
	}

	MSrc = mMSrc;
	Address = mAddress;
	Elements = mElements;
	TypeCode = mValType;

	switch(mMSrc){
	case R_ATTRIBUTE:
		m_cmdtextlen = 4;
		m_cmdtext = new char[m_cmdtextlen + 1];
		sprintf(m_cmdtext,"%04X", R_ATTRIBUTE);
		break;

	case R_VARIABLE:
		m_cmdtextlen = 16;
		TypeCode = mValType;
		m_cmdtext = new char[m_cmdtextlen + 1];
						// Mrc+Src	Valtype Address	bit offset number of elements
		sprintf(m_cmdtext,	"%04X" 	"%02X"	"%04X"	"00"	"%04X",
						R_VARIABLE,
						TypeCode,
						Address,
						Elements);

		break;
	case W_VARIABLE:
		if(!mdata) return 0;
		x = strlen(mdata);
		if( 8 * Elements != x) return 0;

		m_cmdtextlen = 16 + x;

		TypeCode = mValType;
		m_cmdtext = new char[m_cmdtextlen + 1];							
																	// write data		
		sprintf(m_cmdtext,	"%04X" 	"%02X"	"%04X"	"00"	"%04X"		"%s",
						W_VARIABLE,
						TypeCode,
						Address,
						Elements,
						mdata);
		break;

	case OPERATION_COMMAND:
		m_cmdtextlen = 8;
		TypeCode = mValType;
		m_cmdtext = new char[m_cmdtextlen + 1];
						// Mrc+Src	(be wrote command code and information)
		sprintf(m_cmdtext,	"%04X" 	"%s",
						OPERATION_COMMAND,
						mdata);

		break;
	default:

		return 0;
	}

	return m_cmdtext;
}



char* cmdtext::GetCmdText()
{
	return m_cmdtext;
}


/*--------------------------------------------------------------
	class OmronTemperature
--------------------------------------------------------------*/

OmronTemperature::~OmronTemperature()
{

}

OmronTemperature::OmronTemperature()
{
	m_hCom = NULL;
	SetNodeNumber(0);
	m_rdatalen = 0;
	memset(&m_Variable,0,sizeof(VariableArea));
}

int OmronTemperature::ReadAttribute()
{
	char* msbuff;
	DWORD dwlen;

	msbuff = m_commtext.CreateCmdText(cmdtext::R_ATTRIBUTE);
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return -1;

		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL))
			return 0;
	}
	return -1;
}

int OmronTemperature::ReadVariable1()
{
	char* msbuff;
	DWORD dwlen;

	msbuff = m_commtext.CreateCmdText(cmdtext::R_VARIABLE, cmdtext::RO_AREA0, 0, 2);
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff)
	{
		if(!m_hCom) return -1;

		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL))
			return 0;
	}
	return -1;
}

int OmronTemperature::ReadVariable2()
{
	char* msbuff;
	DWORD dwlen;

	msbuff = m_commtext.CreateCmdText(cmdtext::R_VARIABLE, cmdtext::RO_AREA0, 2, 2);
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return -1;
		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL))
			return 0;
	}
	return -1;
}

int OmronTemperature::ReadVariable3()
{
	char* msbuff;
	DWORD dwlen;

	msbuff = m_commtext.CreateCmdText(cmdtext::R_VARIABLE, cmdtext::RO_AREA0, 4, 2);
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return -1;
		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL))
			return 0;
	}
	return -1;
}

void OmronTemperature::SetNodeNumber(char mNumber)
{
	if(mNumber > -1 && mNumber < 100)
		m_commframe.NodeNumber = mNumber;
	else
		m_commframe.NodeNumber = 0;
}

char OmronTemperature::GetNodeNumber()
{
	return m_commframe.NodeNumber;
}

int OmronTemperature::ReadVariable()
{
	char *msbuff;
	int	ilen;
	int	 endpos = 0, i;
	int iResult, iResult1;

	iResult = ReadCommData();
	
	if(iResult < 0) 
		return iResult;

	iResult = No_Error;
	
	endpos = AnalyzeBuffer();
	
	if(endpos){
		ilen = endpos + 1;

		iResult1 = m_commframe.AnalyzeReport(m_rbuff, ilen);
		
		if(iResult1 == cmdframe::No_Error){
					
			msbuff = m_commframe.PickupCmdText(m_rbuff, ilen, &i);
			
			if(msbuff && i){
				if(m_commtext.GetReportResult(msbuff,i) == cmdtext::No_Error){
					
					AnalyzeData();
					ClearBuff(endpos + 1);
					return iResult;
				}
			}else if(!i){
				ClearBuff(endpos + 1);
				return 0;
			}
		}else if(iResult == cmdframe::CRC_Error)

			iResult = ReportCRC_Error;
		else
			iResult = Return_Error;

		ClearBuff(endpos + 1);	
	}else
		iResult = ReportFormat_Error;

	return iResult;
}

int OmronTemperature::ReadCommData()
{
	DWORD dwlen;
	char mbuff[READBUFFSIZE];

	ASSERT(m_hCom);

	if(!m_hCom) return ComPort_Error;

	if(ReadFile(m_hCom,mbuff,READBUFFSIZE,&dwlen,NULL) && dwlen){
		unsigned ilen = READBUFFSIZE - m_rdatalen;

		if(ilen >= dwlen){
			memcpy(m_rbuff + m_rdatalen, mbuff, dwlen);
			m_rdatalen += dwlen;
		}else{
			ilen = dwlen - ilen;
			if(ilen == m_rdatalen){
				memcpy(m_rbuff, mbuff, dwlen);
				m_rdatalen = dwlen;
			}else{
				memcpy(m_rbuff, m_rbuff + ilen, m_rdatalen - ilen);
				m_rdatalen -= ilen;
			}
		}

		return (int)dwlen;
	}
	return NotReturn_Error;
}

int OmronTemperature::AnalyzeBuffer()
{
	if(m_rdatalen > 9){
		DWORD i;
		for(i = 0; i < m_rdatalen; i++){
			if(m_rbuff[i] == 0x2){		// look for start position
				if(i){
					memcpy(m_rbuff, m_rbuff + i, m_rdatalen - i);
					m_rdatalen -= i;
					i = 0;
				}
				break;
			}
		}

		if(i < m_rdatalen && m_rdatalen - i >= 9){ // found start position
			for(i = 7; i < m_rdatalen; i++){
				if(m_rbuff[i] == 0x3){		// look for end position
					if(i < m_rdatalen -1){
						return i + 1;			//	return effect position
					}
					break;
				}
			}
		}
	}
	return 0;
}

void OmronTemperature::ClearBuff(int ilen)
{
	if((unsigned)ilen >= m_rdatalen)
		m_rdatalen = 0;
	else{
		memcpy(m_rbuff, m_rbuff + ilen, m_rdatalen - ilen);
		m_rdatalen -= ilen;
	}		
}

void OmronTemperature::AnalyzeData()
{
	if(m_commtext.MSrc == cmdtext::R_ATTRIBUTE){
		strncpy(m_ModelNo,m_commtext.m_rdata,10);
		m_ModelNo[10] = '\0';

		sscanf(m_commtext.m_rdata + 10, "%4X", &m_buffsize);
	}else if(m_commtext.MSrc == cmdtext::R_VARIABLE && m_commtext.Elements > 1){
		int idata1,idata2;

		sscanf(m_commtext.m_rdata, "%8X" "%8X", &idata1, &idata2);

		if(m_commtext.TypeCode == cmdtext::RO_AREA0){	
			switch(m_commtext.Address){
			case 0:
				m_Variable.m_Temperature = ((float)idata1) / 10;
				m_Variable.m_Status = (unsigned) idata2;
				break;
			case 2:
				m_Variable.m_SettingTemp = ((float)idata1) / 10;
				m_Variable.m_Heater1Current = ((float)idata2) / 10;
				break;
			case 4:
				m_Variable.m_HMvMonitor = ((float)idata1) / 10;
				m_Variable.m_CMvMonitor = ((float)idata2) / 10;
				break;
			}
		}else if(m_commtext.TypeCode == cmdtext::RW_AREA0){
			switch(m_commtext.Address){
			case 0x03:
				m_Variable.m_setpoint = ((float)idata1) / 10;
				m_Variable.m_alarm1 = ((float)idata2) / 10;
				break;
			case 0x05:
				m_Variable.m_alarm1up = ((float)idata1) / 10;
				m_Variable.m_alarm1down = ((float)idata2) / 10;
				break;
			case 0x07:
				m_Variable.m_alarm2 = ((float)idata1) / 10;
				m_Variable.m_alarm2up = ((float)idata2) / 10;
				break;
			case 0x09:
				m_Variable.m_alarm2down = ((float)idata1) / 10;
				m_Variable.m_alarm3 = ((float)idata2) / 10;
				break;
			case 0x0b:
				m_Variable.m_alarm3up = ((float)idata1) / 10;
				m_Variable.m_alarm3down = ((float)idata2) / 10;
				break;
			}
		}
	}
}

BOOL OmronTemperature::ReadSetVariable()
{
	char* msbuff;
	DWORD dwlen;
	//int		ilen;
	//int	 endpos = 0, i;

	msbuff = m_commtext.CreateCmdText(cmdtext::R_VARIABLE, cmdtext::RW_AREA0, 3, 2);
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;

		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}else
			return FALSE;
	}

	msbuff = m_commtext.CreateCmdText(cmdtext::R_VARIABLE, cmdtext::RW_AREA0, 0x5, 2);
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;

		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}
	}

	msbuff = m_commtext.CreateCmdText(cmdtext::R_VARIABLE, cmdtext::RW_AREA0, 7, 2);
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;

		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}
	}

	msbuff = m_commtext.CreateCmdText(cmdtext::R_VARIABLE, cmdtext::RW_AREA0, 9, 2);
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;

		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}else
			return FALSE;
	}

	msbuff = m_commtext.CreateCmdText(cmdtext::R_VARIABLE, cmdtext::RW_AREA0, 0xa, 2);
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;

		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}else
			return FALSE;
	}
	return TRUE;
}

BOOL OmronTemperature::WriteVariable0()
{
	char* msbuff;
	DWORD dwlen;
	CString sz;
	int idata1, idata2;
	

	idata1 = (int)(m_Variable.m_setpoint * 10);
	idata2 = (int)(m_Variable.m_alarm1 * 10);
	sz.Format("%08X" "%08X",idata1,idata2);
	msbuff = m_commtext.CreateCmdText(cmdtext::W_VARIABLE, cmdtext::RW_AREA0, 3, 2, sz.GetBuffer(sz.GetLength()));
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;
		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}
	}

	idata1 = (int)(m_Variable.m_alarm1up * 10);
	idata2 = (int)(m_Variable.m_alarm1down * 10);
	sz.Format("%08X" "%08X",idata1,idata2);
	msbuff = m_commtext.CreateCmdText(cmdtext::W_VARIABLE, cmdtext::RW_AREA0, 5, 2, sz.GetBuffer(sz.GetLength()));
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;
		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}
	}

	idata1 = (int)(m_Variable.m_alarm2 * 10);
	idata2 = (int)(m_Variable.m_alarm2up * 10);
	sz.Format("%08X" "%08X",idata1,idata2);
	msbuff = m_commtext.CreateCmdText(cmdtext::W_VARIABLE, cmdtext::RW_AREA0, 7, 2, sz.GetBuffer(sz.GetLength()));
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;
		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}
	}

	idata1 = (int)(m_Variable.m_alarm2down * 10);
	idata2 = (int)(m_Variable.m_alarm3 * 10);
	sz.Format("%08X" "%08X",idata1,idata2);
	msbuff = m_commtext.CreateCmdText(cmdtext::W_VARIABLE, cmdtext::RW_AREA0, 9, 2, sz.GetBuffer(sz.GetLength()));
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;
		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}
	}

	idata1 = (int)(m_Variable.m_alarm3up * 10);
	idata2 = (int)(m_Variable.m_alarm3down * 10);
	sz.Format("%08X" "%08X",idata1,idata2);
	msbuff = m_commtext.CreateCmdText(cmdtext::W_VARIABLE, cmdtext::RW_AREA0, 0xa, 2, sz.GetBuffer(sz.GetLength()));
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;
		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}
	}

	return TRUE;
}

BOOL OmronTemperature::SetTempRunState(int m_GoRun)
{
	char* msbuff;
	DWORD dwlen;
	CString sz;
	int idata1;
	

	idata1 = m_GoRun ? 1 : 0;
	sz.Format("%02X" "%02X", 0x1, idata1);
	msbuff = m_commtext.CreateCmdText(cmdtext::OPERATION_COMMAND, 0, 0, 0, sz.GetBuffer(sz.GetLength()));
	msbuff = m_commframe.CreateReport(msbuff);

	if(msbuff){
		if(!m_hCom) return FALSE;
		if(WriteFile(m_hCom, msbuff, m_commframe.GetGrameTextLen(), &dwlen, NULL)){
			if(ReadVariable())
				return FALSE;
		}
	}

	return TRUE;
}

BOOL OmronTemperature::InitOmronTemp(int iPort)
{
	//HANDLE hCom;

	if(m_hCom && m_hCom != INVALID_HANDLE_VALUE)
		return TRUE;

	CString szport;

	if(iPort < 1)
		return FALSE;

	szport.Format(_T("\\\\.\\com%d"), iPort);

	m_hCom = CreateFile(szport, 
						GENERIC_READ | GENERIC_WRITE,
						0, NULL, OPEN_EXISTING, 0, 0);

	if(m_hCom != INVALID_HANDLE_VALUE)
	{
		COMMTIMEOUTS timeout;
		DCB dcb;
		memset(&dcb,0,sizeof(DCB));
		dcb.DCBlength = sizeof(DCB);
		::BuildCommDCB("baud=9600 parity=N data=8 stop=1",&dcb);
		::SetCommState(m_hCom,&dcb);
		SetupComm(m_hCom,1024,512);
		SetCommMask(m_hCom,EV_RXCHAR);

		memset(&timeout,0,sizeof(COMMTIMEOUTS));
		//timeout.ReadTotalTimeoutMultiplier = 20;
		timeout.ReadIntervalTimeout = 20;
		timeout.ReadTotalTimeoutConstant = 1000;
		::SetCommTimeouts(m_hCom,&timeout);

		return TRUE;
	}	
	return FALSE;
}

BOOL OmronTemperature::UninitOmronTemp()
{
	if(m_hCom && m_hCom != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
	}
	return TRUE;
}

BOOL OmronTemperature::IsInitModule()
{
	if(m_hCom == 0 || m_hCom == INVALID_HANDLE_VALUE)
		return FALSE;

	return TRUE;
}
