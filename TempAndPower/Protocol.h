
#ifndef __PROTOCOL_H
#define __PROTOCOL_H

/*------------------------------------------------------------------
	communication protocol frame

	STX,Node number,Sub-Address,SID,Command Text,ETX,BCC
	STX:			This code (H02) indicates the beginning of the communications frame (text).
						Always set this character in the first byte.
					When STX is received again during reception, reception is carried out again from
					the point where STX was received.

	Node number:	This number specifies the transmission's destination.
					Specify the E5CN's communications unit number.
					A BCD value between 00 and 99 or an ASCII value of XX can be set.
					Specify "XX" for a broadcast transmission. No responses will be returned for broadcast transmissions.
					No responses will be returned from node numbers other than the ones in the above range

	Sub-Address:	The sub-address is not used in the E5CN. Be sure to set the sub-address to "00."
	SID:			The service ID is not used in the E5CN. Be sure to set the service ID to "00."
	Command Text:	This is the command text area. For details, see follow of Command Text.
	ETX:			This code (H03) indicates the end of the text.
------------------------------------------------------------------*/


class cmdframe{
public:
	enum{
		No_Error = 0,
			ReportLen_Error = 1,
			CRC_Error = 2,
			Number_Error = 3,
			SubAddress_Error = 4,
			Return_Error = 5,
	};

public:
	int GetGrameTextLen();
	char* GetFrameText(int *textlen);
	char* PickupCmdText(char* report, int len, int* textlen);
	int AnalyzeReport(char *reporttext, int len);
	char* CreateReport(char* commandtext);
	char CountCRC(char* mbuff, int len);
	cmdframe();
	~cmdframe();
	char			NodeNumber;
	char			SubAddress;
	char			SID;
	unsigned char	EndCode;
private:
	int m_frametextlen;
	char* m_frametext;
};

class cmdtext 
{
public:

	// enum list variable for TypeCode
	enum {	RO_AREA0 = 0xC0,
			RW_AREA0 = 0xC1,
			RW_AREA1 = 0xC3 };

	// enum list variable for MRC and SRC
	enum {	R_VARIABLE = 0x0101,
			W_VARIABLE = 0x0102,
			R_ATTRIBUTE = 0x0503,
			R_STATUS    =0x0601,
			ECHOBACK	= 0x0801,
			OPERATION_COMMAND = 0x3005 };

	// enum return report error status
	enum{
			No_Error = 0,
				MSrc_Error = 1,
				Responese_Error = 2,
	};
public:
	unsigned short	MSrc;		//hight byte is MRC  low byte is SRC
	unsigned char	TypeCode;
	short	Address;
	short	Elements;

public:
	unsigned short	MSres;
	
public:
	char* GetCmdText();
	char* CreateCmdText(int mMSrc, int mValType = 0, int mAddress = 0, int mElements = 0, char* mdata = 0);
	static void DeleteReportResult(cmdtext* cmd);
	int GetReportResult(char *report, int len);
	int GetCmdtextlen();
	char* GetCommandReport(int cmdCode);
	cmdtext();
	~cmdtext();
	unsigned short m_data1;
	unsigned short m_data2;
	char		m_rdata[50];		//从设备中读取回来的字符信息

	union ReturnResult{
		// Read_Attribute R_ATTRIBUTE
		struct{
			char m_ModelNo[11];
			short m_buffsize;
		};

		//R_VARIABLE   Temperature and status
		struct{
			float		m_Temperature;
			union{
				unsigned short	m_Status;
				struct{
					unsigned short m_01HeaderOver1:1;
					unsigned short m_02HeaderHold1:1;
					unsigned short m_03Headerbumout:1;
					unsigned short m_04HSAlarmouput:1;
					unsigned short m_05Reserver1:1;
					unsigned short m_06DispRangeExe:1;
					unsigned short m_07InputError:1;
					unsigned short m_08Reserver2:1;
					unsigned short m_09HeatingOutput:1;
					unsigned short m_10CoolingOutput:1;
					unsigned short m_11HBAlarmOuput1:1;
					unsigned short m_12HBAlarmOuput2:1;
					unsigned short m_13AlaramOutput1:1;
					unsigned short m_14AlaramOutput2:1;
					unsigned short m_15AlaramOutput3:1;
					unsigned short m_16ProgramEndOutput:1;
				};
			};
		};
	}m_Result;

private:
	int m_CommunicateCode;		//当前要通信读取的值
	void DeleteCmdText();
	
