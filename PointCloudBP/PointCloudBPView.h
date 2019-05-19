
// PointCloudBPView.h : CPointCloudBPView 类的接口
//

// Include the OpenGL headers
//#include "Angel.h"  //Dr. Angel's code does this for us

#pragma once
#include "ScenePointCloud.h"
#include "gPoint.h"
#include "PointCloudBPDoc.h"


class CPointCloudBPView : public CView
{
protected: // 仅从序列化创建
	CPointCloudBPView();
	DECLARE_DYNCREATE(CPointCloudBPView)

// 特性
public:
	CPointCloudBPDoc* GetDocument() const;

// 操作
public:
	// 清空场景
	void ClearScene(); 

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
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

// 生成的消息映射函数
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
	afx_msg void OnRealcolor();
	afx_msg void OnAltitudecolor();
	afx_msg void OnClassifycolor();
	afx_msg void OnIntensitycolor();
	afx_msg void OnReturncolor();
};

#ifndef _DEBUG  // PointCloudBPView.cpp 中的调试版本
inline CPointCloudBPDoc* CPointCloudBPView::GetDocument() const
   { return reinterpret_cast<CPointCloudBPDoc*>(m_pDocument); }
#endif