#pragma once
#include "BoundBox.h"
#include <vector>

/// <summary>
/// 图形对象基类
/// </summary>
class GraphicalObject
{
public:
	GraphicalObject(BoundBox* box = nullptr);
	~GraphicalObject();

public:
	// 绘制
	virtual void render()=0;
	void RenderSelected() { render(); if (m_box != nullptr) m_box->render(); }

	// 获取对象的包围盒，(cx,cy,cz) 为中心坐标
	BoundBox* GetBoundBox() { return m_box; }

protected:
	BoundBox* m_box;
	unsigned int m_vaoHandle;
	unsigned int m_vboHandles[2];

	// 对象包围盒计算
	virtual void Box() = 0;
};