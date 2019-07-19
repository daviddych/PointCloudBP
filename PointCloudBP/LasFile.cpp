#include "stdafx.h"
#include "LasFile.h"
//#include <liblas/liblas.hpp>
#include <fstream>  
#include <iostream> 
#include "Resource.h"
#include <algorithm>
#include <vector>

LasFile::LasFile()
{
}

LasFile::~LasFile()
{
}

bool LasFile::openfile(const char* filename, char* parse_string)
{
	// start a work thread for loading data from las file.
 	boost::thread t{ LasFile::read, this, filename, parse_string };
 	return true;
}

bool LasFile::read(LasFile* lasf, std::string filename, char* parse_string)
{
	assert(filename.c_str() != NULL);

	// Progress Window
	CProgressStatusDlg *dlg = new CProgressStatusDlg();
	dlg->Create(IDD_PROGRESS_DLG, AfxGetMainWnd());
	dlg->ShowWindow(SW_SHOW);

	lasf->m_lasreadopener.set_file_name(filename.c_str());
	lasf->m_lasreader = lasf->m_lasreadopener.open();
	if (lasf->m_lasreader == 0)
	{
		fprintf(stderr, "ERROR: could not open lasreader\n");
		return false;
	}

	// (maybe) open laswaveform13reader
	LASwaveform13reader* laswaveform13reader = lasf->m_lasreadopener.open_waveform13(&lasf->m_lasreader->header);
	lasf->m_header = &(lasf->m_lasreader->header);

// 	FILE *fpi = fopen(filename.c_str(), "rb");
// 	lasf->m_lasreader = new LASreader();
// 
// 	lasf->m_lasreader->open(fpi);
// 	LASheader  &header = lasf->m_lasreader->header;

	// calculate center of gravity 
	double xc = (lasf->m_header->max_x + lasf->m_header->min_x) * 0.5;
	double yc = (lasf->m_header->max_y + lasf->m_header->min_y) * 0.5;
	double zc = (lasf->m_header->max_z + lasf->m_header->min_z) * 0.5;

	size_t points_count;
	if (lasf->m_header->version_minor >= 4)
	{
#ifdef _WIN32
		printf("%I64d       \012", lasf->m_header->extended_number_of_point_records);
		points_count = lasf->m_header->extended_number_of_point_records;
#else
		points_count = lasf->m_header->extended_number_of_point_records;
		printf("%lld       \012", lasf->m_header->extended_number_of_point_records);
#endif
	}
	else
	{
		points_count = lasf->m_header->number_of_point_records;
		printf("%u       \012", lasf->m_header->number_of_point_records);
	}

	lasf->m_xyz.resize(points_count);
	lasf->m_rgb.resize(points_count);
	std::vector<unsigned short> intensities(points_count, 0);

	// read and convert the points to ASCII
#ifdef _WIN32
	printf("processing %I64d points with '%s'.\n", lasf->m_lasreader->npoints);
#else
	printf("processing %lld points with '%s'.\n", lasf->m_lasreader->npoints);
#endif

	int nstep = floor(points_count / 100.0);
	int k = 1, i;
	unsigned int id = 0;
	while (lasf->m_lasreader->read_point())
	{
		i = 0;
		while (true)
		{
			switch (parse_string[i])
			{
			case 'x': // the x coordinate
				lasf->m_xyz[id].x = lasf->m_lasreader->point.get_x() - xc;
				break;
			case 'y': // the y coordinate
				lasf->m_xyz[id].y = lasf->m_lasreader->point.get_y() - yc;
				break;
			case 'z': // the z coordinate
				lasf->m_xyz[id].z = lasf->m_lasreader->point.get_z() - zc;
				break;
			case 'X': // the unscaled raw integer X coordinate
				lasf->m_xyz[id].x = lasf->m_lasreader->point.get_X();
				break;
			case 'Y': // the unscaled raw integer Y coordinate
				lasf->m_xyz[id].x = lasf->m_lasreader->point.get_Y();
				break;
			case 'Z': // the unscaled raw integer Z coordinate
				lasf->m_xyz[id].x = lasf->m_lasreader->point.get_Z();
				break;
			case 't': // the gps-time
					  //m_lasreader->point.get_gps_time();
				break;
			case 'i': // the intensity
				intensities[id] = lasf->m_lasreader->point.get_intensity();
				break;
			case 'r': // red
				lasf->m_rgb[id].x = lasf->m_lasreader->point.rgb[0];
				break;
			case 'g': // green
				lasf->m_rgb[id].y = lasf->m_lasreader->point.rgb[1];
				break;
			case 'b': // blue
				lasf->m_rgb[id].z = lasf->m_lasreader->point.rgb[2];
				break;
			default:
				i++;
			}
			i++;
			if (parse_string[i] == 0)
				break;
		}
		++id;

		if (id % nstep == 0)
		{
			dlg->StepIt();
		}
	}

	// close the reader
	lasf->m_lasreader->close();
	delete lasf->m_lasreader;

	// 下面是颜色和灰度信息处理
	auto inres = std::minmax_element(intensities.begin(), intensities.end(), [](const unsigned short a, unsigned short &b) { return a < b; });
	unsigned short min_i(*inres.first), max_i(*inres.second);
	if (min_i != max_i)
	{
		glm::vec3 pt;
		std::transform(intensities.begin(), intensities.end(), lasf->m_rgb.begin(), [&pt](unsigned short v) { pt.x = v; pt.y = v; pt.z = v; return pt; });
		//intensities.clear();
	}

	auto result = std::minmax_element(lasf->m_rgb.begin(), lasf->m_rgb.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return a.x < b.x; });
	float min_r((*result.first).x), max_r((*result.second).x);

	result = std::minmax_element(lasf->m_rgb.begin(), lasf->m_rgb.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return a.y < b.y; });
	float min_g((*result.first).y), max_g((*result.second).y);

	result = std::minmax_element(lasf->m_rgb.begin(), lasf->m_rgb.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return a.z < b.z; });
	float min_b((*result.first).z), max_b((*result.second).z);

	float max_v = std::max(max_r, std::max(max_g, max_b));
	if (max_v > 255.0f)
	{
		std::transform(lasf->m_rgb.begin(), lasf->m_rgb.end(), lasf->m_rgb.begin(), [](glm::vec3& v) { return v / 65535.0f; });
	}
	else if (max_v > 1.0f)
	{
		std::transform(lasf->m_rgb.begin(), lasf->m_rgb.end(), lasf->m_rgb.begin(), [](glm::vec3& v) { return v / 255.0f; });
	}

	// (maybe) close the waveform reader
	if (laswaveform13reader)
	{
		laswaveform13reader->close();
		delete laswaveform13reader;
	}

