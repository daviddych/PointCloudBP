#include "stdafx.h"
#include "TxtFile.h"
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <afxwin.h>
#include <sstream>
#include <string>
#include <CONIO.H>
#include <algorithm>
#include "Resource.h"

CTxtFile::CTxtFile()
{
}
bool CTxtFile::openfile(const char* filename)
{
	boost::thread t{ CTxtFile::openfile_mapping, this, filename };
	
	return true;
}

bool CTxtFile::openfile_mapping(CTxtFile* txtf, CString filename)
{
	HANDLE hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	
	DWORD size_low, size_high;
	size_low = GetFileSize(hFile, &size_high);
	size_low |= size_high << 32;

	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, size_high, size_low, NULL);
	if (hMapFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("Can't create file mapping. Error %d:\n", GetLastError());
		CloseHandle(hFile);
		return false;
	}

	// 把文件数据映射到进程的地址空间
	void* pvFile = MapViewOfFile(hMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	char* ptr = (char*)pvFile;
	
	unsigned int line_num = 0;
	for (DWORD i = 0; i <= size_low; ++i)
	{
		if (*ptr == '\n')
		{
			line_num++;
		}
		ptr++;
	}
	line_num++;
	txtf->m_xyz.resize(line_num);

	ptr = (char*)pvFile;
	int columns = calc_columns(ptr);
	if (columns != 3 && columns >= 6)
	{
		AfxMessageBox("请注意：TXT文件应该有三列或者大于等于6列");
		return false;
	}

	int nstep = floor(line_num / 100.0);
	// 进度条窗口
	CProgressStatusDlg *dlg = new CProgressStatusDlg();
	dlg->Create(IDD_PROGRESS_DLG, AfxGetMainWnd());
	dlg->ShowWindow(SW_SHOW);

	ptr = (char*)pvFile;
	
	char * str = strdup(ptr);
	char seps[] = " ,\t\r\n";
	char *token;
	token = strtok(str, seps);
	int ii = 0;
	unsigned int line_i = 0;
	
	if (columns == 3)
	{
		while (token != NULL)
		{
			switch (ii)
			{
			case 0:
				txtf->m_xyz[line_i].x = (float)atof(token);// _cprintf(" %lf\t", (double)atof(token));
				ii = 1;
				break;
			case 1:
				txtf->m_xyz[line_i].y = (float)atof(token);// _cprintf(" %lf\t", (double)atof(token));
				ii = 2;
				break;
			case 2:
				txtf->m_xyz[line_i].z = (float)atof(token);// _cprintf(" %lf\n", (double)atof(token));
				line_i++;
				if (line_i % nstep == 0)
				{
					dlg->StepIt();
				}
				ii = 0;
				break;
			}

			// Get next token:   
			token = strtok(NULL, seps); // C4996  
		}
	}
	else // ==6
	{
		while (token != NULL)
		{
			switch (ii)
			{
			case 0:
				txtf->m_xyz[line_i].x = (float)atof(token);// _cprintf(" %lf\t", (double)atof(token));
				ii = 1;
				break;
			case 1:
				txtf->m_xyz[line_i].y = (float)atof(token);// _cprintf(" %lf\t", (double)atof(token));
				ii = 2;
				break;
			case 2:
				txtf->m_xyz[line_i].y = (float)atof(token);// _cprintf(" %lf\t", (double)atof(token));
				ii = 3;
				break;
			case 3:
				txtf->m_rgb[line_i].x = (float)atof(token);// _cprintf(" %lf\t", (double)atof(token));
				ii = 4;
				break;
			case 4:
				txtf->m_rgb[line_i].y = (float)atof(token);// _cprintf(" %lf\t", (double)atof(token));
				ii = 5;
				break;
			case 5:
				txtf->m_rgb[line_i].z = (float)atof(token);// _cprintf(" %lf\n", (double)atof(token));
				line_i++;
				if (line_i % nstep == 0)
				{
					dlg->StepIt();
				}
				ii = 0;
				break;
			}

			// Get next token:   
			token = strtok(NULL, seps); // C4996  
		}
	}

	free(str);
		
	// 结束了，关闭文件映射.
	UnmapViewOfFile(pvFile);
	CloseHandle(hFile);

	txtf->m_offset = txtf->center(txtf->m_xyz);
	normalize(txtf->m_xyz, txtf->m_offset);

	txtf->SenCompletedMessage();
	return true;
}

int CTxtFile::calc_columns(const char * Str, int readed, char* seps)
{
	char * str = strdup(Str);  // 切忌需要free(str).
	char * pos = strchr(str, '\n');
	if (pos != NULL) {
		*pos = '\0';
	}

	int columns = 0;

	char * token = strtok(str, seps);
	while (token != NULL) {
		double val = (double)atof(token);
		if (val != 0)
			columns++;
		else {
			if (strlen(token) != 0) {
				if ((token[0] >= '0') && (token[0] <= '9'))
					columns++;
			}
		}
		token = strtok(NULL, seps);
	};

	free(str);
	return columns;
}

bool CTxtFile::openfile_ifstream(CString filename /*= CString("")*/)
{
	std::ifstream infile;
	infile.open(filename);
	if (!infile.rdbuf()->is_open())
		return false;

	char string_buf[256];
	unsigned int line_num = 0;
	do
	{
		memset(string_buf, 0, 256 * sizeof(char));
		infile.getline(string_buf, 256);
		line_num++;
	} while (string_buf[0] != 0);
	--line_num;
	infile.close();

	infile.open(filename);
	m_xyz.resize(line_num);
	infile.seekg(0, std::ios::beg);
	float x(0), y(0), z(0);
	for (unsigned int i = 0; i < line_num; ++i)
	{
		infile >> m_xyz[i].x >> m_xyz[i].y >> m_xyz[i].z;
	}
	infile.close();
	return true;
}

CTxtFile::~CTxtFile()
{
}

bool cmpx(const glm::vec3 &a, const glm::vec3 &b)
{
	return a.x < b.x;
}

bool cmpy(const glm::vec3 &a, const glm::vec3 &b)
{
	return a.y < b.y;
}

bool cmpz(const glm::vec3 &a, const glm::vec3 &b)
{
	return a.z < b.z;
}

void CTxtFile::normalize(std::vector<glm::vec3>& xyz, glm::vec3 offset)
{
	for (std::vector<glm::vec3>::iterator iter= xyz.begin(); iter != xyz.end(); ++iter)
	{
		*iter -= offset;
	}
}

glm::vec3 CTxtFile::center(std::vector<glm::vec3>& xyz)
{
	auto result = std::minmax_element(xyz.begin(), xyz.end(), cmpx);
	float min_x((*result.first).x), max_x((*result.second).x);

	result = std::minmax_element(xyz.begin(), xyz.end(), cmpy);
	float min_y((*result.first).y), max_y((*result.second).y);

	result = std::minmax_element(xyz.begin(), xyz.end(), cmpz);
	float min_z((*result.first).z), max_z((*result.second).z);

	return glm::vec3(max_x + min_x, max_y + min_y, max_z + min_z) / 2.0f;
}