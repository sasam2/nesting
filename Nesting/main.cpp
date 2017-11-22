#include "OpenglCallbacks.h"
#include "PreProcessing.h"
#include "algorithms.h"

#ifndef CALLBACK
#define CALLBACK
#endif

//#define DEBUG


// function declarations //////////////////////////////////////////////////////
void displayCB();
void initGL();
int  initGLUT(int argc, char **argv);

// global variables ///////////////////////////////////////////////////////////
//tesselation
GLdouble vertices[64][6];               //"OpenglCalbacks.h" arrary to store newly created vertices (x,y,z,r,g,b) by combine callback
int vertexIndex = 0;                    //"OpenglCalbacks.h" array index for above array incremented inside combine callback
int tx, ty, tw, th;
//opengl
int handle; //"OpenglCalbacks.h"

GLUI_EditText *edittext_file; //"PreProcessing.h"
GLUI_RadioGroup *radiogroup_heuristic; //"PreProcessing.h"
GLUI_RadioGroup *radiogroup_staticheuristic; //"PreProcessing.h"
GLUI_RadioGroup *radiogroup_nfps; //"PreProcessing.h"
vector<Polygon_2> (*nfps)(vector<Polygon_2>, vector<Polygon_2>);//"PreProcessing.h"

//Load problem
char problemName[1024]="shapes_config.txt"; //"shapes_config.txt"; //"PreProcessing.h" "algorithms.h"
Layout layout; //"PreProcessing.h" "algorithms.h"

//Positioning Algorithms
int saveAll;			//"PreProcessing.h"
int staticHeuristic;	//"PreProcessing.h" "algorithms.h"

//static vars
list<Point_2> piecesOrdered;	//"PreProcessing.h" "algorithms.h"

//dynamic vars
map<Point_2, GLuint> layoutNFPsTest;	//"PreProcessing.h" "algorithms.h" nfps dos layouts possiveis na proxima iteracao
map<Point_2, map<Point_2, DrawingWithRotations>> drawingNFPsTest;	//"PreProcessing.h" "açgorithms.h" nfps do layout actual
map<Point_2, DrawingWithRotations> drawingPolysTest;				//"algorithms.h" poligonos do layout actual

//static and dynamic vars
int iteration; //"algorithms.h"
vector<int> piecesAvaliability; //"algorithms.h"
int stockList;
GLfloat *blue;

Polygon_with_holes_2 **polygons; //"algorithms.h" "PreProcessing.h"
int numberOfPolygons; //"algorithms.h" "PreProcessing.h"
int nrOfRotations;

DrawingWithRotations currentDrawingPolys;				//"algorithms.h"
map<Point_2, DrawingWithRotations> currentDrawingNFPs;	//"algorithms.h"

//debug test files
ofstream testfile;


// functions /////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    initSharedMem();

    // init GLUT and GL
    initGLUT(argc, argv);
	if (GLEW_OK != glewInit()) return 1;
    initGL();

	blue=createColor(0.0,0.0,1.0,0.25);
    // the last GLUT call (LOOP)
    // window will be shown and display callback is triggered by events
    // NOTE: this call never return main().
    glutMainLoop();

    return 0;
}

