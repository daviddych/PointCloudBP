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
	

public:
	CTxtFile();
	~CTxtFile();
	bool openfile(const char* filename, char* parse_string = "xyzirgb") override;
};