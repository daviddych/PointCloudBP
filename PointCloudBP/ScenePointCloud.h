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
#include "ColorMap.h"

using glm::mat4;
using glm::vec3;

enum class SHOW_COLOR_TYPE{SH_REAL,SH_ALTITUDE, SH_CLASSIFICATION, SH_INTENSIFY, SH_RETURNS};

class ScenePointCloud :	public Scene
{ 
	using uint = unsigned int;
public:
	ScenePointCloud();
	~ScenePointCloud();
	
	GLSLProgram* getCurProgram() { return &prog; }; // { return this->prog; }
private:
	GLSLProgram prog;

	int m_view_width, m_view_height;
	float m_depthz, m_initdepth; // 分别记录当前深度和场景初始载入深度
	glm::vec3 m_obj_rotate{ .0f, .0f, .0f };;
	glm::vec3 m_obj_translate{.0f, .0f, .0f};
	glm::vec3 m_scene_offset{ .0f, .0f, .0f };
	
	// 定义了整个绘制场景的全部对象包围盒和绘制对象
	BoundBox *m_box;                              // 整个场景的包围盒
	std::vector<GraphicalObject*>  m_graphicObjs; // 场景对象
	JetMap jetMap;

	glm::mat4 m_model;
	glm::mat4 m_view;
	glm::mat4 m_projection;

	void initScene();
	void initialize_rotate_angle();
	void setMatrices();
	void compileAndLinkShader();
	glm::vec3  project(glm::vec3 obj);	// Get screen coordinates from an object point
	glm::vec3  unproject(glm::vec3 screenCoords);	// Get object coordinates from an screen point

	// 修改m_graphicObjs[obj_id]中点xyz[idx]的颜色
	void update_color(const std::vector<uint>& idx, const std::vector<glm::vec3>& rgb, uint obj_id = 0);
	// 分析数据，转换为JET颜色映射，返回rgb颜色信息
	std::vector<glm::vec3> colorMap(std::vector<float> &data);
public:
	// 
	void update_color(SHOW_COLOR_TYPE color_type=SHOW_COLOR_TYPE::SH_REAL);
	glm::vec3 get_scene_offset();
	void update(float t);
	// 查询指定点云场景中的点数量
	unsigned int get_point_number(uint obj_id=0);
	void render_old(); // 作废
	void render();
	void resize(int, int);
	void setLookAt(vec3 const & eye = vec3(0.0f, 0.0f, 10.0f), glm::vec3 const & center= vec3(0.0f, 0.0f, 0.0f), glm::vec3 const & up= vec3(0.0f, 1.0f, 0.0f));
	void setSceneView(glm::vec3 obj_xyz_min, glm::vec3 obj_xyz_center, glm::vec3 obj_xyz_max);
	void setSceneView(BoundBox* box);
	void zoom(int zDelta);
	void rotate(float xDelta, float zDelta);
	void translate(int xDelta, int yDelta);
	// 重置场景旋转和平移
	void reset();
	void add_obj(GraphicalObject *obj, glm::vec3 offset=glm::vec3()); // 向场景中添加一个对象
	bool pickpoint(int winx, int winy, glm::vec3& picked,int active_pcl=0); // 从当前活动的点云数据中拾取点数据
	bool empty() { return m_graphicObjs.empty(); }
	void clear(); // 清除已经加载的数据
};