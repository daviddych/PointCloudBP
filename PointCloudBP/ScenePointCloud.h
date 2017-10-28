#pragma once

#include "scene.h"
#include "glslprogram.h"
#include "vboteapot.h"
#include "vbotorus.h"

#include "cookbookogl.h"

#include <glm/glm.hpp>
#include <vector>
#include "GPointCloud.h"
#include <vector>

using glm::mat4;
using glm::vec3;

class ScenePointCloud :	public Scene
{ 
public:
	ScenePointCloud();
	~ScenePointCloud();
	
	GLSLProgram* getCurProgram() { return &prog; }; // { return this->prog; }
private:
	GLSLProgram prog;

	int m_view_width, m_view_height;
	float m_depthz;
	glm::vec3 m_obj_rotate;
	glm::vec3 m_obj_translate;
	glm::vec3 m_scene_offset;
	float m_initial_radius;
	
	// �������������Ƴ�����ȫ�������Χ�кͻ��ƶ���
	BoundBox *m_box;                              // ���������İ�Χ��
	std::vector<GraphicalObject*>  m_graphicObjs; // ��������

	glm::mat4 m_model;
	glm::mat4 m_view;
	glm::mat4 m_projection;

	void initScene();
	void initialize_rotate_angle();
	void setMatrices();
	void compileAndLinkShader();
	glm::vec3  project(glm::vec3 obj);	// Get screen coordinates from an object point
	glm::vec3  unproject(glm::vec3 screenCoords);	// Get object coordinates from an screen point

	
public:
	glm::vec3 get_scene_offset();
	void update(float t);
	void render_old(); // ����
	void render();
	void resize(int, int);
	void setLookAt(vec3 const & eye = vec3(0.0f, 0.0f, 10.0f), glm::vec3 const & center= vec3(0.0f, 0.0f, 0.0f), glm::vec3 const & up= vec3(0.0f, 1.0f, 0.0f));
	void setSceneView(glm::vec3 obj_xyz_min, glm::vec3 obj_xyz_center, glm::vec3 obj_xyz_max);
	void setSceneView(BoundBox* box);
	void zoom(int zDelta);
	void rotate(float xDelta, float zDelta);
	void translate(int xDelta, int yDelta);
	void reset();
	void add_obj(GraphicalObject *obj, glm::vec3 offset=glm::vec3()); // �򳡾������һ������
	bool pickpoint(int winx, int winy, glm::vec3& picked,int active_pcl=0); // �ӵ�ǰ��ĵ���������ʰȡ������
	bool empty() { return m_graphicObjs.empty(); }
};