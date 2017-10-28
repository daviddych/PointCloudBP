
// PointCloudBPView.cpp : CPointCloudBPView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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
	// ��׼��ӡ����
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

// CPointCloudBPView ����/����
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

// CPointCloudBPView ����
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

// CPointCloudBPView ��ӡ
BOOL CPointCloudBPView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CPointCloudBPView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CPointCloudBPView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// CPointCloudBPView ���
#ifdef _DEBUG
void CPointCloudBPView::AssertValid() const
{
	CView::AssertValid();
}

void CPointCloudBPView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPointCloudBPDoc* CPointCloudBPView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPointCloudBPDoc)));
	return (CPointCloudBPDoc*)m_pDocument;
}
#endif //_DEBUG


// CPointCloudBPView ��Ϣ�������
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
	// ͬʱ���¡�ctrl + ����������ʰȡ��������Ϣ�� 
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
	// ͬʱ���¡�ctrl + ����Ҽ�������ʾ/����ʰȡ�ĵ� 
	if ((GetKeyState(VK_CONTROL) & 0x8000) && !m_scene.empty())
	{
		m_select_model = !m_select_model;
	}
	CView::OnRButtonDown(nFlags, point);
}
