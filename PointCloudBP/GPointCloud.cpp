#include "stdafx.h"
#include "GPointCloud.h"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <assert.h>
#include <algorithm>

using glm::vec3;

GPointCloud::GPointCloud(std::vector<glm::vec3>& xyz, std::vector<glm::vec3>& rgb):GraphicalObject(nullptr)
{
	if (!xyz.empty())
	{
		m_xyz.resize(xyz.size());
		std::copy(xyz.begin(), xyz.end(), m_xyz.begin());
		Box(xyz);

		if (!rgb.empty())
		{
			assert(xyz.size() == rgb.size());
			m_rgb.resize(rgb.size());
			std::copy(rgb.begin(), rgb.end(), m_rgb.begin());
		}
		else
		{
			m_rgb.resize(xyz.size(),glm::vec3(1,1,1));
		}

		// Create and populate the buffer objects
		createVAO(m_xyz, m_rgb);
	}
}

GPointCloud::GPointCloud(unsigned int N, float* xyz, float* rgb) : GraphicalObject(nullptr)
{
	assert(N > 0);

	m_xyz.resize(N);
	memcpy(&m_xyz[0].x, xyz, sizeof(float)*N * 3);
	Box(m_xyz);

	if (rgb != nullptr)
	{
		m_rgb.resize(N);
		float* ptr = rgb;
		for (unsigned int i = 0; i < N; ++i)
		{
			m_rgb[i].r = *ptr++;
			m_rgb[i].g = *ptr++;
			m_rgb[i].b = *ptr++;
		}
	}
	else
	{
		m_rgb.resize(N, glm::vec3(1.0f, 1.0f, 1.0f));
	}

	// Create and populate the buffer objects
	createVAO(m_xyz, m_rgb);
}

void GPointCloud::createVAO(std::vector<glm::vec3>& xyz, std::vector<glm::vec3>& rgb)
{
	glGenBuffers(2, m_vboHandles);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[0]);
	glBufferData(GL_ARRAY_BUFFER, xyz.size() * 3 * sizeof(float), &xyz[0].x, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[1]);
	glBufferData(GL_ARRAY_BUFFER, rgb.size() * 3 * sizeof(float), &rgb[0].r, GL_DYNAMIC_DRAW);

	// Allocate Vertex Array Object
	glGenVertexArrays(1, &m_vaoHandle);
	glBindVertexArray(m_vaoHandle);

	glEnableVertexAttribArray(0);  // Vertex position
	glEnableVertexAttribArray(1);  // Vertex color

	glBindVertexBuffer(0, m_vboHandles[0], 0, sizeof(GLfloat) * 3);
	glBindVertexBuffer(1, m_vboHandles[1], 0, sizeof(GLfloat) * 3);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);

	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);

	glBindVertexArray(0);
}

// calculate the range box of the input points
void GPointCloud::Box(std::vector<glm::vec3> xyz)
{
//	glm::vec3 leftbottom,rightup;
//	float x_min, x_max, y_min, z_min, y_max, z_max;
// 	[&xyz, &leftbottom, &rightup]() {
// 		std::sort(xyz.begin(), xyz.end(), [](const vec3& d1, const vec3& d2) { return d1.x > d2.x; });
// 		rightup.x = xyz[0].x;
// 		leftbottom.x = xyz[xyz.size() - 1].x;
// 	};
// 
// 	[&xyz, &leftbottom, &rightup]() {
// 		std::sort(xyz.begin(), xyz.end(), [](const vec3& d1, const vec3& d2) { return d1.y > d2.y; });
// 		rightup.y = xyz[0].y;
// 		leftbottom.y = xyz[xyz.size() - 1].y;
// 	};
// 
// 	[&xyz, &leftbottom, &rightup]() {
// 		std::sort(xyz.begin(), xyz.end(), [](const vec3& d1, const vec3& d2) { return d1.z > d2.z; });
// 		rightup.z = xyz[0].z;
// 		leftbottom.z = xyz[xyz.size() - 1].z;
// 	};

	float x_min = std::numeric_limits<float>::max();
	float x_max = -x_min;
	float y_min(x_min), z_min(x_min), y_max(x_max), z_max(x_max);
	for (auto iter = xyz.begin(); iter != xyz.end(); ++iter)
	{
		x_min = x_min < iter->x ? x_min : iter->x;
		x_max = x_max > iter->x ? x_max : iter->x;
		y_min = y_min < iter->y ? y_min : iter->y;
		y_max = y_max > iter->y ? y_max : iter->y;
		z_min = z_min < iter->z ? z_min : iter->z;
		z_max = z_max > iter->z ? z_max : iter->z;
	}
	vec3 leftbottom(x_min, y_min, z_min);
	vec3 rightup(x_max, y_max, z_max);
	
	if( m_box != nullptr)
		delete m_box;

	m_box = new BoundBox(leftbottom, rightup);
}

void GPointCloud::render()
{
	if (m_vaoHandle==0) return;
	//_cprintf("GPointCloud vao = %d\n", _vaoHandle);
	//GLuint programHandle = prog.getHandle();
	//GLuint location = glGetUniformLocation(programHandle, "RotationMatrix");
	//glUniformMatrix4fv(location, 1, GL_FALSE, &rotationMatrix[0][0]);

	glBindVertexArray(m_vaoHandle);
	glDrawArrays(GL_POINTS, 0, m_xyz.size()); 
//	glVertexAttrib3f((GLuint)1, 1.0f, 0.0f, 0.0f);
	glBindVertexArray(0);

	//m_box->render();
}