	int		m_cmdtextlen;
	char*	m_cmdtext;


};


class OmronTemperature
{
public:
	enum __ErrorState{ 
		No_Error = 0,
			ComPort_Error = -1,			//通信串行口错误
			NotReturn_Error = -2,		//设备无返回数据
			ReportFormat_Error = -3,	//返回的数据帧格式错误
			ReportCRC_Error = -4,		//返回的数据帧校验码错误

			Return_Error = -100,		//由设备返回的数据帧中有错误代码
	};
public:
	OmronTemperature();
	~OmronTemperature();

	int ReadVariable3();
	int ReadVariable2();
	int ReadVariable1();
	int ReadAttribute();

public:
	char		m_NodeNumber;

protected:
	cmdframe	m_commframe;
	cmdtext		m_commtext;

	// public Omron data
public:
	BOOL IsInitModule();
	BOOL UninitOmronTemp();
	BOOL InitOmronTemp(int iPort);
	BOOL SetTempRunState(int m_GoRun = 0);
	BOOL WriteVariable0();
	BOOL ReadSetVariable();
	HANDLE m_hCom;
	int AnalyzeBuffer();
	int ReadCommData();
	int ReadVariable();
	char GetNodeNumber();
	void SetNodeNumber(char mNumber);
	// Read_Attribute R_ATTRIBUTE
	// controller attribute
	char	m_ModelNo[11];
	short	m_buffsize;

	// Read_Variable Area
	// Variable Area data
	struct VariableArea{
		
		//R_VARIABLE   Temperature and status
		float			m_Temperature;				//	Area 0 0000
		union{										// Area 0 0001
			unsigned 			m_Status;
			struct {
				unsigned	m_00HeaderOver1:1;	
				unsigned	m_01HeaderHold1:1;
				unsigned	m_02Headerbumout:1;
				unsigned	m_03HSAlarmOuput:1;
				unsigned	m_04Reserver:1;
				unsigned	m_05DispRangeExe:1;
				unsigned	m_06InputError:1;
				unsigned	m_07Reserver:1;

				unsigned	m_08HeatingOutput:1;
				unsigned	m_09CoolingOutput:1;
				unsigned	m_10HBAlarmOuput1:1;
				unsigned	m_11HBAlarmOuput2:1;
				unsigned	m_12AlaramOutput1:1;
				unsigned	m_13AlaramOutput2:1;
				unsigned	m_14AlaramOutput3:1;
				unsigned	m_15ProgramEndOutput:1;

				unsigned	m_16EventInput1:1;
				unsigned	m_17EventInput2:1;
				unsigned	m_18Reserver:1;
				unsigned	m_19Reserver:1;
				unsigned	m_20WriteMode:1;
				unsigned	m_21EPROM:1;
				unsigned	m_22SetupArea:1;
				unsigned	m_23ATExecute:1;

				unsigned	m_24Run_Stop:1;
				unsigned	m_25CommWrite:1;
				unsigned	m_26Auto:1;
				unsigned	m_27Reserver:1;
				unsigned	m_28HeaderOver2:1;
				unsigned	m_29HeaderHold2:1;
				unsigned	m_30Reserver:1;
				unsigned	m_31HSAlarmOutput2:1;
			}m_ControllerStatus;
		};

		float			m_SettingTemp;				// Area 0 0002

		float			m_Heater1Current;			// Area 0 0003

		float			m_HMvMonitor;				//	Area 0 0004
		float			m_CMvMonitor;				//	Area 0 0005

		// R/W Area 0
		float			m_setpoint;					// 0003
		float			m_alarm1;					//0004
		float			m_alarm1up;					//0005
		float			m_alarm1down;
		float			m_alarm2;
		float			m_alarm2up;
		float			m_alarm2down;
		float			m_alarm3;
		float			m_alarm3up;
		float			m_alarm3down;
	}m_Variable;

private:
	void AnalyzeData();
	void ClearBuff(int ilen);
	enum {READBUFFSIZE = 128,};

	char		m_rbuff[READBUFFSIZE];
	unsigned	m_rdatalen;
};

#endif
