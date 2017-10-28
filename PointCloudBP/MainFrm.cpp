
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "PointCloudBP.h"

#include "MainFrm.h"
#include "PointCloudBPView.h"
#include "FileObj.h"
#include "PointCloudBPDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_LOADFILE_COMPLETE_MSG, LoadFileComplete_MsgHandler)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;     
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	SetWindowText("PointCloudBP");
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style &= ~FWS_ADDTOTITLE;

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序
LRESULT CMainFrame::LoadFileComplete_MsgHandler(WPARAM wparam, LPARAM lparam)
{
	CPointCloudBPView* pView = (CPointCloudBPView*)GetActiveView();
	CPointCloudBPDoc*  pDoc = pView->GetDocument();
	FileObj* fileobj = pDoc->m_fileobj;
	if (fileobj == nullptr)
		return -1;
	
	// 允许多次导入数据进行叠加显示。
	glm::vec3 offset;
	if (!pView->m_scene.empty())
	{
		glm::vec3 offset = pView->m_scene.get_scene_offset();
		fileobj->update_offset(offset);
	}
	GPointCloud *pointCloud = new GPointCloud(fileobj->get_xyz(), fileobj->get_rgb());
	offset = fileobj->get_offset();
	delete fileobj;
	pView->m_scene.add_obj(pointCloud, offset);
	pView->Invalidate(TRUE);
	return 0;
}