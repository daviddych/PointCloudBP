
// PointCloudBPView.cpp : CPointCloudBPView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "PointCloudBP.h"
#endif

#include "PointCloudBPDoc.h"
#include "PointCloudBPView.h"
#include "GL/glew.h"
#include "GL/wglew.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPointCloudBPView

IMPLEMENT_DYNCREATE(CPointCloudBPView, CView)

BEGIN_MESSAGE_MAP(CPointCloudBPView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

//// You will add stuff here!!!!
//const char* const CPointCloudBPView::_ErrorStrings[]= {
//	{"No Error"},                     // 0
//	{"Unable to get a DC"},           // 1
//	{"ChoosePixelFormat failed"},     // 2
//	{"SelectPixelFormat failed"},     // 3
//	{"wglCreateContext failed"},      // 4
//	{"wglMakeCurrent failed"},        // 5
//	{"wglDeleteContext failed"},      // 6
//	{"SwapBuffers failed"},           // 7
//};

// CPointCloudBPView 构造/析构
CPointCloudBPView::CPointCloudBPView() : m_select_model(false), m_gpoint_ptr(nullptr)
{
}

CPointCloudBPView::~CPointCloudBPView()
{
	if (m_gpoint_ptr != nullptr)
		delete m_gpoint_ptr;
}

BOOL CPointCloudBPView::PreCreateWindow(CREATESTRUCT& cs)
{
	// An OpenGL window must be created with the following flags and must not include CS_PARENTDC for the class style.
	cs.style |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	//cs.lpszClass = AfxRegisterWndClass(CS_OWNDC | CS_HREDRAW | CS_VREDRAW);

	return CView::PreCreateWindow(cs);
}

// CPointCloudBPView 绘制
void CPointCloudBPView::OnDraw(CDC* pDC)
{
	CPointCloudBPDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_scene.render();

	if (m_select_model)
	{
		m_scene.getCurProgram()->setUniform("circle_flag", 1.0f);
		m_gpoint_ptr->render();
		m_scene.getCurProgram()->setUniform("circle_flag", 0.0f);
	}

	m_scene.SwapBuffers();
}

// CPointCloudBPView 打印
BOOL CPointCloudBPView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CPointCloudBPView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CPointCloudBPView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CPointCloudBPView 诊断
#ifdef _DEBUG
void CPointCloudBPView::AssertValid() const
{
	CView::AssertValid();
}

void CPointCloudBPView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPointCloudBPDoc* CPointCloudBPView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPointCloudBPDoc)));
	return (CPointCloudBPDoc*)m_pDocument;
}
#endif //_DEBUG


// CPointCloudBPView 消息处理程序
int CPointCloudBPView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Can we put this in the constructor?
	CDC* pDC = new CClientDC(this);
	if (NULL == pDC) 
	{
		AfxMessageBox(CString("Unable to get a DC"));
		return FALSE;
	}
		
	m_scene.sceneInit(pDC);

	return 0;
}

void CPointCloudBPView::OnDestroy()
{
	CView::OnDestroy();

	m_scene.releaseRes();
}

void CPointCloudBPView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	m_scene.resize(cx, cy);
}

BOOL CPointCloudBPView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CPointCloudBPView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (!m_scene.empty())
	{
		m_scene.zoom(int(zDelta / 30));
		Invalidate(TRUE);
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CPointCloudBPView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'w' || nChar == 'W')
	{
		m_scene.rotate(1,0);
	}
	else if (nChar == 'x' || nChar == 'X')
	{
		m_scene.rotate(-1,0);
	}
	else if (nChar == 'a' || nChar == 'A')
	{
		m_scene.rotate(0, 1);
	}
	else if (nChar == 'd' || nChar == 'D')
	{
		m_scene.rotate(0, -1);
	}
	else if (nChar == 'r' || nChar == 'R')
	{
		m_scene.reset();
	}
	Invalidate(TRUE);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CPointCloudBPView::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt = point - m_mousePt;
	if (nFlags == MK_LBUTTON )
	{
		m_scene.rotate(float(pt.x), float(pt.y));
	}
	else if (nFlags == MK_RBUTTON)
	{
	//	m_scene.zoom(pt.y>0?1:-1);
		m_scene.translate(pt.x, pt.y);
	}
	m_mousePt = point;
	Invalidate(TRUE);

	CView::OnMouseMove(nFlags, point);
}

void CPointCloudBPView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_mousePt = point;
	// 同时按下“ctrl + 鼠标左键”，拾取点坐标信息。 
	if ((nFlags & MK_CONTROL) && !m_scene.empty())
	{
		if (m_scene.pickpoint(point.x, point.y, m_picked_pt))
		{
			m_select_model = true;
			if (m_gpoint_ptr == nullptr)
			{
				m_gpoint_ptr = new GPoint(m_picked_pt);
			}
			else
			{
				m_gpoint_ptr->Update(m_picked_pt);
			}
			Invalidate(TRUE);
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

void CPointCloudBPView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// 同时按下“ctrl + 鼠标右键”，显示/隐藏拾取的点 
	if ((GetKeyState(VK_CONTROL) & 0x8000) && !m_scene.empty())
	{
		m_select_model = !m_select_model;
	}
	CView::OnRButtonDown(nFlags, point);
}
