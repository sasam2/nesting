#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "global.h"

using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::ends;

extern GLdouble vertices[64][6];               // arrary to store newly created vertices (x,y,z,r,g,b) by combine callback
extern int vertexIndex;
extern int handle;
extern int stockList;
extern GLfloat *blue;

// CALLBACK functions for GLU_TESS ////////////////////////////////////////////
// NOTE: must be declared with CALLBACK directive
void CALLBACK tessBeginCB(GLenum which);
void CALLBACK tessEndCB();
void CALLBACK tessErrorCB(GLenum errorCode);
void CALLBACK tessVertexCB(const GLvoid *data);
void CALLBACK tessVertexCB2(const GLvoid *data);
void CALLBACK tessCombineCB(const GLdouble newVertex[3], const GLdouble *neighborVertex[4],
                            const GLfloat neighborWeight[4], GLdouble **outData);



// GLUT CALLBACK functions ////////////////////////////////////////////////////
void reshapeCB(int w, int h);
void timerCB(int millisec);
void idleCB();
bool initSharedMem();
void clearSharedMem();
//void keyboardCB(unsigned char key, int x, int y);
//void mouseCB(int button, int stat, int x, int y);
//void mouseMotionCB(int x, int y);
int generatePolygonDisplayList(vector<Polygon_2> p);
int generatePolygonWHolesDisplayList(CGAL::Polygon_with_holes_2<Kernel> p);
