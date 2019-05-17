// TempAndPower.h : main header file for the TEMPANDPOWER application
//

#if !defined(AFX_TEMPANDPOWER_H__A471BD7A_540E_45C2_A59C_EAAC26976255__INCLUDED_)
#define AFX_TEMPANDPOWER_H__A471BD7A_540E_45C2_A59C_EAAC26976255__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTempAndPowerApp:
// See TempAndPower.cpp for the implementation of this class
//

class CTempAndPowerApp : public CWinApp
{
public:
	CTempAndPowerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTempAndPowerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTempAndPowerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPANDPOWER_H__A471BD7A_540E_45C2_A59C_EAAC26976255__INCLUDED_)