int initGLUT(int argc, char **argv)
{
    // GLUT stuff for windowing
    // initialization openGL window.
    // it is called before any other GLUT routine
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode

    glutInitWindowSize(500, 200);               // window size

    glutInitWindowPosition(100, 100);           // window location

    // finally, create a window with openGL context
    // Window will not displayed until glutMainLoop() is called
    // it returns a unique ID
    handle = glutCreateWindow(argv[0]);     // param is the title of window

    // register GLUT callback functions
    glutDisplayFunc(displayCB);
    glutTimerFunc(33, timerCB, 33);             // redraw only every given millisec
    
	//glutIdleFunc(idleCB);                       // redraw only every given millisec
	//glutReshapeFunc(reshapeCB);
	
	GLUI_Master.set_glutIdleFunc( idleCB );
	GLUI_Master.set_glutReshapeFunc( reshapeCB );
	
	GLUI *glui = GLUI_Master.create_glui_subwindow( handle, GLUI_SUBWINDOW_BOTTOM );
														/*	GLUI_SUBWINDOW_RIGHT
															GLUI_SUBWINDOW_BOTTOM */
	glui->set_main_gfx_window( handle );
	
	GLUI_Panel *panel_nfps = glui->add_panel("Calculo nfps:");
	radiogroup_nfps = glui->add_radiogroup_to_panel(panel_nfps, &saveAll);
	GLUI_RadioButton *radiobutton_slp = glui->add_radiobutton_to_group(radiogroup_nfps, "Diagrama de declives");
	GLUI_RadioButton *radiobutton_mk = glui->add_radiobutton_to_group(radiogroup_nfps, "Somas de Minkowski");

	GLUI_Panel *panel_heuristic = glui->add_panel("Heuristica:");
	radiogroup_heuristic = glui->add_radiogroup_to_panel(panel_heuristic, &saveAll, -1,heuristicCB);
	GLUI_RadioButton *radiobutton_static = glui->add_radiobutton_to_group(radiogroup_heuristic, "Estatica");
	GLUI_RadioButton *radiobutton_dynamic = glui->add_radiobutton_to_group(radiogroup_heuristic, "Dinamica");


	
	//WIDER 1
	//HIGHER 2
	//LARGER 3
	//MORE_IRREGULAR 4
	//MORE_RECTANGULAR 5
	//RANDOM 6

	glui->add_column( true );
	GLUI_Panel *panel_staticheuristic = glui->add_panel("Heuristica estaticas:");
	radiogroup_staticheuristic = glui->add_radiogroup_to_panel(panel_staticheuristic, &saveAll);
	GLUI_RadioButton *radiobutton_wider = glui->add_radiobutton_to_group(radiogroup_staticheuristic, "Largura");
	GLUI_RadioButton *radiobutton_higher = glui->add_radiobutton_to_group(radiogroup_staticheuristic, "Altura");
	GLUI_RadioButton *radiobutton_larger = glui->add_radiobutton_to_group(radiogroup_staticheuristic, "Tamanho");
	GLUI_RadioButton *radiobutton_irregular = glui->add_radiobutton_to_group(radiogroup_staticheuristic, "Irregularidade");
	GLUI_RadioButton *radiobutton_rectangular = glui->add_radiobutton_to_group(radiogroup_staticheuristic, "Rectangularidade");
	GLUI_RadioButton *radiobutton_random = glui->add_radiobutton_to_group(radiogroup_staticheuristic, "Aleatoria");
	
	glui->add_column( true );
	GLUI_Panel *panel_file = glui->add_panel("Ficheiro:");
	edittext_file = glui->add_edittext_to_panel(panel_file, "Nome", GLUI_EDITTEXT_TEXT, problemName);
	
	GLUI_Button *botaoUndo= glui->add_button("Resolver", 1, solveCB);
	
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );

    return handle;
}

void initGL()
{
    glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

    // enable /disable features
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

     // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(0, 0, 0, 0);                   // background color
    glClearStencil(0);                          // clear stencil buffer
    glClearDepth(1.0f);                         // 0 is near, 1 is far
    glDepthFunc(GL_LEQUAL);

    //initLights();
    //setCamera(0, 0, 5, 0, 0, 0);
}

void displayCB()
{
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // save the initial ModelView matrix before modifying ModelView matrix
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_BLEND);

	if(currentDrawingNFPs.size()>0)
	{
		if(saveAll){//Programaçao Dinamica
#ifdef DEBUG
			displayDynamic(&testfile);
#else
			buildDynamicLayout();
#endif		
		}else{		//Criterios Estáticos
#ifdef DEBUG
			if(!testfile.is_open()){
				char filename[100];
				sprintf(filename, "staticTests %s - %d.csv", problemName, staticHeuristic);
				testfile.open (filename);
				testfile<<"Iteration; Placement time ; Updating time"<<endl;
			}
			displayStatic(&testfile);
#else
			buildStaticLayout();
#endif
		}
	}
#ifdef DEBUG
	else
	{
		if(testfile.is_open())
			testfile.close();
	}
