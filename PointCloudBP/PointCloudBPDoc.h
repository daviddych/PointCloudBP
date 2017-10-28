
// PointCloudBPDoc.h : CPointCloudBPDoc ��Ľӿ�
//


#pragma once
#include "GPointCloud.h"
#include "FileObj.h"


class CPointCloudBPDoc : public CDocument
{
protected: // �������л�����
	CPointCloudBPDoc();
	DECLARE_DYNCREATE(CPointCloudBPDoc)

// ����
public:
	CString m_filename;

// ����
private:

public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CPointCloudBPDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnFileOpen();
	FileObj *m_fileobj;
};
