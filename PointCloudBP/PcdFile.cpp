#include "stdafx.h"
#include "PcdFile.h"
#include "Resource.h"
#include <algorithm>
//#include <initializer_list>
#ifdef _DEBUG
#pragma comment(lib, "pcl_common_debug.lib")
#pragma comment(lib, "pcl_io_debug.lib")
#else
#pragma comment(lib, "pcl_io_release.lib")
#pragma comment(lib, "pcl_common_release.lib")
#endif


PcdFile::PcdFile()
{
}


PcdFile::~PcdFile()
{
}

bool PcdFile::find(const std::vector<::pcl::PCLPointField>& fields, const std::string& field)// 查询是否包含指定的字段
{
	std::string lfield = field;
	std::transform(std::begin(lfield), std::end(lfield), std::begin(lfield), ::tolower);
	for (auto& val : fields)
	{
		std::string tfield = val.name;
		std::transform(std::begin(tfield), std::end(tfield), std::begin(tfield), ::tolower);
		
		if (tfield == lfield)
			return true;
	}

	return false;
}

bool PcdFile::read(PcdFile* lasf, std::string filename, char* parse_string)
{
	// Progress Window
	CProgressStatusDlg *dlg = new CProgressStatusDlg();
	dlg->Create(IDD_PROGRESS_DLG, AfxGetMainWnd());
	dlg->ShowWindow(SW_SHOW);

// 	pcl::PCDReader pcdReader;
// 	pcl::PCLPointCloud2 pclPointCloud2;
// 	const int offset = 0;
// 	int res = pcdReader.readHeader(filename, pclPointCloud2, offset);

	pcl::PCLPointCloud2 cloud_blob;
	pcl::io::loadPCDFile(filename, cloud_blob);
	std::vector<::pcl::PCLPointField>& fields = cloud_blob.fields;
	dlg->SetPos(50);

	if (fields.size() > 3)
	{
		if (find(fields, "rgb"))
		{
			pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
			pcl::fromPCLPointCloud2(cloud_blob, *cloud); //* convert from pcl/PCLPointCloud2 to pcl::PointCloud<T>
			dlg->StepIt();
			lasf->convert2xyzrgb(cloud);
			dlg->StepIt();
		}
		else if (find(fields, "intensity"))
		{
			pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);
			pcl::fromPCLPointCloud2(cloud_blob, *cloud);
			dlg->StepIt();
			lasf->convert2xyzrgb(cloud);
			dlg->StepIt();
		}
	}
	else
	{
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::fromPCLPointCloud2(cloud_blob, *cloud); //* convert from pcl/PCLPointCloud2 to pcl::PointCloud<T>
		dlg->StepIt();
		lasf->convert2xyzrgb(cloud);
		dlg->StepIt();
	}

	lasf->m_offset = center(lasf->m_xyz);
	dlg->StepIt();
	normalize(lasf->m_xyz, lasf->m_offset);

	dlg->SetPos(100);
	delete dlg;

	lasf->SenCompletedMessage();
	return true;
}

bool PcdFile::openfile(const char* filename, char* parse_string)
{
	boost::thread t{ PcdFile::read, this, filename, parse_string };
	return true;
}

void PcdFile::convert2xyzrgb(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud)
{
	m_xyz.resize(cloud->points.size());
	m_rgb.resize(cloud->points.size(), glm::vec3(0.5f,0.5f,0.5f));
	glm::vec3 m_pt3f;
	std::transform(cloud->points.begin(), cloud->points.end(), m_xyz.begin(), 
		[&m_pt3f](const pcl::PointXYZ& pt) { m_pt3f.x = pt.x; m_pt3f.y = pt.y; m_pt3f.z = pt.z; return m_pt3f; });
}

void PcdFile::convert2xyzrgb(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud)
{
	m_xyz.resize(cloud->points.size());
	m_rgb.resize(cloud->points.size());
	glm::vec3 pt3f;
	std::transform(cloud->points.begin(), cloud->points.end(), m_xyz.begin(),
		[&pt3f](const pcl::PointXYZI& pt) { pt3f.x = pt.x; pt3f.y = pt.y; pt3f.z = pt.z; return pt3f; });

	std::vector<float> intensities(cloud->points.size(), 0);
	std::transform(cloud->points.begin(), cloud->points.end(), intensities.begin(),
		[](const pcl::PointXYZI& pt) { return pt.intensity; });

	auto inres = std::minmax_element(intensities.begin(), intensities.end(), [](float a, float &b) { return a < b; });
	unsigned short min_i(*inres.first), max_i(*inres.second);
	if (max_i > 255.0f)
	{
		std::transform(intensities.begin(), intensities.end(), intensities.begin(), [](float v) { return v / 65535.0f; });
	}
	else if (max_i > 1.0f)
	{
		std::transform(intensities.begin(), intensities.end(), intensities.begin(), [](float v) { return v / 255.0f; });
	}

	std::transform(intensities.begin(), intensities.end(), m_rgb.begin(),
		[&pt3f](const float intensity) { pt3f.x = intensity; pt3f.y = intensity; pt3f.z = intensity; return pt3f; });
}

void PcdFile::convert2xyzrgb(pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud)
{
	m_xyz.resize(cloud->points.size());
	m_rgb.resize(cloud->points.size());
	glm::vec3 pt3f;
	std::transform(cloud->points.begin(), cloud->points.end(), m_xyz.begin(),
		[&pt3f](const pcl::PointXYZRGB& pt) { pt3f.x = pt.data[0]; pt3f.y = pt.data[1]; pt3f.z = pt.data[2]; return pt3f; });

	std::transform(cloud->points.begin(), cloud->points.end(), m_rgb.begin(),
		[&pt3f](const pcl::PointXYZRGB& pt) { pt3f.x = pt.r; pt3f.y = pt.g; pt3f.z = pt.b; return pt3f; });

	auto result = std::minmax_element(m_rgb.begin(), m_rgb.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return a.x < b.x; });
	float min_r((*result.first).x), max_r((*result.second).x);

	result = std::minmax_element(m_rgb.begin(), m_rgb.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return a.y < b.y; });
	float min_g((*result.first).y), max_g((*result.second).y);

	result = std::minmax_element(m_rgb.begin(), m_rgb.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return a.z < b.z; });
	float min_b((*result.first).z), max_b((*result.second).z);

	float max_v = std::max(max_r, std::max(max_g, max_b));
	if (max_v > 255.0f)
	{
		std::transform(m_rgb.begin(), m_rgb.end(), m_rgb.begin(), [](glm::vec3& v) { return v / 65535.0f; });
	}
	else if (max_v > 1.0f)
	{
		std::transform(m_rgb.begin(), m_rgb.end(), m_rgb.begin(), [](glm::vec3& v) { return v / 255.0f; });
	}
}