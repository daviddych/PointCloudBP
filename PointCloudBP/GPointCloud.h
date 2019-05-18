#pragma once
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <vector>
#include "BoundBox.h"
#include "GraphicalObject.h"

using glm::vec3;
using glm::mat4;

class GPointCloud : public GraphicalObject
{
	using uint = unsigned int;
public:
	GPointCloud(unsigned int N=0, float* xyz = nullptr, float* rgb=nullptr);
	GPointCloud(std::vector<glm::vec3>& xyz, std::vector<glm::vec3>& m_rgb= std::vector<glm::vec3>());
	~GPointCloud();

	void set_intensity(unsigned int N = 0, float* intensity = nullptr);
	void set_return_number(unsigned int N = 0, float* intensity = nullptr);
	void set_number_of_return(unsigned int N = 0, float* intensity = nullptr);
	void set_scan_direction_flag(unsigned int N = 0, float* intensity = nullptr);
	void set_edge_of_flight_line(unsigned int N = 0, float* intensity = nullptr);
	void set_classification(unsigned int N = 0, float* intensity = nullptr);
	void set_scan_angle_rank(unsigned int N = 0, float* intensity = nullptr);
	void set_user_data(unsigned int N = 0, float* intensity = nullptr);
	void set_point_source_id(unsigned int N = 0, float* intensity = nullptr);

	BoundBox* get_box() { return m_box;  }
	std::vector<glm::vec3> get_xyz() { return m_xyz; }
	std::vector<glm::vec3> get_rgb() { return m_rgb; }
	glm::vec3 get_center_xyz() { return _center_xyz; }
	glm::vec3 get_min_xyz() { return _min_xyz;  }
	glm::vec3 get_max_xyz() { return _max_xyz; }

	// 重载基类渲染函数
	void render();
	// 更新指定点的颜色
	void update_color(const std::vector<uint>& idx, const std::vector<glm::vec3>& rgb) override;
	// 查询点数
	virtual unsigned int get_size() override { return m_xyz.size(); }

private:
	void Box() { Box(m_xyz); }
	void Box(std::vector<glm::vec3> xyz);
	void createVAO(std::vector<glm::vec3>& xyz, std::vector<glm::vec3>& rgb);

private:
	float m_offset_xyz[3];
	std::vector<glm::vec3> m_xyz;
	std::vector<glm::vec3> m_rgb;
	std::vector<glm::vec3> _normal_xyz;
	std::vector<float> _intensity;
	std::vector<float> _return_number;
	std::vector<float> _number_of_return;
	std::vector<float> _scan_direction_flag;
	std::vector<float> _edge_of_flight_line;
	std::vector<int> _classification;       // 点云分类
	std::vector<float> _scan_angle_rank;
	std::vector<float> _user_data;
	std::vector<float> _point_source_id;
	
	glm::vec3 _center_xyz;
	glm::vec3 _min_xyz;
	glm::vec3 _max_xyz;
};

