#ifndef SCENE_H
#define SCENE_H

class Scene
{
public:
	Scene();
	~Scene();

	// OpenGL 环境初始化
	bool sceneInit(CDC* pDC);

	void SwapBuffers();
	// 释放资源
	void releaseRes();

    /** This is called prior to every frame.  Use this to update your animation.  */
    virtual void update( float t ) = 0;

    /** Draw your scene.   */
    virtual void render_old() = 0;

    /** Called when screen is resized */
    virtual void resize(int, int) = 0;
    
    void animate( bool value ) { m_animate = value; }
    bool animating() { return m_animate; }

protected:
	/** 创建OpenGL环境，如果成功，返回true和有效的hRC */
	/** OpenGL Setup, Rendering Context and Device Context Pointers */
	bool CreateGLContext(CDC* pDC, HGLRC& hRC);

	/** Load textures, initialize shaders, etc.  */
	virtual void initScene() = 0;

	bool      m_animate;
	HGLRC     m_hRC;
	CDC*      m_pDC;
};

#endif // SCENE_H