GPointCloud::~GPointCloud()
{
	if(!m_xyz.empty())
		m_xyz.clear();

	if (!m_rgb.empty())
		m_rgb.clear();
	
	if (!_intensity.empty())
		_intensity.clear();

	if (!_return_number.empty())
		_return_number.clear();

	if (!_number_of_return.empty())
		_number_of_return.clear();
	
	
	if (!_scan_direction_flag.empty())
		_scan_direction_flag.clear();
	
	if (!_edge_of_flight_line.empty())
		_edge_of_flight_line.clear();
	
	if (!_classification.empty())
		_classification.clear();
	
	if (!_scan_angle_rank.empty())
		_scan_angle_rank.clear();
	
	if (!_user_data.empty())
		_user_data.clear();
	
	if (!_point_source_id.empty())
		_point_source_id.clear();
}

void GPointCloud::update_color(const std::vector<uint>& idx, const std::vector<glm::vec3>& rgb) 
{
	assert(idx.size() == rgb.size() && rgb.size() <= m_rgb.size());

	// 传入空向量，表示直接重置场景颜色
	if (rgb.empty() && !m_rgb.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[1]);
		glBufferData(GL_ARRAY_BUFFER, m_rgb.size() * 3 * sizeof(float), &m_rgb[0].r, GL_DYNAMIC_DRAW);
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[1]);
	float* ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	// if the pointer is valid(mapped), update VBO
	if (ptr)
	{	// modify buffer data
		for (auto id : idx)
		{
			memcpy(ptr + id * 3, &rgb[id].r, 3 * sizeof(float));
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);    // unmap it after use
	}
}

void GPointCloud::set_intensity(unsigned int N/* = 0*/, float* intensity /*= nullptr*/)
{
	if (N > 0)
	{
		float* ptr = intensity;
		for (unsigned int i = 0; i < N; ++i)
		{
			_intensity[i] = *ptr++;
		}
	}
}

void GPointCloud::set_return_number(unsigned int N/* = 0*/, float* return_number/* = nullptr*/)
{
	if (N > 0)
	{
		float* ptr = return_number;
		for (unsigned int i = 0; i < N; ++i)
		{
			_return_number[i] = *ptr++;
		}
	}
}

void GPointCloud::set_number_of_return(unsigned int N/* = 0*/, float* number_of_return/* = nullptr*/)
{
	if (N > 0)
	{
		float* ptr = number_of_return;
		for (unsigned int i = 0; i < N; ++i)
		{
			_number_of_return[i] = *ptr++;
		}
	}
}

void GPointCloud::set_scan_direction_flag(unsigned int N/* = 0*/, float* scan_direction_flag/* = nullptr*/)
{
	if (N > 0)
	{
		float* ptr = scan_direction_flag;
		for (unsigned int i = 0; i < N; ++i)
		{
			_scan_direction_flag[i] = *ptr++;
		}
	}
}

void GPointCloud::set_edge_of_flight_line(unsigned int N/* = 0*/, float* edge_of_flight_line/* = nullptr*/)
{
	if (N > 0)
	{
		float* ptr = edge_of_flight_line;
		for (unsigned int i = 0; i < N; ++i)
		{
			_edge_of_flight_line[i] = *ptr++;
		}
	}
}

void GPointCloud::set_classification(unsigned int N/* = 0*/, float* classification/* = nullptr*/)
{
	if (N > 0)
	{
		float* ptr = classification;
		for (unsigned int i = 0; i < N; ++i)
		{
			_classification[i] = *ptr++;
		}
	}
}

void GPointCloud::set_scan_angle_rank(unsigned int N/* = 0*/, float* scan_angle_rank/* = nullptr*/)
{
	if (N > 0)
	{
		float* ptr = scan_angle_rank;
		for (unsigned int i = 0; i < N; ++i)
		{
			_scan_angle_rank[i] = *ptr++;
		}
	}
}

void GPointCloud::set_user_data(unsigned int N/* = 0*/, float* user_data/* = nullptr*/)
{
	if (N > 0)
	{
		float* ptr = user_data;
		for (unsigned int i = 0; i < N; ++i)
		{
			_user_data[i] = *ptr++;
		}
	}
}

void GPointCloud::set_point_source_id(unsigned int N/* = 0*/, float* point_source_id/* = nullptr*/)
{
	if (N > 0)
	{
		float* ptr = point_source_id;
		for (unsigned int i = 0; i < N; ++i)
		{
			_point_source_id[i] = *ptr++;
		}
	}
}

std::vector<unsigned int> GPointCloud::get_point_source_id() 
{
	if (m_xyz.size() != _point_source_id.size())
	{
		_point_source_id.resize(m_xyz.size());
		for (size_t i = 0; i < m_xyz.size(); ++i)
			_point_source_id[i] = i;

	}
	return _point_source_id; 
}

std::vector<float> GPointCloud::get_intensity() { 
	if (_intensity.empty())
	{
		_intensity.resize(m_xyz.size(), 0.0f);
		// _intensity = R*0.299 + G*0.587 + B*0.114
#pragma omp parallel for
		for (int i = 0; i < m_rgb.size(); ++i)
		{
			_intensity[i] = m_rgb[i].r*0.299f + m_rgb[i].g*0.587f + m_rgb[i].b*0.114f;
		}
	}
	return _intensity; 
}

std::vector<float> GPointCloud::get_number_of_return() {
	if (_number_of_return.empty())
		_number_of_return.resize(m_xyz.size(), 0.0f);
	return _number_of_return; 
}

std::vector<float>   GPointCloud::get_classification() 
{ 
	if (_classification.empty())
		_classification.resize(m_xyz.size(), 0.0f);
	return _classification; 
}

std::vector<float>  GPointCloud::get_altitudes()
{
	std::vector<float> altis(m_xyz.size());
	for (size_t i = 0; i < m_xyz.size(); ++i)
	{
		altis[i] = m_xyz[i].z;
	}

	return std::move(altis);
}