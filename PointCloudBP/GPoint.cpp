#include "stdafx.h"
#include "GPoint.h"


GPoint::GPoint(glm::vec3 pt, glm::vec3 rgb)
{
	createVAO(pt, rgb);
}

GPoint::~GPoint()
{
}

void GPoint::createVAO(glm::vec3& pt, glm::vec3& rgb)
{
	glGenBuffers(2, m_vboHandles);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &pt.x, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &rgb.r, GL_STATIC_DRAW);

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
void GPoint::render()
{
	if (m_vaoHandle == 0) return;


	glBindVertexArray(m_vaoHandle);

	glPointSize(10.0f);
	glDisable(GL_DEPTH_TEST);
	
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glPointSize(1);
}

void GPoint::Update(glm::vec3 pt, glm::vec3 rgb)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[0]);
	float* ptr = (float*)glMapBufferARB(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr)
	{
		ptr[0] = pt.x;
		ptr[1] = pt.y;
		ptr[2] = pt.z;
		glUnmapBufferARB(GL_ARRAY_BUFFER);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vboHandles[1]);
	ptr = (float*)glMapBufferARB(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr)
	{
		ptr[0] = rgb.x;
		ptr[1] = rgb.y;
		ptr[2] = rgb.z;
		glUnmapBufferARB(GL_ARRAY_BUFFER);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}