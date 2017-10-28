#pragma once
#include "GraphicalObject.h"
#include <glm/glm.hpp>

class GPoint :	public GraphicalObject
{
public:
	GPoint(glm::vec3 pt, glm::vec3 rgb=glm::vec3(1.0f,0.0f,0.0f));
	~GPoint();

	void render();
	void Update(glm::vec3 pt, glm::vec3 rgb = glm::vec3(1.0f, 0.0f, 0.0f));
	void Box() {};

private:
	void createVAO(glm::vec3& pt, glm::vec3& rgb);
};

