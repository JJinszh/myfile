// TempAndPowerDlg.h : header file
//

#if !defined(AFX_TEMPANDPOWERDLG_H__66248601_9C7D_4C44_AC60_E642ADD1304B__INCLUDED_)
#define AFX_TEMPANDPOWERDLG_H__66248601_9C7D_4C44_AC60_E642ADD1304B__INCLUDED_

#include "Protocol.h"	// Added by ClassView
#include "ADModule.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTempAndPowerDlg dialog

class CTempAndPowerDlg : public CDialog
{
// Construction
public:
	CTempAndPowerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTempAndPowerDlg)
	enum { IDD = IDD_TEMPANDPOWER_DIALOG };
	int		m_powerport;
	int		m_tempport;
	int		m_saveintervaltime;
	CString	m_savefilename;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTempAndPowerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTempAndPowerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnStartmeasure();
	afx_msg void OnStopmeasure();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CADModule m_admodule;
	CFile m_hFile;
	OmronTemperature m_TempValiable;
	HANDLE OpenCommPort(int iport);
	HANDLE		m_hTempPort;
	HANDLE		m_hPowerPort;

	time_t		m_time;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPANDPOWERDLG_H__66248601_9C7D_4C44_AC60_E642ADD1304B__INCLUDED_)
