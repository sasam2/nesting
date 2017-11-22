#pragma once
#include "GL/glew.h"
#include <GL/glui.h>
#include <iostream>
#include "loadFile.h"
#include "global.h"
#include "imageProcessing.h"
#include "Picture.h"
#include "algorithms.h"
#include "OpenglCallbacks.h"

#define WIDER 1
#define HIGHER 2
#define LARGER 3
#define MORE_IRREGULAR 4
#define MORE_RECTANGULAR 5
#define RANDOM 6

using std::cout;
using std::endl;

extern GLUI_EditText *edittext_file;

extern GLUI_RadioGroup *radiogroup_heuristic; 
extern GLUI_RadioGroup *radiogroup_staticheuristic;

//extern char *problemName;
extern char problemName[1024];
extern int saveAll;
extern int staticHeuristic;
extern Layout layout;
extern list<Point_2> piecesOrdered;
extern int numberOfPolygons;
extern int nrOfRotations;

extern map<Point_2, GLuint> layoutNFPsTest;	//"PreProcessing.h" nfps dos layouts possiveis na proxima iteracao
extern map<Point_2, map<Point_2, DrawingWithRotations>> drawingNFPsTest;	//"PreProcessing.h" nfps do layout actual
extern map<Point_2, DrawingWithRotations> drawingPolysTest;					//poligonos do layout actual
extern int tx, ty, tw, th;
extern GLUI_RadioGroup *radiogroup_nfps;
extern vector<Polygon_2> (*nfps)(vector<Polygon_2>, vector<Polygon_2>);

void solveCB(int dummy);
void heuristicCB(int h_choosen);
void updateSolvingConfiguration();
void preProcessing();
void preProcessingStatic(int nrOfRotations, map<Point_2, PolyInfo> infos);
void preProcessingDynamic(int nrOfRotations, int numberOfNFPs, map<Point_2, PolyInfo> infos, vector<Polygon_2>** polygonsDecompositions, vector<Polygon_2>**** nfpsPiecesAndStockSheet);