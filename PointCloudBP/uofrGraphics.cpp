//
//  uofrGraphics.cpp
//  mytest
//
//  Created by Alex Clarke on 2013-05-02.
//  Copyright (c) 2013 CS 315. All rights reserved.
//
#include "stdafx.h"

#include <cmath>
#include <cassert>
#include <iostream>
using namespace std;
#include "uofrGraphics.h"

uofrGraphics urgl;



//connectShader
//  Purpose: get vertex attribute entry points of a shader ("in" type variables)
//  Preconditions:
//     shaderProgram - the index number of a valid compiled shader program
//
//     positionAttribName - the name of the vertex position input as it appears
//                          in the shader's code. The input MUST be of type vec4
//                          It should not be NULL. If it is, it will be
//                          silently ignored.
//
//     normalAttribName - the name of the vertex normal input as it appears
//                        in the shader's code. The input MUST be of type vec4.
//                        It should not be NULL. If it is, it will be
//                        silently ignored.
//
//     colourAttribName - the name of the colour position input as it appears
//                        in the shader's code. The input MUST be of type vec3.
//                        It should not be NULL. If it is, it will be
//                        silently ignored.
//
// PostConditions:
//     the locations for the three attribute names will be retrieved and stored
//     in corresponding *AttribLoc index variable for use when drawing.
//     If any of the names were NULL pointers or were invalid names, the error
//     is silently ignored.
void uofrGraphics::connectShader(GLuint shaderProgram,
                                 const char* positionAttribName,
                                 const char* normalAttribName,
                                 const char* colourAttribName)
{
    this->shaderProgram = shaderProgram;
    positionAttribLoc = normalAttribLoc = colourAttribLoc = 0xFFFFFFFF;
    if (positionAttribName != NULL)
        positionAttribLoc = glGetAttribLocation( shaderProgram, positionAttribName );
    if (normalAttribName != NULL)
        normalAttribLoc = glGetAttribLocation( shaderProgram, normalAttribName );
    if (colourAttribName != NULL)
        colourAttribLoc = glGetAttribLocation( shaderProgram, colourAttribName );
}

//setDrawColour
//  Purpose: set a colour with which to draw primitives
//  Preconditions:
//     The elements of a vec3 are set to red, green, and blue values.
//     The value of each channel should be normalized, ie. lie between 0 and 1
//     with 0 being the darkest and 1 being the brightest.
//
// Postconditions:
//     Local variable colour is set to the incoming colour. It will be used as
//     the colour for subsequent draw operations.
void uofrGraphics::setDrawColour(vec3 colour)
{
    this->colour = colour;
}

//setDrawColour
//  Purpose: set a colour with which to draw primitives
//  Preconditions:
//     The elements r,g, and b are set to red, green, and blue values respectively.
//     The value of each channel should be normalized, ie. lie between 0 and 1
//     with 0 being the darkest and 1 being the brightest.
//
// Postconditions:
//     Local variable colour is set to the incoming colour. It will be used as
//     the colour for subsequent draw operations.
void uofrGraphics::setDrawColour(float r, float g, float b)
{
    colour = vec3(r,g,b);
}

//drawWireSphere
//  Purpose: draw a sphere in wire frame style.
//  Preconditions:
//     radius: should be a positive value indicating the desired radius of the
//             sphere
//     slices: should be a positive value indicating how many "slices" you see
//             if you view the sphere from the top.
//     stacks: should be a positive value indicating how many layers there are
//             between the top and bottom of the sphere.
//
//  Postconditions:
//     The vertices for the sphere are sent in GL_LINE_STRIP mode with the
//     desired number of stacks and slices. The sphere has radius "radius" and
//     is centered at (0,0,0).
//     The vertices are stored in buffers that are managed by a vertex array
//     object that are local to drawSolidSphere function.
void uofrGraphics::drawWireSphere(GLfloat radius, GLint slices, GLint stacks)
{
    primitive = GL_LINE_STRIP;
    drawSolidSphere(radius, slices, stacks);
    primitive = GL_TRIANGLES;
}

