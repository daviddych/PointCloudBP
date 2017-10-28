// ProgressStatusDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PointCloudBP.h"
#include "ProgressStatusDlg.h"
#include "afxdialogex.h"


// CProgressStatusDlg �Ի���

IMPLEMENT_DYNAMIC(CProgressStatusDlg, CDialogEx)

CProgressStatusDlg::CProgressStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PROGRESS_DLG, pParent)
{
}

CProgressStatusDlg::~CProgressStatusDlg()
{
}

void CProgressStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress_ctrl);
}


BEGIN_MESSAGE_MAP(CProgressStatusDlg, CDialogEx)
END_MESSAGE_MAP()


void CProgressStatusDlg::StepIt(void)
{
	m_progress_ctrl.StepIt();
	int nPos = m_progress_ctrl.GetPos();
	int nLower, nUpper;
	m_progress_ctrl.GetRange(nLower, nUpper);
	if (nPos == nUpper )
	{
		PostMessage(WM_QUIT, 0, 0);
	}
	Invalidate(TRUE);
}


void CProgressStatusDlg::SetPos(int nPos)
{
	m_progress_ctrl.SetPos(nPos);
	int nLower, nUpper;
	m_progress_ctrl.GetRange(nLower, nUpper);
	if (nPos == nUpper)
	{
		PostMessage(WM_QUIT, 0, 0);
	}
	Invalidate(TRUE);
}

BOOL CProgressStatusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_progress_ctrl.SetRange(0, 100);
	m_progress_ctrl.SetStep(1);
	m_progress_ctrl.SetPos(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
