#include "stdafx.h"
#include "BoundBox.h"
#include <algorithm>
#include "gl\GL.H"
#include <glm/gtc/matrix_transform.hpp>

GLubyte BoundBox::s_indices[24] = { 0,1,1,2,2,3,3,0,5,6,6,7,7,4,4,5,6,1,7,2,4,3,5,0 };

BoundBox::BoundBox(const vec3& lb , const vec3& ru)
{
	_vaoHandle = 0;
	_leftbottom = lb;
	_rightup = ru;
	_center = (_leftbottom + _rightup) / 2.0f;

	glm::vec3 pt = _rightup - _leftbottom;

	_x_length = pt.x;
	_y_length = pt.y;
	_z_length = pt.z;

	init_graphic();
}

BoundBox::BoundBox(const BoundBox* box)
{
	assign_coords(box);

	_vaoHandle = 0;

	init_graphic();
}

void BoundBox::assign_coords(const BoundBox* box)
{
	if (box == nullptr)
		return;

	_leftbottom = box->_leftbottom;
	_rightup = box->_rightup;
	_center = (_leftbottom + _rightup) / 2.0f;

	glm::vec3 pt = _rightup - _leftbottom;

	_x_length = pt.x;
	_y_length = pt.y;
	_z_length = pt.z;
}

BoundBox::~BoundBox()
{
	release();
}

void BoundBox::release()
{
	glDeleteBuffers(2, _vbo);
	glDeleteVertexArrays(1, &_vaoHandle);
	_vaoHandle = 0;
	_leftbottom = vec3(0.0f, 0.0f, 0.0f);
	_rightup = vec3(0.0f, 0.0f, 0.0f);
	_center = vec3(0.0f, 0.0f, 0.0f);
	_x_length = 0.0f;
	_y_length = 0.0f;
	_z_length = 0.0f;
}

BoundBox BoundBox::operator+(const BoundBox& b)
{ 
	vec3 lb(std::min(_leftbottom.x, b._leftbottom.x), std::min(_leftbottom.y, b._leftbottom.y), std::min(_leftbottom.z, b._leftbottom.z));
	vec3 ru(std::max(_rightup.x, b._rightup.x), std::max(_rightup.y, b._rightup.y), std::max(_rightup.z, b._rightup.z));
	return std::move(BoundBox(lb, ru)); 
}

BoundBox* BoundBox::plus(const vec3& lb, const vec3& ru)
{
	_leftbottom = vec3(std::min(_leftbottom.x, lb.x), std::min(_leftbottom.y, lb.y), std::min(_leftbottom.z, lb.z));
	_rightup = vec3(std::max(_rightup.x, ru.x), std::max(_rightup.y, ru.y), std::max(_rightup.z, ru.z));
	_center = (_leftbottom + _rightup) / 2.0f;

	glm::vec3 pt = _rightup - _leftbottom;

	_x_length = pt.x;
	_y_length = pt.y;
	_z_length = pt.z;

	glDeleteBuffers(2, _vbo);
	glDeleteVertexArrays(1, &_vaoHandle);
	_vaoHandle = 0;
	init_graphic();
	return this;
}

void BoundBox::update(const BoundBox* box)
{
	assign_coords(box);

	float x1(_leftbottom.x), y1(_leftbottom.y), z1(_leftbottom.z);
	float x2(_rightup.x), y2(_rightup.y), z2(_rightup.z);

	// vertex coords array
	float vertices[] = { x2,y1,z2,  x1,y1,z2,   x1,y1,z1,    x2,y1,z1,    // v0-v1-v2-v3
		x2,y2,z1,    x2,y2,z2,   x1,y2,z2,   x1,y2,z1 };  // v4-v5-v6-v7

	glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_DYNAMIC_DRAW);
}
void BoundBox::init_graphic()
{
// cube ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

float x1(_leftbottom.x), y1(_leftbottom.y), z1(_leftbottom.z);
float x2(_rightup.x), y2(_rightup.y), z2(_rightup.z);

// vertex coords array
float vertices[] = { x2,y1,z2,  x1,y1,z2,   x1,y1,z1,    x2,y1,z1,    // v0-v1-v2-v3
					 x2,y2,z1,    x2,y2,z2,   x1,y2,z2,   x1,y2,z1};  // v4-v5-v6-v7

// normal array
float normals[] = { 0,0,1,  0,0,1,  0,0,1,  0,0,1,             // v0-v1-v2-v3
					0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1 };        // v4-v5-v6-v7

// color array
float colors[] = {  1,1,1,  1,1,0,  1,0,0,  1,0,1,              // v0-v1-v2-v3
				0,0,1,  0,1,1,  0,1,0,  1,1,1 };             // v4-v5-v6-v7


// index array of vertex array for glDrawElements()
// Notice the indices are listed straight from beginning to end as exactly
// same order of vertex array without hopping, because of different normals at
// a shared vertex. For this case, glDrawArrays() and glDrawElements() have no
// difference.

glGenBuffers(2, _vbo);
glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_DYNAMIC_DRAW);

glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), colors, GL_STATIC_DRAW);

// Allocate Vertex Array Object
glGenVertexArrays(1, &_vaoHandle);
glBindVertexArray(_vaoHandle);

glEnableVertexAttribArray(0);  // Vertex position
glEnableVertexAttribArray(1);  // Vertex color

glBindVertexBuffer(0, _vbo[0], 0, sizeof(GLfloat) * 3);
glBindVertexBuffer(1, _vbo[1], 0, sizeof(GLfloat) * 3);

glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0); // glVertexAttribPointer
glVertexAttribBinding(0, 0);

glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
glVertexAttribBinding(1, 1);

glBindVertexArray(0);
}

void BoundBox::render()
{
	if (_vaoHandle == 0) return;

	glBindVertexArray(_vaoHandle);
	glLineWidth(1);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_BYTE, s_indices);
	glBindVertexArray(0);
}