#endif	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	BoundingBox layoutBB = currentDrawingPolys.getViewPort();
	gluOrtho2D(layoutBB.getX(), layoutBB.getWidth(), layoutBB.getY(), layoutBB.getHeight());	// set to orthogonal projection
	glMatrixMode(GL_MODELVIEW);																	// switch to modelview matrix

	glColor3f(1.0, 1.0, 0.0);
	for(int i=0; i < currentDrawingPolys.getPositions().size(); i++)
	{
		glPushMatrix();
		//glColor4f(currentDrawingPolys[i].getColors()[i][0], currentDrawingPolys[i].getColors()[i][1], currentDrawingPolys[i].getColors()[i][2], currentDrawingPolys[i].getColors()[i][3]);
		glTranslatef(currentDrawingPolys.getPositions()[i][0], currentDrawingPolys.getPositions()[i][1], currentDrawingPolys.getPositions()[i][2]);
		glCallList(currentDrawingPolys.getLists()[currentDrawingPolys.getOrder()[i].first][currentDrawingPolys.getOrder()[i].second]);
		glPopMatrix();
	}
	glColor3f(1.0, 0.55, 0.0);
	glCallList(stockList);

    glPopMatrix();
    glutSwapBuffers();
}



// function declarations //////////////////////////////////////////////////////
//void initLights();
//void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
//void drawString(const char *str, int x, int y, float color[4], void *font);
//void drawString3D(const char *str, float pos[3], float color[4], void *font);
//void showInfo();
//const char* getPrimitiveType(GLenum type);
//GLuint tessellate1();
//GLuint tessellate2();
//GLuint tessellate3();



