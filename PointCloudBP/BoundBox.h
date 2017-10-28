#pragma once
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

class BoundBox
{
	typedef  glm::vec3  vec3;

public:
	BoundBox(vec3 lb=vec3(), vec3 ru=vec3());
	BoundBox(BoundBox* box);
	~BoundBox();

	void release();

	// 包围盒求并集
	BoundBox operator+(BoundBox b);
	BoundBox* Union(vec3 lb = vec3(), vec3 ru = vec3());
	
	void render();

	vec3 _leftbottom;
	vec3 _rightup;
	vec3 _center;
	float _x_length, _y_length, _z_length;

private:
	void init_graphic();

	unsigned int _vbo[2];
	unsigned int _vaoHandle;
	static GLubyte s_indices[24];
};



