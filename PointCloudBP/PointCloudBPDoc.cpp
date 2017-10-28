
// PointCloudBPDoc.cpp : CPointCloudBPDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "PointCloudBP.h"
#endif

#include "PointCloudBPDoc.h"

#include <propkey.h>
#include "TxtFile.h"
#include "PointCloudBPView.h"
#include "LasFile.h"
//#include <boost/thread.hpp>
#include "threadfunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPointCloudBPDoc

IMPLEMENT_DYNCREATE(CPointCloudBPDoc, CDocument)

BEGIN_MESSAGE_MAP(CPointCloudBPDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CPointCloudBPDoc::OnFileOpen)
END_MESSAGE_MAP()


// CPointCloudBPDoc 构造/析构

CPointCloudBPDoc::CPointCloudBPDoc()
{
}

CPointCloudBPDoc::~CPointCloudBPDoc()
{
}

BOOL CPointCloudBPDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}


// CPointCloudBPDoc 序列化

void CPointCloudBPDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CPointCloudBPDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CPointCloudBPDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:  strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CPointCloudBPDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CPointCloudBPDoc 诊断

#ifdef _DEBUG
void CPointCloudBPDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPointCloudBPDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPointCloudBPDoc 命令


void CPointCloudBPDoc::OnFileOpen()
{
	CFileDialog fileDialog(TRUE, ".las", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("LAS_MODEL(*.las)|*.las|\
			TXT_MODEL(*.txt)|*.txt|\
			Surfit_Dat(*.dat)|*.dat|\
			xyzi_MODEL(*.xyzi)|*.xyzi|\
			point_line(*.pl)|*.pl|\
			CSV_MODEL(*.csv)|*.csv|\
			所有文件(*.*)|*.*||"));
	// 标题
	fileDialog.m_ofn.lpstrTitle = "打开点云数据";
	// 指定默认定位文件夹
	fileDialog.m_ofn.lpstrInitialDir = "d:\\"; 

	if (fileDialog.DoModal() != IDOK) 	return;

	CString FilePathName = fileDialog.GetPathName();
	m_filename = FilePathName;
	
	CString ext(FilePathName.Right(3));
	ext.MakeLower();
	char* msg = (LPTSTR)(LPCTSTR)FilePathName;
	msg = FilePathName.GetBuffer(0);
	FilePathName.ReleaseBuffer();
	GPointCloud *pointCloud;
	glm::vec3 offset;

	FileObj *fileobj;
	if (ext == "txt")
	{
		fileobj = new CTxtFile;
	}
	else if (ext == "las")
	{
		fileobj = new LasFile;
	}
	// waiting to add more file format support.

	fileobj->openfile(m_filename);

	POSITION pos = GetFirstViewPosition();
	CPointCloudBPView* pView = (CPointCloudBPView*)GetNextView(pos);
	boost::thread t2{ thread_open_pointcloud, pView, m_filename };
	if (!pView->m_scene.empty())
	{
		offset = pView->m_scene.get_scene_offset();
		fileobj->update_offset(offset);
	}
	pointCloud = new GPointCloud(fileobj->get_xyz(), fileobj->get_rgb());
	offset = fileobj->get_offset();
	delete fileobj;
	pView->m_scene.add_obj(pointCloud, offset);
	pView->Invalidate(TRUE);
}