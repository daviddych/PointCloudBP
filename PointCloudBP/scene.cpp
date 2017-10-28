#include "stdafx.h"
#include "scene.h"
#include "GL/glew.h"
#include "GL/wglew.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>

Scene::Scene() : m_animate(true), m_pDC(nullptr),m_hRC(nullptr)
{
}

Scene::~Scene()
{
}

bool Scene::sceneInit(CDC* pDC)
{ 
	if (!CreateGLContext(pDC, m_hRC))
	{
		//Something went wrong with getting the rendering context.
		AfxMessageBox(CString("Unable to create GL context"));
		m_hRC = nullptr;
		return false;
	}

	initScene();
	return true;
}

bool Scene::CreateGLContext(CDC* pDC, HGLRC& hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_GDI;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);

	if (nPixelFormat == 0)
	{
		nPixelFormat = 1;
		if (DescribePixelFormat(pDC->m_hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd) == 0)
		{
			return false;
		}
	}

	if (SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd) == false)
		return false;

	// Create standard OpenGL (2.1) rendering context which will be used only temporarily (tempContext), and make it current
	HGLRC tempContext = wglCreateContext(pDC->m_hDC);
	wglMakeCurrent(pDC->m_hDC, tempContext);

	// Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		AfxMessageBox(_T("GLEW is not initialized!"));
	}

	//Checking GL version
	const GLubyte *GLVersionString = glGetString(GL_VERSION);

	//Or better yet, use the GL3 way to get the version number
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	// Setup attributes for a brand new OpenGL 3.1 rendering context
	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, OpenGLVersion[0],
		WGL_CONTEXT_MINOR_VERSION_ARB, OpenGLVersion[1],
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		// Create new rendering context and Delete tempContext
		hRC= wglCreateContextAttribsARB(pDC->m_hDC, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(pDC->m_hDC, hRC);
	}
	else
	{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		hRC = tempContext;
	}

	if (!hRC) 
		return false;

	m_pDC = pDC;
	m_hRC = hRC;
	return true;
}

void Scene::SwapBuffers()
{
	if (FALSE == ::SwapBuffers(m_pDC->GetSafeHdc()))
		AfxMessageBox(CString("SwapBuffers failed"));
}
void Scene::releaseRes()
{
	wglMakeCurrent(NULL, NULL);
	if (m_hRC != nullptr)
	{
		if (FALSE == ::wglDeleteContext(m_hRC))
			AfxMessageBox(CString("wglDeleteContext failed"));
		m_hRC = NULL;
	}

	if (m_pDC != nullptr)
		delete m_pDC;
}