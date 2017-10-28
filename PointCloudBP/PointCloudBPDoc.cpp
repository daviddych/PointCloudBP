
// PointCloudBPDoc.cpp : CPointCloudBPDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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


// CPointCloudBPDoc ����/����

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

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}


// CPointCloudBPDoc ���л�

void CPointCloudBPDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CPointCloudBPDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
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

// ������������֧��
void CPointCloudBPDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:  strSearchContent = _T("point;rectangle;circle;ole object;")��
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

// CPointCloudBPDoc ���

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


// CPointCloudBPDoc ����


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
			�����ļ�(*.*)|*.*||"));
	// ����
	fileDialog.m_ofn.lpstrTitle = "�򿪵�������";
	// ָ��Ĭ�϶�λ�ļ���
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