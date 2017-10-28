//
//  uofrGraphics.h
//  mytest
//
//  Created by Alex Clarke on 2013-05-02.
//  Copyright (c) 2013 CS 315. All rights reserved.
//
#include "stdafx.h"
#ifndef __uofrGraphics__
#define __uofrGraphics__

#include <iostream>
#include <cstring>
#include "Angel.h"

class uofrGraphics {
    GLuint shaderProgram;
    GLuint positionAttribLoc;
    GLuint normalAttribLoc;
    GLuint colourAttribLoc;
    vec3 colour;
    GLuint primitive;
    
public:
    uofrGraphics() { primitive = GL_TRIANGLES; }
    void connectShader(GLuint shaderProgram,
                       const char* positionAttribName,
                       const char* normalAttribName,
                       const char* colourAttribName);
    
    void setDrawColour(vec3 colour);
    void setDrawColour(float r, float g, float b);
    void drawSolidSphere(GLfloat radius, GLint slices, GLint stacks);
    void drawSolidCube(GLfloat size);
    void drawQuad(vec4 v1, vec4 v2, vec4 v3, vec4 v4);
    void drawWireSphere(GLfloat radius, GLint slices, GLint stacks);
    void drawWireCube(GLfloat size);
    void drawSolidTorus( GLfloat innerRadius, GLfloat outerRadius, GLint nSides, GLint nRings);
    void drawWireTorus( GLfloat innerRadius, GLfloat outerRadius, GLint nSides, GLint nRings);
};

extern uofrGraphics urgl;

#endif /* defined(__uofrGraphics__) */
