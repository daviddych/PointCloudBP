#include "stdafx.h"
#include "LasFile.h"
//#include <liblas/liblas.hpp>
#include <fstream>  // std::ifstream
#include <iostream> // std::cout


LasFile::LasFile()
{
}

LasFile::~LasFile()
{
}

bool LasFile::openfile(const char* filename)
{
	return read(filename);
}

bool LasFile::read(std::string filename)
{
	assert(filename.c_str() != NULL);

	FILE *fpi = fopen(filename.c_str(), "rb");
	m_lasreader = new LASreader();

	m_lasreader->open(fpi);
	header = &m_lasreader->header;

	// calculate center of gravity 
	double m_X_Center = (header->max_x + header->min_x) * 0.5;
	double m_Y_Center = (header->max_y + header->min_y) * 0.5;
	double m_Z_Center = (header->max_z + header->min_z) * 0.5;

	size_t points_count = header->number_of_point_records;
	m_xyz.resize(points_count);
	
	if (m_lasreader->points_have_rgb)
	{
		m_rgb.resize(points_count);
		for (size_t i = 0; i < points_count; ++i)
		{
			m_lasreader->read_point();

			//m_lasreader->get_coordinates(&m_xyz[i].x);
			//m_xyz[i].x = m_lasreader->point.x*m_lasreader->header.x_scale_factor + m_lasreader->header.x_offset;
			//m_xyz[i].y = m_lasreader->point.y*m_lasreader->header.y_scale_factor + m_lasreader->header.y_offset;
			//m_xyz[i].z = m_lasreader->point.z*m_lasreader->header.z_scale_factor + m_lasreader->header.z_offset;
			m_xyz[i].x = m_lasreader->point.x *m_lasreader->header.x_scale_factor + m_lasreader->header.x_offset - m_X_Center;
			m_xyz[i].y = m_lasreader->point.y *m_lasreader->header.y_scale_factor + m_lasreader->header.y_offset - m_Y_Center;
			m_xyz[i].z = m_lasreader->point.z *m_lasreader->header.z_scale_factor + m_lasreader->header.z_offset - m_Z_Center;

			m_rgb[i].r = m_lasreader->rgb[0] / 65535.0;
			m_rgb[i].g = m_lasreader->rgb[1] / 65535.0;
			m_rgb[i].b = m_lasreader->rgb[2] / 65535.0;
		}
	}
	else
	{
		m_rgb.clear();
		for (size_t i = 0; i < points_count; ++i)
		{
			m_lasreader->read_point();

			//m_xyz[i].x = m_lasreader->point.x*m_lasreader->header.x_scale_factor + m_lasreader->header.x_offset;
			//m_xyz[i].y = m_lasreader->point.y*m_lasreader->header.y_scale_factor + m_lasreader->header.y_offset;
			//m_xyz[i].z = m_lasreader->point.z*m_lasreader->header.z_scale_factor + m_lasreader->header.z_offset;
			m_xyz[i].x = m_lasreader->point.x *m_lasreader->header.x_scale_factor + m_lasreader->header.x_offset - m_X_Center;
			m_xyz[i].y = m_lasreader->point.y *m_lasreader->header.y_scale_factor + m_lasreader->header.y_offset - m_Y_Center;
			m_xyz[i].z = m_lasreader->point.z *m_lasreader->header.z_scale_factor + m_lasreader->header.z_offset - m_Z_Center;
		}
	}

	fclose(fpi);

	m_offset = glm::vec3(m_X_Center, m_Y_Center, m_Z_Center);
	return true;
}