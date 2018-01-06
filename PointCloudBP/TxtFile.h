#pragma once
#include "FileObj.h"
#include <fstream>
#include <glm/glm.hpp>
#include <vector>

#include <stdio.h>

class CTxtFile : public FileObj
{
private:
	static bool openfile_mapping(CTxtFile* txtf, const std::string& filename = std::string(""));
	bool openfile_ifstream(const std::string& filename = std::string(""));
	static int  calc_columns(const char * Str, int readed=2048, char *seps = " \t");
	static void normalize(std::vector<glm::vec3>& xyz, glm::vec3 offset); // 对xyz每一个元素偏移.
	static glm::vec3 center(std::vector<glm::vec3>& xyz);    // calculate the center point

public:
	CTxtFile();
	~CTxtFile();
	bool openfile(const char* filename);
};