#pragma once
#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////////////
// ��������ʾ
/////////////////////////////////////////////////////////////////////////////////////
// CProgressStatusDlg �Ի���

class CProgressStatusDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressStatusDlg)

public:
	CProgressStatusDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProgressStatusDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// �������ؼ�
	CProgressCtrl m_progress_ctrl;
	// ���ƽ�����ǰ�������ٴ���
	void StepIt(void);
	// ���ƽ�������ʾλ�û����ٴ���
	void SetPos(int nPos);

	virtual BOOL OnInitDialog();
};
