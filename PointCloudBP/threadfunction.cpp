#include "stdafx.h"
#include "threadfunction.h"
#include "Resource.h"
#include "GPointCloud.h"
#include "glm/glm.hpp"
#include "FileObj.h"
#include <string>
#include "TxtFile.h"
#include "LasFile.h"
#include "PointCloudBPView.h"

void thread_open_pointcloud(CPointCloudBPView* pView, CString& filename)
{
	GPointCloud *pointCloud;
	glm::vec3 offset;

	CString ext(filename.Right(3));
	
	FileObj *fileobj;
	if (ext == "txt")
	{
		fileobj = new CTxtFile;
	}
	else if (ext == "las")
	{
		fileobj = new LasFile;
	}
	fileobj->openfile(filename);

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