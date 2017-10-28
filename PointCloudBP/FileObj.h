#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

class FileObj
{
public:
	FileObj();
	~FileObj();

	virtual bool openfile(const char* filename)=0;
	std::vector<glm::vec3> get_xyz() { return m_xyz; }
	std::vector<glm::vec3> get_rgb() { return m_rgb; }
	glm::vec3 get_offset() { return m_offset; }
	void update_offset(glm::vec3 offset);// reset data using a global offset.

protected:
	std::vector<glm::vec3> m_xyz;
	std::vector<glm::vec3> m_rgb;
	glm::vec3              m_offset;     // the center of object coordinates.
};

