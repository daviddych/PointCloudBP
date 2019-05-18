#include "stdafx.h"
#include "ScenePointCloud.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "glutils.h"

#include <omp.h>

using std::cerr;
using std::endl;

using glm::vec3;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>

ScenePointCloud::ScenePointCloud():m_depthz(0.0f), m_initdepth(0.0f)
{
	initialize_rotate_angle();

	m_box = nullptr;
	m_initdepth = 0;
}

ScenePointCloud::~ScenePointCloud()
{
	clear();
}

void ScenePointCloud::clear()
{
	if (m_box != m_box) {
		delete m_box;
		m_box = nullptr;
	}

	if (!m_graphicObjs.empty()) {
		for (auto& obj : m_graphicObjs) {
			delete obj;
			obj = nullptr;
		}
		m_graphicObjs.clear();
// 		for (size_t i = 0; i < m_graphicObjs.size(); ++i)
// 		{
// 			delete m_graphicObjs[i];
// 			m_graphicObjs[i] = nullptr;
// 		}
// 		m_graphicObjs.clear();
	}
}

void ScenePointCloud::initialize_rotate_angle()
{
	m_obj_rotate.x = 302; m_obj_rotate.y = 6; m_obj_rotate.z = 0;
}

void ScenePointCloud::compileAndLinkShader()
{
	try {
		prog.compileShader("../glsl/pointcloud.vert"); // subroutine
		prog.compileShader("../glsl/pointcloud.frag");
		prog.link();
		prog.use();
	}
	catch (GLSLProgramException & e) {
		cerr << e.what() << endl;
		_cprintf("Error: %s\n", e.what());
		exit(EXIT_FAILURE);
	}
}

void ScenePointCloud::setLookAt(glm::vec3 const & eye, glm::vec3 const & center, glm::vec3 const & up)
{
	m_view = glm::lookAt(eye, center, up);
}

void ScenePointCloud::initScene()
{
	glClearColor(0.0235f, 0.2588f, 0.3843f, 1.0f);
	compileAndLinkShader();

	std::cout << std::endl;
	prog.printActiveUniforms();

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);                         // 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	//prog.setUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
	//prog.setUniform("Light.Ld", 1.0f, 1.0f, 1.0f);
	//prog.setUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
	//prog.setUniform("Light.La", 0.4f, 0.4f, 0.4f);
	//prog.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
	//prog.setUniform("Light.Ls", 1.0f, 1.0f, 1.0f);
	//prog.setUniform("Material.Shininess", 100.0f);
}

void ScenePointCloud::update(float t) 
{
	if (m_animate) {
		m_obj_rotate.z += 1.0f;
		if (m_obj_rotate.z >=360.0f)
		{
			m_obj_rotate.z -= 360.0f;
		}
	}
}

void ScenePointCloud::update_color(const std::vector<uint>& idx, const std::vector<glm::vec3>& rgb, uint obj_id)
{
	if (m_graphicObjs.empty() || m_graphicObjs.size() <= obj_id)
		return;

	GraphicalObject *graphicObjs = m_graphicObjs[obj_id];
	graphicObjs->update_color(idx, rgb);
}

unsigned int ScenePointCloud::get_point_number(uint obj_id) 
{
	if (m_graphicObjs.empty() || m_graphicObjs.size() <= obj_id)
		return 0;

	GraphicalObject *graphicObjs = m_graphicObjs[obj_id];
	return graphicObjs->get_size();
}

void ScenePointCloud::render_old()
{// 作废
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec4 lightPos = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	prog.setUniform("Light.Position", lightPos);

	GLuint programHandle = prog.getHandle();

	GLuint adsIndex = glGetSubroutineIndex(programHandle, GL_VERTEX_SHADER, "phongModel");
	GLuint diffuseIndex = glGetSubroutineIndex(programHandle, GL_VERTEX_SHADER, "diffuseOnly");

	//glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &adsIndex);
	//m_model = mat4(1.0f);
	//m_model *= glm::translate(vec3(-3.0, -1.5, 0.0));
	//m_model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	//setMatrices();
	//teapot->render_old();

	//glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &diffuseIndex);
	//m_model = mat4(1.0f);
	//m_model *= glm::translate(vec3(3.0f, -1.5f, 0.0f));
	//m_model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	//setMatrices();
	//teapot->render_old();
}
void ScenePointCloud::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec4 lightPos = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	prog.setUniform("Light.Position", lightPos);

	GLuint programHandle = prog.getHandle();

	GLuint adsIndex = glGetSubroutineIndex(programHandle, GL_VERTEX_SHADER, "phongModel");
	GLuint diffuseIndex = glGetSubroutineIndex(programHandle, GL_VERTEX_SHADER, "diffuseOnly");

	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &adsIndex);
	m_model = mat4(1.0f);
	m_model *= glm::translate(m_obj_translate);
	m_model *= glm::translate(vec3(0, 0, m_depthz));
	m_model *= glm::rotate(glm::radians(m_obj_rotate.z), vec3(0.0f, 0.0f, 1.0f));
	m_model *= glm::rotate(glm::radians(m_obj_rotate.y), vec3(0.0f, 1.0f, 0.0f));
	m_model *= glm::rotate(glm::radians(m_obj_rotate.x), vec3(1.0f, 0.0f, 0.0f));
	//if (m_box != nullptr)
	//{ // 测试结果表明，数据太大时，会存在绘制偏差
	//	m_model *= glm::translate(glm::vec3(-m_box->_center.x, -m_box->_center.y, -m_box->_center.z));
	//}
	
	setMatrices();

	// draw objects
	if (!m_graphicObjs.empty())
	{
		for (const auto& obj : m_graphicObjs)
		{
			obj->render();
		}
		m_box->render();
	}
}