//drawWireCube
//  Purpose: draw a cube in wire frame style.
//  Preconditions:
//     size: should be a positive value indicating the dimension of one edge of
//           the cube
//
//  Postconditions:
//     The vertices for the cube are sent in GL_LINE_STRIP mode.
//     The cube has sides that measure size units and it centered at (0,0,0).
//     Data for 36 vertices is stored in buffers that are managed by a vertex
//     array object that are local to the drawSolidCube function.
void uofrGraphics::drawWireCube(GLfloat size)
{
    primitive = GL_LINE_STRIP;
    drawSolidCube(size);
    primitive = GL_TRIANGLES;
}

//drawSolidSphere
//  Purpose: draw a sphere with solid filled polygons.
//  Preconditions:
//     radius: should be a positive value indicating the desired radius of the
//             sphere
//     slices: should be a positive value indicating how many "slices" you see
//             if you view the sphere from the top.
//     stacks: should be a positive value indicating how many layers there are
//             between the top and bottom of the sphere.
//
//  Postconditions:
//     the vertices for the sphere are sent in GL_TRIANGLES mode with the
//     desired number of stacks and slices. The sphere has radius "radius" and
//     is centered at (0,0,0).
//     The vertices are stored in buffers that are managed by a vertex array
//     object that are local to this function.
void uofrGraphics::drawSolidSphere(GLfloat radius, GLint slices, GLint stacks)
{
    // Trig utility definitions
#define toSin(a) sinf((a)/360.0f*2.0f*3.141596f)
#define toCos(a) cosf((a)/360.0f*2.0f*3.141596f)
#define toPhi(_s) (_s)*phiStep
#define toRho(_t) (_t)*rhoStep
    
    
    static GLint lastSlices = 0;
    static GLint lastStacks = 0;
    static GLuint lastShaderProg = -1;
    static GLfloat *vertsArray = NULL;
    static GLfloat *normsArray = NULL;
    static GLuint sphere = 0, buffer = 0;
    
    GLint sphereVerts = 6 * (slices * stacks -  slices);
    
    //If the shader changed and we've drawn before,
    //we need to detach from old shader and connect to new.
    if (lastShaderProg != shaderProgram && vertsArray != NULL )
    {
        //connect position and normal arrays to shader
        if (positionAttribLoc != 0XFFFFFFFF)
        {
            glEnableVertexAttribArray( positionAttribLoc );
            glVertexAttribPointer( positionAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(0) );
        }
        
        if (normalAttribLoc != 0xFFFFFFFF)
        {
            glEnableVertexAttribArray( normalAttribLoc );
            glVertexAttribPointer( normalAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(6 * (lastSlices * lastStacks -  lastSlices)*4*sizeof(GLfloat)) );
        }
        
        if (colourAttribLoc != 0xFFFFFFFF)
        {
            //set a constant colour
            glDisableVertexAttribArray(colourAttribLoc);
            glVertexAttrib4fv(colourAttribLoc,colour);
        }
        
    }
    lastShaderProg = shaderProgram;
    
    
    
    
    //Generate a new sphere ONLY if necessary - not the same dimesions as last time
    if (lastSlices != slices || lastStacks != stacks)
    {
        lastSlices = slices;
        lastStacks = stacks;
        GLfloat phiStep = 360.0f/slices;
        GLfloat rhoStep = 180.0f/stacks;
        
        //allocate new memory
        vertsArray = new GLfloat[sphereVerts*4];
        normsArray = new GLfloat[sphereVerts*4];
        
        int i = 0;
        
        //Top (Special because v2 and v3 are always both 0,0)
        for (int s = 0; s < slices; s++)
        {
            int t = 0;
            //Triangle:
            // v1 * (v3)
            //    |\
            //    | \
            // v2 *--* v4
            
            //v1
            normsArray[i] = toSin(toPhi(s)) * toSin(toRho(t));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toPhi(s)) * toSin(toRho(t));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toRho(t));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = 0.0f;
            vertsArray[i++] = 1.0f;
            
            //v2
            normsArray[i] = toSin(toPhi(s)) * toSin(toRho(t+1));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toPhi(s)) * toSin(toRho(t+1));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toRho(t+1));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = 0.0f;
            vertsArray[i++] = 1.0f;
            
            //v4
            normsArray[i] = toSin(toPhi(s+1)) * toSin(toRho(t+1));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toPhi(s+1)) * toSin(toRho(t+1));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toRho(t+1));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = 0.0f;
            vertsArray[i++] = 1.0f;
        }
        
        //Body of sphere
        for (int t = 1; t < stacks - 1; t++)
        {
            for (int s = 0; s < slices; s++)
            {
                //Triangle 1:
                // v1 *  * v3
                //    |\
                //    | \
                // v2 *__* v4
                
                
                //v1
                normsArray[i] = toSin(toPhi(s)) * toSin(toRho(t));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i]  = toCos(toPhi(s)) * toSin(toRho(t));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i]  = toCos(toRho(t));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = 0.0f;
                vertsArray[i++] = 1.0f;
                
                //v2
                normsArray[i] = toSin(toPhi(s)) * toSin(toRho(t+1));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = toCos(toPhi(s)) * toSin(toRho(t+1));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = toCos(toRho(t+1));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = 0.0f;
                vertsArray[i++] = 1.0f;
                
                //v4
                normsArray[i] = toSin(toPhi(s+1)) * toSin(toRho(t+1));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = toCos(toPhi(s+1)) * toSin(toRho(t+1));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = toCos(toRho(t+1));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = 0.0f;
                vertsArray[i++] = 1.0f;
                
                
                //Triangle 2:
                // v1 *--* v3
                //     \ |
                //      \|
                // v2 *  * v4
                
                
                //v4
                normsArray[i] = toSin(toPhi(s+1)) * toSin(toRho(t+1));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = toCos(toPhi(s+1)) * toSin(toRho(t+1));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = toCos(toRho(t+1));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = 0.0f;
                vertsArray[i++] = 1.0f;
                
                
                //v3
                normsArray[i] = toSin(toPhi(s+1)) * toSin(toRho(t));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = toCos(toPhi(s+1)) * toSin(toRho(t));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = toCos(toRho(t));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = 0.0f;
                vertsArray[i++] = 1.0f;
                
                //v1
                normsArray[i] = toSin(toPhi(s)) * toSin(toRho(t));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i]  = toCos(toPhi(s)) * toSin(toRho(t));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i]  = toCos(toRho(t));
                vertsArray[i++] = normsArray[i] * radius;
                normsArray[i] = 0.0f;
                vertsArray[i++] = 1.0f;
                
            }
        }
        
        
        //Bottom (Special because v2 and v4 are always both 180,180)
        for (int s = 0; s < slices; s++)
        {
            int t = stacks-1;
            //Triangle:
            // v1 *--* v3
            //    | /
            //    |/
            // v2 *  * v4
            
            //v1
            normsArray[i] = toSin(toPhi(s)) * toSin(toRho(t));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toPhi(s)) * toSin(toRho(t));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] =   toCos(toRho(t));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = 0.0f;
            vertsArray[i++] = 1.0f;
            
            //v2
            normsArray[i] = toSin(toPhi(s)) * toSin(toRho(t+1));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toPhi(s)) * toSin(toRho(t+1));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toRho(t+1));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = 0.0f;
            vertsArray[i++] = 1.0f;
            
            //v3
            normsArray[i] = toSin(toPhi(s+1)) * toSin(toRho(t));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toPhi(s+1)) * toSin(toRho(t));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = toCos(toRho(t));
            vertsArray[i++] = normsArray[i] * radius;
            normsArray[i] = 0.0f;
            vertsArray[i++] = 1.0f;
        }
        
        if (!glIsVertexArray(sphere))
        {
            glGenVertexArrays( 1, &sphere );
        }
        
        glBindVertexArray( sphere );
        
        if (glIsBuffer(buffer))
        {
            glDeleteBuffers(1, &buffer);
        }
        glGenBuffers( 1, &buffer );
        glBindBuffer( GL_ARRAY_BUFFER, buffer );
        glBufferData( GL_ARRAY_BUFFER, sphereVerts*4*sizeof(GLfloat)*2,
                     NULL, GL_STATIC_DRAW );
        glBufferSubData( GL_ARRAY_BUFFER, 0, sphereVerts*4*sizeof(GLfloat), vertsArray );
        glBufferSubData( GL_ARRAY_BUFFER, sphereVerts*4*sizeof(GLfloat), sphereVerts*4*sizeof(GLfloat), normsArray );
        
        //connect position and normal arrays to shader
        if (positionAttribLoc != 0XFFFFFFFF)
        {
            glEnableVertexAttribArray( positionAttribLoc );
            glVertexAttribPointer( positionAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(0) );
        }
        
        if (normalAttribLoc != 0xFFFFFFFF)
        {
            glEnableVertexAttribArray( normalAttribLoc );
            glVertexAttribPointer( normalAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(sphereVerts*4*sizeof(GLfloat)) );
        }
        
        if (colourAttribLoc != 0xFFFFFFFF)
        {
            //set a constant colour
            glDisableVertexAttribArray(colourAttribLoc);
        }
        
        
        delete [] vertsArray;
        delete [] normsArray;
        
    }
    
    glBindVertexArray(sphere);
    glVertexAttrib4fv(colourAttribLoc,colour);
    glDrawArrays(primitive, 0, sphereVerts);
}

