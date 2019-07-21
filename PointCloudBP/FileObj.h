#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "ProgressStatusDlg.h"
#include <boost/thread.hpp>

class FileObj
{
public:
	FileObj();
	virtual ~FileObj();

	virtual bool openfile(const char* filename, char* parse_string = "xyzirgb")=0;
	std::vector<glm::vec3> get_xyz() { return m_xyz; }
	std::vector<glm::vec3> get_rgb() { return m_rgb; }
	glm::vec3 get_offset() { return m_offset; }
	void update_offset(glm::vec3 offset);// reset data using a global offset.
	static void normalize(std::vector<glm::vec3>& xyz, glm::vec3 offset); // 对xyz每一个元素偏移.
	static glm::vec3 center(std::vector<glm::vec3>& xyz);    // calculate the center point

	void release();
public:
	static CProgressCtrl*  m_s_dlg;
	std::vector<glm::vec3> m_xyz;
	std::vector<glm::vec3> m_rgb;
	glm::vec3              m_offset;     // the center of object coordinates.

protected:
	void SenCompletedMessage()
	{
		CWnd* pWnd = CWnd::FindWindow(NULL, _T("PointCloudBP"));
		if (pWnd == NULL)
		{
			AfxMessageBox("接收程序没有运行!");
			return;
		}
		pWnd->SendMessage(WM_LOADFILE_COMPLETE_MSG, NULL, 0);
	}
};