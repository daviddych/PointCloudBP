#pragma once
#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////////////
// 进度条显示
/////////////////////////////////////////////////////////////////////////////////////
// CProgressStatusDlg 对话框

class CProgressStatusDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressStatusDlg)

public:
	CProgressStatusDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProgressStatusDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 进度条控件
	CProgressCtrl m_progress_ctrl;
	// 控制进度条前进或销毁窗口
	void StepIt(void);
	// 控制进度条显示位置或销毁窗口
	void SetPos(int nPos);

	virtual BOOL OnInitDialog();
};