//	if (lasf->m_lasreader->points_have_rgb)
	//{
		//lasf->m_rgb.resize(points_count);
		//for (size_t i = 0; i < points_count; ++i)
		//{
		//	lasf->m_lasreader->read_point();

		//	lasf->m_xyz[i].x = lasf->m_lasreader->point.get_x() * header.x_scale_factor + header.x_offset - xc;
		//	lasf->m_xyz[i].y = lasf->m_lasreader->point.y * header.y_scale_factor + header.y_offset - yc;
		//	lasf->m_xyz[i].z = lasf->m_lasreader->point.z * header.z_scale_factor + header.z_offset - zc;
		//	
		//	lasf->m_rgb[i].r = lasf->m_lasreader->rgb[0] / 65535.0;
		//	lasf->m_rgb[i].g = lasf->m_lasreader->rgb[1] / 65535.0;
		//	lasf->m_rgb[i].b = lasf->m_lasreader->rgb[2] / 65535.0;

		//	if (i % nstep == 0)
		//	{
		//		dlg->StepIt();
		//	}
		//}
	//}
	//else
	//{
		//lasf->m_rgb.clear();
		//for (size_t i = 0; i < points_count; ++i)
		//{
		//	lasf->m_lasreader->read_point();

		//	lasf->m_xyz[i].x = lasf->m_lasreader->point.x * header.x_scale_factor + header.x_offset - xc;
		//	lasf->m_xyz[i].y = lasf->m_lasreader->point.y * header.y_scale_factor + header.y_offset - yc;
		//	lasf->m_xyz[i].z = lasf->m_lasreader->point.z * header.z_scale_factor + header.z_offset - zc;

		//	if (i % nstep == 0)
		//	{
		//		dlg->StepIt();
		//	}
		//}
	//}
	dlg->SetPos(100);
	delete dlg;

	//fclose(fpi);

	lasf->m_offset = glm::vec3(xc, yc, zc);

	lasf->SenCompletedMessage();
	
	return true;
}