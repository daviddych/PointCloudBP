
// PointCloudBP.h : PointCloudBP Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CPointCloudBPApp:
// �йش����ʵ�֣������ PointCloudBP.cpp
//

class CPointCloudBPApp : public CWinApp
{
public:
	CPointCloudBPApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPointCloudBPApp theApp;
