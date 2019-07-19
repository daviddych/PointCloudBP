#pragma once
#include "FileObj.h"
#include "lasreader.hpp"
#include "laswriter.hpp"
#include <string>

class LasFile : public FileObj
{
public:
	LasFile();
	~LasFile();

	// Load data from a *.las file.
	bool openfile(const char* filename, char* parse_string = "xyzirgb") override;

private:
	LASreader  *m_lasreader;
	LASheader  *m_header;

	LASreadOpener m_lasreadopener;

	static bool read(LasFile* lasf, std::string filename, char* parse_string = "xyzirgb");
};

