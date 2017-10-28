
// PointCloudBPView.h : CPointCloudBPView ��Ľӿ�
//

// Include the OpenGL headers
//#include "Angel.h"  //Dr. Angel's code does this for us

#pragma once
#include "ScenePointCloud.h"
#include "gPoint.h"
#include "PointCloudBPDoc.h"


class CPointCloudBPView : public CView
{
protected: // �������л�����
	CPointCloudBPView();
	DECLARE_DYNCREATE(CPointCloudBPView)

// ����
public:
	CPointCloudBPDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CPointCloudBPView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	ScenePointCloud m_scene;
private:
	CPoint    m_mousePt;
	bool      m_select_model;
	glm::vec3 m_picked_pt;
	GPoint   *m_gpoint_ptr;
	//Error Handling
	//void SetError(int e);
	//static const char* const _ErrorStrings[];

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:

public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // PointCloudBPView.cpp �еĵ��԰汾
inline CPointCloudBPDoc* CPointCloudBPView::GetDocument() const
   { return reinterpret_cast<CPointCloudBPDoc*>(m_pDocument); }
#endif