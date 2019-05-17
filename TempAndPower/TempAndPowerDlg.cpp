// TempAndPowerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TempAndPower.h"
#include "TempAndPowerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTempAndPowerDlg dialog

CTempAndPowerDlg::CTempAndPowerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTempAndPowerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTempAndPowerDlg)
	m_powerport = 3;
	m_tempport = 4;
	m_saveintervaltime = 0;
	m_savefilename = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hTempPort = NULL;
	m_hPowerPort = NULL;
}

void CTempAndPowerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTempAndPowerDlg)
	DDX_Text(pDX, IDC_POWRPORT, m_powerport);
	DDV_MinMaxInt(pDX, m_powerport, 1, 10);
	DDX_Text(pDX, IDC_TEMPPORT, m_tempport);
	DDV_MinMaxInt(pDX, m_tempport, 1, 10);
	DDX_Text(pDX, IDC_SAVETIME, m_saveintervaltime);
	DDV_MinMaxInt(pDX, m_saveintervaltime, 0, 2147483647);
	DDX_Text(pDX, IDC_SAVEFILE, m_savefilename);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTempAndPowerDlg, CDialog)
	//{{AFX_MSG_MAP(CTempAndPowerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_STARTMEASURE, OnStartmeasure)
	ON_BN_CLICKED(IDC_STOPMEASURE, OnStopmeasure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTempAndPowerDlg message handlers

BOOL CTempAndPowerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTempAndPowerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTempAndPowerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTempAndPowerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

HANDLE CTempAndPowerDlg::OpenCommPort(int iport)
{
	HANDLE hCom;

	//if(m_hCom && m_hCom != INVALID_HANDLE_VALUE)
	//	return TRUE;

	CString szport;

	if(iport < 1)
		return FALSE;

	szport.Format(_T("\\\\.\\com%d"), iport);

	hCom = CreateFile(szport, 
						GENERIC_READ | GENERIC_WRITE,
						0, NULL, OPEN_EXISTING, 0, 0);

	if(hCom != INVALID_HANDLE_VALUE)
	{
		COMMTIMEOUTS timeout;
		DCB dcb;
		memset(&dcb,0,sizeof(DCB));
		dcb.DCBlength = sizeof(DCB);
		::BuildCommDCB("baud=9600 parity=N data=8 stop=1",&dcb);
		::SetCommState(hCom,&dcb);
		SetupComm(hCom,1024,512);
		SetCommMask(hCom,EV_RXCHAR);

		memset(&timeout,0,sizeof(COMMTIMEOUTS));
		//timeout.ReadTotalTimeoutMultiplier = 20;
		timeout.ReadIntervalTimeout = 20;
		timeout.ReadTotalTimeoutConstant = 1000;
		::SetCommTimeouts(hCom,&timeout);

		return hCom;
	}	
	return NULL;
}

static const TCHAR TimeFmt[] = _T("%Y-%m-%d %H:%M:%S");
void CTempAndPowerDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	BOOL bReadTemp = FALSE;
	BOOL bReadPower = FALSE;
	CString szText;
	float   fval;
	CTime	m_t = CTime::GetCurrentTime();

	if(m_TempValiable.IsInitModule() && !m_TempValiable.ReadVariable1())
	{
		if(m_TempValiable.ReadVariable() == OmronTemperature::No_Error)
		{
			bReadTemp = TRUE;

			fval = m_TempValiable.m_Variable.m_Temperature;
			szText.Format(_T("%f"), fval);
			SetDlgItemText(IDC_CHECKTEMP, szText);
		}
	}

	if(m_admodule.IsInitModule() && m_admodule.ReadAdVoltage())
	{
		bReadPower = TRUE;
		fval = m_admodule[0];
		szText.Format(_T("%f"), fval);
		SetDlgItemText(IDC_CHECKPOWER, szText);
		//m_admodule[0];
	}

	if(m_hFile.m_hFile != (UINT)INVALID_HANDLE_VALUE && (m_saveintervaltime == 0 || time(0) - m_time > m_saveintervaltime))
	{
		if(bReadTemp || bReadPower)
		{
			szText = m_t.Format(TimeFmt);
			szText += _T("\t");
			m_hFile.Write(szText.GetBuffer(0), szText.GetLength());
		}

		if(bReadTemp)
		{
			fval = m_TempValiable.m_Variable.m_Temperature;

			szText.Format(_T("Temp = %f\t"), fval);
			m_hFile.Write(szText.GetBuffer(0), szText.GetLength());
		}
		
		if(bReadPower)
		{
			fval = m_admodule[0];

			szText.Format(_T("Laser power = %f\t"), fval);
			m_hFile.Write(szText.GetBuffer(0), szText.GetLength());
		}

		if(bReadTemp || bReadPower)
		{
			szText = _T("\r\n");
			m_hFile.Write(szText.GetBuffer(0), szText.GetLength());
		}

		m_time = time(0);
	}
	CDialog::OnTimer(nIDEvent);
}

void CTempAndPowerDlg::OnStartmeasure() 
{
	// TODO: Add your control notification handler code here
	if(!UpdateData())
		return;

	int i;
	BOOL  blInitPower;
	BOOL  blInitTemp;

	BOOL AdPortEn[CADModule::ADPORTS];

	for(i=0; i < CADModule::ADPORTS; i++)
		AdPortEn[i] = FALSE;

	AdPortEn[0] = TRUE;

	blInitPower = m_admodule.InitAdModule(m_powerport, AdPortEn);

	blInitTemp = m_TempValiable.InitOmronTemp(m_tempport);
	//m_hTempPort = OpenCommPort(m_tempport);
	//m_hPowerPort = OpenCommPort(m_powerport);

	//m_TempValiable.m_hCom = m_hTempPort;

	if(!blInitTemp && !blInitPower)
		return ;

	if(m_hFile.Open(m_savefilename, CFile::modeWrite | CFile::modeNoTruncate))// CFile::modeWrite))
	{
		m_hFile.SeekToEnd();
	}else
	{
		if(m_hFile.Open(m_savefilename, CFile::modeWrite | CFile::modeCreate))
		{
			m_hFile.SeekToEnd();
		}
	}

	m_time = time(0);

	SetTimer(1, 500,0);

	GetDlgItem(IDC_STARTMEASURE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOPMEASURE)->EnableWindow(TRUE);
}


void CTempAndPowerDlg::OnStopmeasure() 
{
	// TODO: Add your control notification handler code here
	KillTimer(1);

	m_TempValiable.UninitOmronTemp();
	m_admodule.UninitAdModule();

	GetDlgItem(IDC_STARTMEASURE)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOPMEASURE)->EnableWindow(FALSE);

	if(m_hFile.m_hFile && m_hFile.m_hFile != (UINT)INVALID_HANDLE_VALUE)
		m_hFile.Close();

}

void CTempAndPowerDlg::OnOK() 
{
	// TODO: Add extra validation here	
	if(GetDlgItem(IDC_STARTMEASURE)->IsWindowEnabled())
		CDialog::OnOK();
}