glm::vec3 ScenePointCloud::get_scene_offset()
{
	return m_scene_offset;
}

void ScenePointCloud::setSceneView(BoundBox* box)
{
// 	if (m_box != nullptr)
// 		delete m_box;
// 
// 	m_box = new BoundBox(box);

	if (m_box==nullptr)
	{
		m_box = new BoundBox(box);
	}
	else
	{
		m_box->update(box);
	}

	m_depthz = -std::max(m_box->_x_length, std::max(m_box->_y_length, m_box->_z_length)) *1.73f;
	m_initdepth = m_depthz;
	m_initdepth = m_depthz;
	setLookAt(m_box->_center + glm::vec3(0, 0, 1), m_box->_center);
	//_cprintf("adf\n");
}

void ScenePointCloud::setSceneView(glm::vec3 obj_xyz_min, glm::vec3 obj_xyz_center, glm::vec3 obj_xyz_max)
{
	if (m_box == nullptr)
	{
		m_box = new BoundBox(obj_xyz_min, obj_xyz_max);
	}
	else
	{
		m_box->plus(obj_xyz_min, obj_xyz_max);
	}
	m_depthz = -std::max(m_box->_x_length, std::max(m_box->_y_length, m_box->_z_length)) *1.73f;
	m_initdepth = m_depthz;
	setLookAt(m_box->_center + glm::vec3(0, 0, 1), m_box->_center);
	_cprintf("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\n");
}

void ScenePointCloud::zoom(int zDelta)
{
	m_depthz += m_box->_z_length*zDelta*0.05f;
	_cprintf("zDelta = %d, depth = %f\n", zDelta, m_depthz/m_initdepth);
}

void ScenePointCloud::rotate(float xDelta, float zDelta)
{
	m_obj_rotate.x += zDelta;
	m_obj_rotate.y += xDelta;
	if (m_obj_rotate.x>=360.0f)
	{
		m_obj_rotate.x -= 360.0f;
	}
	else if (m_obj_rotate.x < 0.0f)
	{
		m_obj_rotate.x += 360.0f;
	}

	if (m_obj_rotate.y >= 360.0f)
	{
		m_obj_rotate.y -= 360.0f;
	}
	else if (m_obj_rotate.y< 0.0f)
	{
		m_obj_rotate.y += 360.0f;
	}

	_cprintf("rotate: <%f,  %f>\n", m_obj_rotate.x, m_obj_rotate.y);
	return;
}

void ScenePointCloud::translate(int xDelta, int yDelta)
{
	m_obj_translate.x -= xDelta*0.01f*m_depthz;
	m_obj_translate.y += yDelta*0.01f*m_view_height/m_view_width*m_depthz;
}

void ScenePointCloud::setMatrices()
{
	glm::mat4 mv = m_view * m_model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix",	mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", m_projection * mv);
}

void ScenePointCloud::resize(int w, int h)
{
	if (w > 0 && h > 0)
	{
		m_view_width = w;
		m_view_height = h;
		glViewport(0, 0, m_view_width, m_view_height);
		m_projection = glm::perspective(glm::radians(45.0f), (float)m_view_width / m_view_height, 0.1f, 1000000.0f);
	}
}

void ScenePointCloud::reset()
{
	initialize_rotate_angle();
	m_depthz = m_initdepth;
	m_obj_translate = glm::vec3{.0f, .0f, .0f};
}

void ScenePointCloud::add_obj(GraphicalObject *obj, glm::vec3 offset)
{
	setSceneView(obj->GetBoundBox());

	// 清空场景，仅容纳一个点云数据集
	if (!m_graphicObjs.empty())
	{
		delete m_graphicObjs[0];
		m_graphicObjs[0] = nullptr;
		m_graphicObjs.clear();
	}
	m_graphicObjs.push_back(obj);
	m_scene_offset = offset;
}

glm::vec3  ScenePointCloud::project(glm::vec3 obj)
{
	return glm::project(obj,m_view*m_model,m_projection,glm::vec4(0, 0, m_view_width, m_view_height));
}

glm::vec3  ScenePointCloud::unproject(glm::vec3 screenCoords)
{
	return glm::unProject(screenCoords,m_model,m_projection, glm::vec4(0, 0, m_view_width, m_view_height));
}

// 遍历所有点在屏幕上的坐标，搜索出点（winx，winy）的最近点。
bool ScenePointCloud::pickpoint(int winx, int winy, glm::vec3& picked, int active_pcl)
{
	if (m_graphicObjs.empty())
		return false;

	assert(active_pcl >= 0 && active_pcl < m_graphicObjs.size());

	std::vector<glm::vec3>& xyz = ((GPointCloud*)m_graphicObjs[active_pcl])->get_xyz();
	std::vector<double> dists(xyz.size(), std::numeric_limits<double>::max());
#pragma omp parallel for
	for (int i = 0; i < xyz.size(); ++i)
	{
		glm::vec3 pt = project(xyz[i]);
		if (pt.z > 0.0f && pt.z < 1.0f)
		{
			double dx = winx - pt.x; 
			double dy = winy - m_view_height + pt.y;
			dists[i] = dx * dx + dy * dy;
		}
	}
	std::vector<double> ::iterator resultIter = std::min_element(dists.begin(), dists.end());
	picked = xyz[resultIter - dists.begin()];
	_cprintf("Picked Point: <%f, %f, %f>\n", picked.x, picked.y, picked.z);
	return true;
}