//drawWireCube
//  Purpose: draw a cube in filled polygon style.
//  Preconditions:
//     size: should be a positive value indicating the dimension of one edge of
//           the cube
//
//  Postconditions:
//     The vertices for the cube are sent in GL_TRIANGLES mode.
//     The cube has sides that measure size units and it centered at (0,0,0).
//     Data for 36 vertices is stored in buffers that are managed by a vertex
//     array object that are local to this function.
void uofrGraphics::drawSolidCube(GLfloat size)
{
    
    static GLfloat lastSize = 0;
    static GLuint lastShaderProg;
    
    const vec4 cubeVerts[] =
    {
        vec4( 0.5, 0.5, 0.5, 1), //0
        vec4( 0.5, 0.5,-0.5, 1), //1
        vec4( 0.5,-0.5, 0.5, 1), //2
        vec4( 0.5,-0.5,-0.5, 1), //3
        vec4(-0.5, 0.5, 0.5, 1), //4
        vec4(-0.5, 0.5,-0.5, 1), //5
        vec4(-0.5,-0.5, 0.5, 1), //6
        vec4(-0.5,-0.5,-0.5, 1)  //7
    };
    
    const vec4 verts[] = //36 vertices total
    {
        cubeVerts[0], cubeVerts[4], cubeVerts[6],  //front
        cubeVerts[6], cubeVerts[2], cubeVerts[0],
        cubeVerts[1], cubeVerts[0], cubeVerts[2],  //right
        cubeVerts[2], cubeVerts[3], cubeVerts[1],
        cubeVerts[5], cubeVerts[1], cubeVerts[3],  //back
        cubeVerts[3], cubeVerts[7], cubeVerts[5],
        cubeVerts[4], cubeVerts[5], cubeVerts[7],  //left
        cubeVerts[7], cubeVerts[6], cubeVerts[4],
        cubeVerts[4], cubeVerts[0], cubeVerts[1],  //top
        cubeVerts[1], cubeVerts[5], cubeVerts[4],
        cubeVerts[6], cubeVerts[7], cubeVerts[3],  //bottom
        cubeVerts[3], cubeVerts[2], cubeVerts[6],
        
    };
    
    
    const vec4 right =   vec4( 1.0f, 0.0f, 0.0f, 0.0f);
    const vec4 left = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
    const vec4 top =  vec4( 0.0f, 1.0f, 0.0f, 0.0f);
    const vec4 bottom =  vec4( 0.0f,-1.0f, 0.0f, 0.0f);
    const vec4 front =   vec4( 0.0f, 0.0f, 1.0f, 0.0f);
    const vec4 back = vec4( 0.0f, 0.0f,-1.0f, 0.0f);
    
    const vec4 normsArray[] =
    {
        front, front, front, front, front, front,
        right, right, right, right, right, right,
        back, back, back, back, back, back,
        left, left, left, left, left, left,
        top, top, top, top, top, top,
        bottom, bottom, bottom, bottom, bottom, bottom
        
    };
    
    static GLuint cube = 0, buffer = 0;
    
    //If the shader changed and we've drawn before,
    //we need to detach from old shader and connect to new.
    if (lastShaderProg != shaderProgram && lastSize != 0)
    {
        //connect position and normal arrays to shader
        if (positionAttribLoc != 0XFFFFFFFF)
        {
            glEnableVertexAttribArray( positionAttribLoc );
            glVertexAttribPointer( positionAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(0) );
        }
        
        if (normalAttribLoc != 0xFFFFFFFF)
        {
            glEnableVertexAttribArray( normalAttribLoc );
            glVertexAttribPointer( normalAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(36*sizeof(vec4)) );
        }
        
        if (colourAttribLoc != 0xFFFFFFFF)
        {
            //set a constant colour
            glDisableVertexAttribArray(colourAttribLoc);
            glVertexAttrib4fv(colourAttribLoc,colour);
        }
    }
    lastShaderProg = shaderProgram;
    
    //Generate a new cube ONLY if necessary - not the same dimesions as last time
    if (lastSize != size)
    {
        vec4 vertsArray[36];
        lastSize = size;
        
        for(int i = 0; i < 36; i++)
        {
            vertsArray[i] = verts[i] * size;
            vertsArray[i].w = 1.0f;
        }
        
        if (!glIsVertexArray(cube))
        {
            glGenVertexArrays( 1, &cube );
        }
        
        glBindVertexArray( cube );
        
        if (glIsBuffer(buffer))
        {
            glDeleteBuffers(1, &buffer);
        }
        glGenBuffers( 1, &buffer );
        glBindBuffer( GL_ARRAY_BUFFER, buffer );
        glBufferData( GL_ARRAY_BUFFER, 36*sizeof(vec4)*2,
                     NULL, GL_STATIC_DRAW );
        glBufferSubData( GL_ARRAY_BUFFER, 0, 36*sizeof(vec4), vertsArray );
        glBufferSubData( GL_ARRAY_BUFFER, 36*sizeof(vec4), 36*sizeof(vec4), normsArray );
        
        //connect position and normal arrays to shader
        if (positionAttribLoc != 0XFFFFFFFF)
        {
            glEnableVertexAttribArray( positionAttribLoc );
            glVertexAttribPointer( positionAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(0) );
        }
        
        if (normalAttribLoc != 0xFFFFFFFF)
        {
            glEnableVertexAttribArray( normalAttribLoc );
            glVertexAttribPointer( normalAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(36*sizeof(vec4)) );
        }
        
        if (colourAttribLoc != 0xFFFFFFFF)
        {
            //set a constant colour
            glDisableVertexAttribArray(colourAttribLoc);
        }
    }
    
    glBindVertexArray(cube);
    glVertexAttrib4fv(colourAttribLoc,colour);
    glDrawArrays(primitive, 0, 36);
}

