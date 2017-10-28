#pragma once
#include "FileObj.h"
#include "lasreader.h"
#include "laswriter.h"
#include <string>

class LasFile : public FileObj
{
public:
	LasFile();
	~LasFile();

	bool openfile(const char* filename);

private:
	LASreader  *m_lasreader;
	static bool read(LasFile* lasf, std::string filename);
};