// global variables ///////////////////////////////////////////////////////////
//void *font = GLUT_BITMAP_8_BY_13;
//bool mouseLeftDown;
//bool mouseRightDown;
//float mouseX, mouseY;
//float cameraAngleX;
//float cameraAngleY;
//float cameraDistance;
//int drawMode = 0;
//GLuint listId1, listId2, listId3;       // IDs of display lists
/*
///////////////////////////////////////////////////////////////////////////////
// create a tessellation object and compile a quad into a display list
///////////////////////////////////////////////////////////////////////////////
GLuint tessellate1()
{
    GLuint id = glGenLists(1);  // create a display list
    if(!id) return id;          // failed to create a list, return 0

    GLUtesselator *tess = gluNewTess(); // create a tessellator
    if(!tess) return 0;  // failed to create tessellation object, return 0

    // define concave quad data (vertices only)
    //  0    2
    //  \ \/ /
    //   \3 /
    //    \/
    //    1
    GLdouble quad1[4][3] = { {-1,3,0}, {0,0,0}, {1,3,0}, {0,2,0} };

    // register callback functions
    gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK *)())tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK *)())tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (CALLBACK *)())tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK *)())tessVertexCB);

    // tessellate and compile a concave quad into display list
    // gluTessVertex() takes 3 params: tess object, pointer to vertex coords,
    // and pointer to vertex data to be passed to vertex callback.
    // The second param is used only to perform tessellation, and the third
    // param is the actual vertex data to draw. It is usually same as the second
    // param, but It can be more than vertex coord, for example, color, normal
    // and UV coords which are needed for actual drawing.
    // Here, we are looking at only vertex coods, so the 2nd and 3rd params are
    // pointing same address.
    glNewList(id, GL_COMPILE);
    glColor3f(1,1,1);
    gluTessBeginPolygon(tess, 0);                   // with NULL data
        gluTessBeginContour(tess);
            gluTessVertex(tess, quad1[0], quad1[0]);
            gluTessVertex(tess, quad1[1], quad1[1]);
            gluTessVertex(tess, quad1[2], quad1[2]);
            gluTessVertex(tess, quad1[3], quad1[3]);
        gluTessEndContour(tess);
    gluTessEndPolygon(tess);
    glEndList();

    gluDeleteTess(tess);        // delete after tessellation

    // DEBUG //
    // print out actual GL calls that are performed
    cout << endl;
    cout << "1. Concave Quad\n";
    cout << "===============\n";
    cout << ss.str().c_str() << endl;
    ss.str("");                     // clear string buffer

    return id;      // return handle ID of a display list
}



///////////////////////////////////////////////////////////////////////////////
// tessellate a polygon with a hole and compile it into a display list
///////////////////////////////////////////////////////////////////////////////
GLuint tessellate2()
{
    GLuint id = glGenLists(1);  // create a display list
    if(!id) return id;          // failed to create a list, return 0

    GLUtesselator *tess = gluNewTess(); // create a tessellator
    if(!tess) return 0;         // failed to create tessellation object, return 0

    // define concave quad with a hole
    //  0--------3
    //  | 4----7 |
    //  | |    | |
    //  | 5----6 |
    //  1--------2
    GLdouble quad2[8][3] = { {-2,3,0}, {-2,0,0}, {2,0,0}, { 2,3,0},
                             {-1,2,0}, {-1,1,0}, {1,1,0}, { 1,2,0} };

    // register callback functions
    gluTessCallback(tess, GLU_TESS_BEGIN, (void (__stdcall*)(void))tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (__stdcall*)(void))tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (__stdcall*)(void))tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (__stdcall*)())tessVertexCB);

    // tessellate and compile a concave quad into display list
    glNewList(id, GL_COMPILE);
    glColor3f(1,1,1);
    gluTessBeginPolygon(tess, 0);                       // with NULL data
        gluTessBeginContour(tess);                      // outer quad
            gluTessVertex(tess, quad2[0], quad2[0]);
            gluTessVertex(tess, quad2[1], quad2[1]);
            gluTessVertex(tess, quad2[2], quad2[2]);
            gluTessVertex(tess, quad2[3], quad2[3]);
        gluTessEndContour(tess);
        gluTessBeginContour(tess);                      // inner quad (hole)
            gluTessVertex(tess, quad2[4], quad2[4]);
            gluTessVertex(tess, quad2[5], quad2[5]);
            gluTessVertex(tess, quad2[6], quad2[6]);
            gluTessVertex(tess, quad2[7], quad2[7]);
        gluTessEndContour(tess);
    gluTessEndPolygon(tess);
    glEndList();

    gluDeleteTess(tess);        // delete after tessellation

    // DEBUG //
    // print out actual GL calls that are performed
    cout << endl;
    cout << "2. Quad with a Hole\n";
    cout << "===================\n";
    cout << ss.str().c_str() << endl;
    ss.str("");                     // clear string buffer

    return id;      // return handle ID of a display list
}



///////////////////////////////////////////////////////////////////////////////
// tessellate a self-intersecting polygon and compile it into a display list
// Note that tessellator will find out the self-intersecting vertex where
// two edge lines are met, and pass it to your combine callback function.
// The combine callback must store the vertex coords into local memory in
// your application and also handle other vertex data, such as color, UVs.
///////////////////////////////////////////////////////////////////////////////
GLuint tessellate3()
{
    GLuint id = glGenLists(1);  // create a display list
    if(!id) return id;          // failed to create a list, return 0

    GLUtesselator *tess = gluNewTess(); // create a tessellator
    if(!tess) return 0;  // failed to create tessellation object, return 0

    // define self-intersecting star shape (with color)
    //      0
    //     / \
    //3---+---+---2
    //  \ |   | /
    //   \|   |/
    //    +   +
    //    |\ /|
    //    | + |
    //    |/ \|
    //    1   4
    GLdouble star[5][6] = { { 0.0, 3.0, 0,  1, 0, 0},       // 0: x,y,z,r,g,b
                            {-1.0, 0.0, 0,  0, 1, 0},       // 1:
                            { 1.6, 1.9, 0,  1, 0, 1},       // 2:
                            {-1.6, 1.9, 0,  1, 1, 0},       // 3:
                            { 1.0, 0.0, 0,  0, 0, 1} };     // 4:

    // register callback functions
    // This polygon is self-intersecting, so GLU_TESS_COMBINE callback function
    // must be registered. The combine callback will process the intersecting vertices.
    gluTessCallback(tess, GLU_TESS_BEGIN, (void (__stdcall*)(void))tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void (__stdcall*)(void))tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void (__stdcall*)(void))tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (__stdcall*)(void))tessVertexCB2);
    gluTessCallback(tess, GLU_TESS_COMBINE, (void (__stdcall*)(void))tessCombineCB);

    // tessellate and compile a concave quad into display list
    // Pay attention to winding rules if multiple contours are overlapped.
    // The winding rules determine which parts of polygon will be filled(interior)
    // or not filled(exterior). For each enclosed region partitioned by multiple
    // contours, tessellator assigns a winding number to the region by using
    // given winding rule. The default winding rule is GLU_TESS_WINDING_ODD,
    // but, here we are using non-zero winding rule to fill the middle area.
    // BTW, the middle region will not be filled with the odd winding rule.
    gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
    glNewList(id, GL_COMPILE);
    gluTessBeginPolygon(tess, 0);                   // with NULL data
        gluTessBeginContour(tess);
            gluTessVertex(tess, star[0], star[0]);
            gluTessVertex(tess, star[1], star[1]);
            gluTessVertex(tess, star[2], star[2]);
            gluTessVertex(tess, star[3], star[3]);
            gluTessVertex(tess, star[4], star[4]);
        gluTessEndContour(tess);
    gluTessEndPolygon(tess);
    glEndList();

    gluDeleteTess(tess);        // safe to delete after tessellation

    // DEBUG //
    // print out actual GL calls that are performed
    cout << endl;
    cout << "3. Self-Intersect Star";
    cout << "======================\n";
    cout << ss.str().c_str() << endl;
    cout << "Tessellator creates " << vertexIndex << " new intersecting vertices\n";
    ss.str("");                     // clear string buffer

    return id;      // return handle ID of a display list
}



///////////////////////////////////////////////////////////////////////////////
// convert enum of OpenGL primitive type to a string(char*)
// OpenGL supports only 10 primitive types.
///////////////////////////////////////////////////////////////////////////////
const char* getPrimitiveType(GLenum type)
{
    switch(type)
    {
    case 0x0000:
        return "GL_POINTS";
        break;
    case 0x0001:
        return "GL_LINES";
        break;
    case 0x0002:
        return "GL_LINE_LOOP";
        break;
    case 0x0003:
        return "GL_LINE_STRIP";
        break;
    case 0x0004:
        return "GL_TRIANGLES";
        break;
    case 0x0005:
        return "GL_TRIANGLE_STRIP";
        break;
    case 0x0006:
        return "GL_TRIANGLE_FAN";
        break;
    case 0x0007:
        return "GL_QUADS";
        break;
    case 0x0008:
        return "GL_QUAD_STRIP";
        break;
    case 0x0009:
        return "GL_POLYGON";
        break;
    }
}
*/

