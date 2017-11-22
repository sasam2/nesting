#include "OpenglCallbacks.h"


void reshapeCB(int w, int h)
{
    // set viewport to be the entire window
    //glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	
	GLUI_Master.auto_set_viewport();
	glutPostRedisplay();
}

void timerCB(int millisec)
{
    glutTimerFunc(millisec, timerCB, millisec);
    glutPostRedisplay();
}

void idleCB()
{
  if ( glutGetWindow() != handle )
    glutSetWindow(handle);
  glutPostRedisplay();
}

bool initSharedMem()
{
    //mouseLeftDown = mouseRightDown = false;
    return true;
}

void clearSharedMem()
{
	glDeleteBuffers(1, (GLuint*)&stockList);
	delete[] blue;
}

/*
void keyboardCB(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27: // ESCAPE
        clearSharedMem();
        exit(0);
        break;

    case ' ':
        break;

    case 'd': // switch rendering modes (fill -> wire -> point)
    case 'D':
        drawMode = ++drawMode % 3;
        if(drawMode == 0)        // fill mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }
        else if(drawMode == 1)  // wireframe mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else                    // point mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        break;

    default:
        ;
    }

    glutPostRedisplay();
}


void mouseCB(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;
    }

    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }
}


void mouseMotionCB(int x, int y)
{
    if(mouseLeftDown)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if(mouseRightDown)
    {
        cameraDistance += (y - mouseY) * 0.2f;
        mouseY = y;
    }

    //glutPostRedisplay();
}
*/


///////////////////////////////////////////////////////////////////////////////
// GLU_TESS CALLBACKS
///////////////////////////////////////////////////////////////////////////////
void CALLBACK tessBeginCB(GLenum which)
{
    glBegin(which);

    // DEBUG //
    //ss << "glBegin(" << getPrimitiveType(which) << ");\n";
}

void CALLBACK tessEndCB()
{
    glEnd();

    // DEBUG //
    //ss << "glEnd();\n";
}

void CALLBACK tessVertexCB(const GLvoid *data)
{
    // cast back to double type
    const GLdouble *ptr = (const GLdouble*)data;

    glVertex3dv(ptr);

    // DEBUG //
    //ss << "  glVertex3d(" << *ptr << ", " << *(ptr+1) << ", " << *(ptr+2) << ");\n";
}

void CALLBACK tessVertexCB2(const GLvoid *data)
{
    // cast back to double type
    const GLdouble *ptr = (const GLdouble*)data;

    glColor3dv(ptr+3);
    glVertex3dv(ptr);

    // DEBUG //
    //ss << "  glColor3d(" << *(ptr+3) << ", " << *(ptr+4) << ", " << *(ptr+5) << ");\n";
    //ss << "  glVertex3d(" << *ptr << ", " << *(ptr+1) << ", " << *(ptr+2) << ");\n";
}

void CALLBACK tessCombineCB(const GLdouble newVertex[3], const GLdouble *neighborVertex[4],
                            const GLfloat neighborWeight[4], GLdouble **outData)
{
    // copy new intersect vertex to local array
    // Because newVertex is temporal and cannot be hold by tessellator until next
    // vertex callback called, it must be copied to the safe place in the app.
    // Once gluTessEndPolygon() called, then you can safly deallocate the array.
    vertices[vertexIndex][0] = newVertex[0];
    vertices[vertexIndex][1] = newVertex[1];
    vertices[vertexIndex][2] = newVertex[2];

    // compute vertex color with given weights and colors of 4 neighbors
    // the neighborVertex[4] must hold required info, in this case, color.
    // neighborVertex was actually the third param of gluTessVertex() and is
    // passed into here to compute the color of the intersect vertex.
    vertices[vertexIndex][3] = neighborWeight[0] * neighborVertex[0][3] +   // red
                               neighborWeight[1] * neighborVertex[1][3] +
                               neighborWeight[2] * neighborVertex[2][3] +
                               neighborWeight[3] * neighborVertex[3][3];
    vertices[vertexIndex][4] = neighborWeight[0] * neighborVertex[0][4] +   // green
                               neighborWeight[1] * neighborVertex[1][4] +
                               neighborWeight[2] * neighborVertex[2][4] +
                               neighborWeight[3] * neighborVertex[3][4];
    vertices[vertexIndex][5] = neighborWeight[0] * neighborVertex[0][5] +   // blue
                               neighborWeight[1] * neighborVertex[1][5] +
                               neighborWeight[2] * neighborVertex[2][5] +
                               neighborWeight[3] * neighborVertex[3][5];


    // return output data (vertex coords and others)
    *outData = vertices[vertexIndex];   // assign the address of new intersect vertex

    ++vertexIndex;  // increase index for next vertex
}

