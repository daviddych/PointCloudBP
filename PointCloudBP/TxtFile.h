#pragma once
#include "FileObj.h"
#include <fstream>
#include <glm/glm.hpp>
#include <vector>

#include <stdio.h>

class CTxtFile : public FileObj
{
private:
	bool openfile_mapping(CString filename = CString(""));
	bool openfile_ifstream(CString filename = CString(""));
	int  calc_columns(const char * Str, int readed=2048, char *seps = " \t");
	void normalize(std::vector<glm::vec3>& xyz, glm::vec3 offset); // 对xyz每一个元素偏移.
	glm::vec3 center(std::vector<glm::vec3>& xyz);    // 计算中心

public:
	CTxtFile();
	~CTxtFile();
	bool openfile(const char* filename);
};