//drawQuad
//  Purpose: draw a quadrilateral in filled polygon style.
//  Preconditions:
//     v1, v2, v3, v4: are vertices that are arranged in "counter clockwise"
//                     order. The quadrilateral is assumed to be flat.
//
//  Postconditions:
//     The vertices for the quadrilateral are sent in GL_TRIANGLES mode.
//     The normal is calculated from three of the vertices.
//     Data for 6 vertices are stored in buffers that are managed by a vertex
//     array object that are local to this function.
void uofrGraphics::drawQuad(vec4 v1, vec4 v2, vec4 v3, vec4 v4)
{
    
    static vec4 normal;
    static GLuint lastShaderProg;
    
    static GLuint quad = 0, buffer = 0;
    
    
    //update quad's data every time. People rarely draw the same quad repeatedly
    //(Lab 1 is an exception... we draw and translate a square... lol!)
    
    
    if (!glIsVertexArray(quad))
    {
        glGenVertexArrays( 1, &quad );
    }
    
    glBindVertexArray( quad );
    
    if (!glIsBuffer(buffer))
    {
        glGenBuffers( 1, &buffer );
        glBindBuffer( GL_ARRAY_BUFFER, buffer );
        glBufferData( GL_ARRAY_BUFFER, 6*sizeof(vec4), NULL, GL_STREAM_DRAW );
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferSubData( GL_ARRAY_BUFFER, 0*sizeof(vec4), sizeof(vec4), v1 );
    glBufferSubData( GL_ARRAY_BUFFER, 1*sizeof(vec4), sizeof(vec4), v2 );
    glBufferSubData( GL_ARRAY_BUFFER, 2*sizeof(vec4), sizeof(vec4), v3 );
    glBufferSubData( GL_ARRAY_BUFFER, 3*sizeof(vec4), sizeof(vec4), v3 );
    glBufferSubData( GL_ARRAY_BUFFER, 4*sizeof(vec4), sizeof(vec4), v4 );
    glBufferSubData( GL_ARRAY_BUFFER, 5*sizeof(vec4), sizeof(vec4), v1 );
    
    normal = vec4(normalize(cross(v3-v2, v1-v2)),0);
    
    //Link to shader on first draw, and if shader changed
    if (lastShaderProg != shaderProgram)
    {
        //connect position and normal arrays to shader
        if (positionAttribLoc != 0XFFFFFFFF)
        {
            glEnableVertexAttribArray( positionAttribLoc );
            glVertexAttribPointer( positionAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(0) );
        }
        
        if (normalAttribLoc != 0xFFFFFFFF)
        {
            glDisableVertexAttribArray( normalAttribLoc );
        }
        
        if (colourAttribLoc != 0xFFFFFFFF)
        {
            //set a constant colour
            glDisableVertexAttribArray(colourAttribLoc);
        }
    }
    lastShaderProg = shaderProgram;
    
    glVertexAttrib4fv(colourAttribLoc,colour);
    glVertexAttrib4fv(normalAttribLoc, normal);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}



/*
 * drawSolidTorus
 *
 * Based on freeGLUT glut*Torus code by John Fay
 * Modified by Alex Clarke, Feb. 2012
 * Made suitable for this library by Alex Clarke, May 2013
 *
 * Precondiitons:
 *    innerRadius: inner radius of the torus
 *    outerRadius: outer radius of the torus
 *    nSides: number of segments in cross section of torus
 *    nRings: number of segments around the torus
 *
 * Postconditions:
 *    The vertices for a torus are sent in GL_TRIANGLES mode.
 *    The torus has the dimensions and divisions requested and it centered at
 *    (0,0,0).
 *    The vertices are stored in buffers that are managed by a vertex array
 *    object that are local to this function.
 */

void uofrGraphics::drawSolidTorus( GLfloat innerRadius, GLfloat outerRadius, GLint nSides, GLint nRings)
{
    
	
    GLfloat  iradius = innerRadius, oradius = outerRadius, phi, psi, dpsi, dphi;
	vec4 *vertex= NULL, *normal=NULL, *vTris=NULL, *nTris=NULL;
    static GLuint torus;
	int    i, j;
    static int verts = 0;
	float spsi, cpsi, sphi, cphi ;
    
    static GLint lastSides = 0, lastRings = 0;
    static GLfloat lastIradius = 0, lastOradius = 0;
    static GLuint lastShaderProg = -1;
    static GLuint buffer = 0;
    nSides++;
    nRings++;
    if ( nSides < 1 ) nSides = 1;
	if ( nRings < 1 ) nRings = 1;
    
	/* Calculate sizes of the data arrays */
	int nVerts = (nSides) * (nRings);
	int triVerts =  2 * 3 * (nSides-1) * (nRings-1);
    
    
    //If the shader changed and we've drawn before,
    //we need to detach from old shader and connect to new.
    if (lastShaderProg != shaderProgram && vertex != NULL )
    {
        //connect position and normal arrays to shader
        if (positionAttribLoc != 0XFFFFFFFF)
        {
            glEnableVertexAttribArray( positionAttribLoc );
            glVertexAttribPointer( positionAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(0) );
        }
        
        if (normalAttribLoc != 0xFFFFFFFF)
        {
            glEnableVertexAttribArray( normalAttribLoc );
            glVertexAttribPointer( normalAttribLoc, 4, GL_FLOAT, GL_FALSE, 0,
                                  BUFFER_OFFSET(sizeof(vec4)*triVerts) );
        }
        
        if (colourAttribLoc != 0xFFFFFFFF)
        {
            //set a constant colour
            glDisableVertexAttribArray(colourAttribLoc);
            glVertexAttrib4fv(colourAttribLoc,colour);
        }
        
    }
    lastShaderProg = shaderProgram;
    
    
    if (nSides != lastSides || nRings != lastRings || iradius != lastIradius  || oradius != lastOradius)
    {
        lastSides = nSides;
        lastRings = nRings;
        lastIradius = iradius;
        lastOradius = oradius;
        vertex = new vec4[nVerts];
        normal = new vec4[nVerts];
        vTris = new vec4[triVerts];
        nTris = new vec4[triVerts];
        
        
        
        dpsi =  2.0f * (GLfloat)M_PI / (GLfloat)(nRings-1) ;
        dphi = -2.0f * (GLfloat)M_PI / (GLfloat)(nSides-1) ;
        psi  = 0.0f;
        
        //Generate points
        for( j=0; j<nRings; j++ )
        {
            cpsi = cos ( psi ) ;
            spsi = sin ( psi ) ;
            phi = 0.0f;
            
            for( i=0; i<nSides; i++ )
            {
                int offset = ( j * nSides + i ) ;
                cphi = cos ( phi ) ;
                sphi = sin ( phi ) ;
                vertex[offset].x = cpsi * ( oradius + cphi * iradius ) ;
                vertex[offset].y = spsi * ( oradius + cphi * iradius ) ;
                vertex[offset].z = sphi * iradius  ;
                vertex[offset].w = 1 ;
                normal[offset].x = cpsi * cphi ;
                normal[offset].y = spsi * cphi ;
                normal[offset].z = sphi ;
                normal[offset].w = 0 ;
                phi += dphi;
            }
            
            psi += dpsi;
        }
        
        int v = 0;
        //Process Quads for faces
        for( i=0; i<nSides-1; i++ )
        {
            for( j=0; j<nRings-1; j++ )
            {
                int offset = ( j * nSides + i ) ;
                //Break Faces into two triangles
                //Triangle 1
                nTris[v] = normal[ offset];
                vTris[v++] = vertex[ offset];
                
                nTris[v] = normal[ offset + 1];
                vTris[v++] = vertex[ offset + 1];
                
                nTris[v] = normal[ offset + nSides + 1];
                vTris[v++] = vertex[ offset +  nSides + 1];
                
                //Triangle 2
                nTris[v] = normal[ offset + nSides];
                vTris[v++] = vertex[ offset + nSides];
                
                nTris[v] = normal[ offset ];
                vTris[v++] = vertex[ offset ];
                
                nTris[v] = normal[ offset +  nSides + 1 ];
                vTris[v++] = vertex[ offset +  nSides + 1 ];
            }
            
        }
        
        //Load data into a vertex buffer
        if (!glIsVertexArray(torus))
        {
            glGenVertexArrays( 1, &torus );
        }
        
        glBindVertexArray( torus );
        
        if (glIsBuffer(buffer))
        {
            glDeleteBuffers(1, &buffer);
        }
        
        glGenBuffers( 1, &buffer );
        glBindBuffer( GL_ARRAY_BUFFER, buffer );
        glBufferData( GL_ARRAY_BUFFER, 2*sizeof(vec4)*triVerts, NULL, GL_STREAM_DRAW );
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vec4)*triVerts, vTris );
        glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*triVerts, sizeof(vec4)*triVerts, nTris );
        
        
        //connect position and normal arrays to shader
        if (positionAttribLoc != 0XFFFFFFFF)
        {
            glEnableVertexAttribArray( positionAttribLoc );
            glVertexAttribPointer( positionAttribLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
        }
        
        if (normalAttribLoc != 0xFFFFFFFF)
        {
            glEnableVertexAttribArray( normalAttribLoc );
            glVertexAttribPointer( normalAttribLoc,   4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4)*triVerts) );
            
        }
        
        if (colourAttribLoc != 0xFFFFFFFF)
        {
            //set a constant colour
            glDisableVertexAttribArray(colourAttribLoc);
        }
        
        
        
        
        free ( vertex );
        free ( normal );
        free ( nTris );
        free ( vTris );
        verts = triVerts;
    }
    
    glBindVertexArray(torus);
    glVertexAttrib4fv(colourAttribLoc,colour);
    glDrawArrays(primitive, 0, verts);
}

void uofrGraphics::drawWireTorus( GLfloat innerRadius, GLfloat outerRadius, GLint nSides, GLint nRings)
{
    primitive = GL_LINE_STRIP;
    drawSolidTorus(innerRadius, outerRadius, nSides, nRings);
    primitive = GL_TRIANGLES;
}