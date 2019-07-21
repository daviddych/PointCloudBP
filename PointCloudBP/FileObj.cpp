#include "stdafx.h"
#include "FileObj.h"

FileObj::FileObj()
{
}

FileObj::~FileObj()
{
	release();
}

void FileObj::release()
{
	if (!m_xyz.empty())
	{
		m_xyz.clear();
	}

	if (m_rgb.empty())
	{
		m_rgb.clear();
	}
}

void FileObj::update_offset(glm::vec3 offset)
{
	glm::vec3 oss = m_offset - offset;
#pragma omp parallel for
	for (int i = 0; i < m_xyz.size(); ++i)
	{
		m_xyz[i] += oss;
	}
	//std::transform(m_xyz.begin(), m_xyz.end(), m_xyz.begin(), [oss](const glm::vec3& v) { return v + oss; });

	m_offset = offset;
}


void FileObj::normalize(std::vector<glm::vec3>& xyz, glm::vec3 offset)
{
	std::transform(xyz.begin(), xyz.end(), xyz.begin(), [offset](const glm::vec3& v) { return v - offset; });
}

glm::vec3 FileObj::center(std::vector<glm::vec3>& xyz)
{
	auto result = std::minmax_element(xyz.begin(), xyz.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return a.x < b.x; });
	float min_x((*result.first).x), max_x((*result.second).x);

	result = std::minmax_element(xyz.begin(), xyz.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return a.y < b.y; });
	float min_y((*result.first).y), max_y((*result.second).y);

	result = std::minmax_element(xyz.begin(), xyz.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return a.z < b.z; });
	float min_z((*result.first).z), max_z((*result.second).z);

	return glm::vec3(max_x + min_x, max_y + min_y, max_z + min_z) / 2.0f;
}