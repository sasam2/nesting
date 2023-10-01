#pragma once
#include <GL/glew.h>
#include <GL/glui.h>
#include "Picture.h"
#include "imageProcessing.h"


//posicionamento estatico
#define WIDER 1
#define HIGHER 2
#define LARGER 3
#define MORE_IRREGULAR 4
#define MORE_RECTANGULAR 5
#define RANDOM 6

extern Layout layout;
extern map<Point_2, map<Point_2, DrawingWithRotations>> drawingNFPsTest;
extern map<Point_2, DrawingWithRotations> drawingPolysTest;
extern map<Point_2, DrawingWithRotations> currentDrawingNFPs;
extern map<Point_2, GLuint> layoutNFPsTest;
extern list<Point_2> piecesOrdered;
extern DrawingWithRotations currentDrawingPolys;
extern int iteration;
extern vector<int> piecesAvaliability;
//extern char *problemName;
extern char problemName[1024];
extern int staticHeuristic;
extern Polygon_with_holes_2 **polygons;
extern int numberOfPolygons;
extern GLfloat *blue;
extern int tx, ty, tw, th;

void buildStaticLayout();
double calculateMaxLength();
double calculateUsedArea(double maxLength);
bool displayStatic(ofstream *myfile);
Point_2 putPieceDynamic(bool reset);
Point_2 dynamicPieceSelection();
GLdouble *getPiecePosition(IplImage *layoutImg);
void displayDynamic_drawCurrentLayout();
bool displayDynamic(ofstream *myfile);
void buildDynamicLayout();