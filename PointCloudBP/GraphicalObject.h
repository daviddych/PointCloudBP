#pragma once
#include "BoundBox.h"
#include <vector>

/// <summary>
/// ͼ�ζ������
/// </summary>
class GraphicalObject
{
	using uint = unsigned int;

public:
	GraphicalObject(BoundBox* box = nullptr);
	~GraphicalObject();

public:
	// ����
	virtual void render()=0;
	// ������ɫ
	virtual void update_color(const std::vector<uint>& idx, const std::vector<glm::vec3>& rgb) {}
	// ��ѯ����
	virtual unsigned int get_size() { return 0; }
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