/*
///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color

    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
void drawString3D(const char *str, float pos[3], float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color

    glColor4fv(color);          // set text color
    glRasterPos3fv(pos);        // place text position

    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_LIGHTING);
    glPopAttrib();
}
*/


///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////




/*
///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void initLights()
{
    // set up light colors (ambient, diffuse, specular)
    GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};  // ambient light
    GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};  // diffuse light
    GLfloat lightKs[] = {1, 1, 1, 1};           // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light
    float lightPos[4] = {0, 0, 20, 1}; // positional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}



///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}



///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo()
{
    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);     // switch to projection matrix
    glPushMatrix();                  // save current projection matrix
    glLoadIdentity();                // reset projection matrix
    gluOrtho2D(0, 400, 0, 200);  // set to orthogonal projection

    float color[4] = {1, 1, 1, 1};

    stringstream ss;
    ss << std::fixed << std::setprecision(3);

    if(drawMode == 0)
        ss << "Draw Mode: Fill" << ends;
    else if(drawMode == 1)
        ss << "Draw Mode: Wireframe" << ends;
    else
        ss << "Draw Mode: Points" << ends;
    drawString(ss.str().c_str(), 1, 186, color, font);
    ss.str("");

    ss << "Press 'D' to switch drawing mode." << ends;
    drawString(ss.str().c_str(), 1, 2, color, font);
    ss.str("");

    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}
*/


//=============================================================================
// CALLBACKS
//=============================================================================

