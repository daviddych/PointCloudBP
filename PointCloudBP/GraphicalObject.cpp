#include "stdafx.h"
#include "GraphicalObject.h"


GraphicalObject::GraphicalObject(BoundBox* box)
{
	m_box = box;
	m_vaoHandle = 0;
}

GraphicalObject::~GraphicalObject()
{
	if (m_box != nullptr)
	{
		delete m_box;
		m_box = nullptr;
	}
	if (m_vaoHandle >0)
	{
		glDeleteBuffers(2, m_vboHandles); m_vboHandles[0] = 0; m_vboHandles[1] = 0;
		glDeleteVertexArrays(1, &m_vaoHandle);	m_vaoHandle = 0;
	}
}