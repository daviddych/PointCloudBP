#pragma once
#include <boost/thread.hpp>
#include <afxstr.h>
#include "PointCloudBPView.h"

class CPointCloudBPView;
class CWnd;

extern boost::mutex mutex;
// void thread_showImage(CVideoAnalysisDlg* pDlg, CWnd* pWnd, char* filename);
// void thread_showImage_process(CVideoAnalysisDlg* pDlg, char* videoFilename);
void thread_open_pointcloud(CPointCloudBPView* pView, CString& filename);