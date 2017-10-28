#include "stdafx.h"
#include "FileObj.h"

FileObj::FileObj()
{
}

FileObj::~FileObj()
{
}

void FileObj::update_offset(glm::vec3 offset)
{
	glm::vec3 oss = m_offset - offset;
#pragma omp parallel for
	for (int i = 0; i < m_xyz.size(); ++i)
	{
		m_xyz[i] += oss;
	}
	m_offset = offset;
}