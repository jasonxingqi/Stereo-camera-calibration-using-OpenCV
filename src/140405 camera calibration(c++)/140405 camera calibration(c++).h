
// 140405 camera calibration(c++).h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMy140405cameracalibrationcApp:
// See 140405 camera calibration(c++).cpp for the implementation of this class
//

class CMy140405cameracalibrationcApp : public CWinApp
{
public:
	CMy140405cameracalibrationcApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMy140405cameracalibrationcApp theApp;