void CALLBACK tessErrorCB(GLenum errorCode)
{
    const GLubyte *errorStr;

    errorStr = gluErrorString(errorCode);
    cerr << "[ERROR]: " << errorStr << endl;
}

int generatePolygonDisplayList(vector<Polygon_2> p){

	GLuint id = glGenLists(1);  // create a display list
    if(!id) return id;          // failed to create a list, return 0
	
	glNewList(id, GL_COMPILE);
	for (int i=0; i < p.size(); i++)
	{
		glBegin(GL_POLYGON);
		for (Polygon_2::Vertex_iterator hit = p[i].vertices_begin(); hit != p[i].vertices_end(); ++hit)
		{
			glVertex2d(	CGAL::to_double(hit->x()), 
						CGAL::to_double(hit->y()));
		}
		glEnd();
	}
	glEndList();

    return id;					// return handle ID of a display list
}
int generatePolygonWHolesDisplayList(CGAL::Polygon_with_holes_2<Kernel> p){

	GLuint id = glGenLists(1);  // create a display list
    if(!id) return id;          // failed to create a list, return 0

    GLUtesselator *tess = gluNewTess(); // create a tessellator
    if(!tess) return 0;  // failed to create tessellation object, return 0

    // register callback functions
    // This polygon is self-intersecting, so GLU_TESS_COMBINE callback function
    // must be registered. The combine callback will process the intersecting vertices.
    gluTessCallback(tess, GLU_TESS_BEGIN, (void (__stdcall*)(void))tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (__stdcall*)(void))tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (__stdcall*)(void))tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (__stdcall*)(void))tessVertexCB);
    
    // tessellate and compile a concave quad into display list
    // Pay attention to winding rules if multiple contours are overlapped.
    // The winding rules determine which parts of polygon will be filled(interior)
    // or not filled(exterior). For each enclosed region partitioned by multiple
    // contours, tessellator assigns a winding number to the region by using
    // given winding rule. The default winding rule is GLU_TESS_WINDING_ODD,
    // but, here we are using non-zero winding rule to fill the middle area.
    // BTW, the middle region will not be filled with the odd winding rule.
    // gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
	glNewList(id, GL_COMPILE);
	// glColor4f(color[0],color[1],color[2],color[3]);
	gluTessBeginPolygon(tess, 0); 

	gluTessBeginContour(tess); 
	for (CGAL::Polygon_2<Kernel>::Vertex_iterator vit = p.outer_boundary().vertices_begin(); vit != p.outer_boundary().vertices_end(); ++vit)
	{
		double* v=createPosition(CGAL::to_double(vit->x()), 
									CGAL::to_double(vit->y()),
									0.0);
		gluTessVertex(tess, v, v);
	}

	gluTessEndContour(tess);

	for (CGAL::Polygon_with_holes_2<Kernel>::Hole_iterator hit = p.holes_begin(); hit != p.holes_end(); ++hit)
	{

		gluTessBeginContour(tess); 
		for (CGAL::Polygon_2<Kernel>::Vertex_iterator vit = hit->vertices_begin(); vit != hit->vertices_end(); ++vit)
		{
			double* v=createPosition(CGAL::to_double(vit->x()), 
									 CGAL::to_double(vit->y()),
									 0.0);
			gluTessVertex(tess, v, v);
		}
		gluTessEndContour(tess);
	}

	gluTessEndPolygon(tess);
	glEndList();
    gluDeleteTess(tess);        // safe to delete after tessellation
    return id;					// return handle ID of a display list
}
