#pragma once
#include "BoundBox.h"
#include <vector>

/// <summary>
/// ͼ�ζ������
/// </summary>
class GraphicalObject
{
public:
	GraphicalObject(BoundBox* box = nullptr);
	~GraphicalObject();

public:
	// ����
	virtual void render()=0;
	void RenderSelected() { render(); if (m_box != nullptr) m_box->render(); }

	// ��ȡ����İ�Χ�У�(cx,cy,cz) Ϊ��������
	BoundBox* GetBoundBox() { return m_box; }

protected:
	BoundBox* m_box;
	unsigned int m_vaoHandle;
	unsigned int m_vboHandles[2];

	// �����Χ�м���
	virtual void Box() = 0;
};