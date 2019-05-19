#pragma once
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

class BoundBox
{
	using  vec3 = glm::vec3;

public:
	BoundBox(const vec3& lb=vec3(), const vec3& ru=vec3());
	BoundBox(const BoundBox* box);

	

	~BoundBox();

	void release();

	// 包围盒求并集
	BoundBox operator+(const BoundBox& b);
	BoundBox* plus(const vec3& lb = vec3(), const vec3& ru = vec3());
	void assign_coords(const vec3& lb, const vec3& ru);

	void render();
	void update(const BoundBox* box);

	vec3 _leftbottom{ 0.0f, 0.0f, 0.0f };
	vec3 _rightup{ 0.0f, 0.0f, 0.0f };
	vec3 _center{0.0f, 0.0f, 0.0f};
	float _x_length{ 0.0f }, _y_length{ 0.0f }, _z_length{ 0.0f };
	 
private:
	void init_graphic();
	void assign_coords(const BoundBox* box); // 坐标赋值
	

	unsigned int _vbo[2];
	unsigned int _vaoHandle;
	static GLubyte s_indices[24];
};