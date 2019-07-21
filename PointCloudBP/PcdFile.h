#pragma once
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

#include "FileObj.h"

class PcdFile : public FileObj
{
public:
	PcdFile();
	~PcdFile();

	bool openfile(const char* filename, char* parse_string = "xyzirgb");

private:
	void convert2xyzrgb(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud);
	void convert2xyzrgb(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud);
	void convert2xyzrgb(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud);
	static bool read(PcdFile* lasf, std::string filename, char* parse_string = "xyzirgb");
	static bool find(const std::vector<::pcl::PCLPointField>& fields, const std::string& field); // 查询是否包含指定的字段
};