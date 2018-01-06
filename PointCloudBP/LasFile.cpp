#include "stdafx.h"
#include "LasFile.h"
//#include <liblas/liblas.hpp>
#include <fstream>  
#include <iostream> 
#include "Resource.h"


LasFile::LasFile()
{
}

LasFile::~LasFile()
{
}

bool LasFile::openfile(const char* filename)
{
	// start a work thread for loading data from las file.
 	boost::thread t{ LasFile::read, this, filename };
 	return true;
}

bool LasFile::read(LasFile* lasf, std::string filename)
{
	assert(filename.c_str() != NULL);

	// Progress Window
	CProgressStatusDlg *dlg = new CProgressStatusDlg();
	dlg->Create(IDD_PROGRESS_DLG, AfxGetMainWnd());
	dlg->ShowWindow(SW_SHOW);

	FILE *fpi = fopen(filename.c_str(), "rb");
	lasf->m_lasreader = new LASreader();

	lasf->m_lasreader->open(fpi);
	LASheader  &header = lasf->m_lasreader->header;

	// calculate center of gravity 
	double xc = (header.max_x + header.min_x) * 0.5;
	double yc = (header.max_y + header.min_y) * 0.5;
	double zc = (header.max_z + header.min_z) * 0.5;

	size_t points_count = header.number_of_point_records;
	int nstep = floor(points_count / 100.0);
	lasf->m_xyz.resize(points_count);
	
	if (lasf->m_lasreader->points_have_rgb)
	{
		lasf->m_rgb.resize(points_count);
		for (size_t i = 0; i < points_count; ++i)
		{
			lasf->m_lasreader->read_point();

			lasf->m_xyz[i].x = lasf->m_lasreader->point.x * header.x_scale_factor + header.x_offset - xc;
			lasf->m_xyz[i].y = lasf->m_lasreader->point.y * header.y_scale_factor + header.y_offset - yc;
			lasf->m_xyz[i].z = lasf->m_lasreader->point.z * header.z_scale_factor + header.z_offset - zc;
			
			lasf->m_rgb[i].r = lasf->m_lasreader->rgb[0] / 65535.0;
			lasf->m_rgb[i].g = lasf->m_lasreader->rgb[1] / 65535.0;
			lasf->m_rgb[i].b = lasf->m_lasreader->rgb[2] / 65535.0;

			if (i % nstep == 0)
			{
				dlg->StepIt();
			}
		}
	}
	else
	{
		lasf->m_rgb.clear();
		for (size_t i = 0; i < points_count; ++i)
		{
			lasf->m_lasreader->read_point();

			lasf->m_xyz[i].x = lasf->m_lasreader->point.x * header.x_scale_factor + header.x_offset - xc;
			lasf->m_xyz[i].y = lasf->m_lasreader->point.y * header.y_scale_factor + header.y_offset - yc;
			lasf->m_xyz[i].z = lasf->m_lasreader->point.z * header.z_scale_factor + header.z_offset - zc;

			if (i % nstep == 0)
			{
				dlg->StepIt();
			}
		}
	}
	dlg->SetPos(100);
	delete dlg;

	fclose(fpi);

	lasf->m_offset = glm::vec3(xc, yc, zc);

	lasf->SenCompletedMessage();
	
	return true;
}