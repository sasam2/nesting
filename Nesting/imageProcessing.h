#pragma once
#include <GL\glut.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <vector>
#include <unordered_set>
#include "Problem.h"
#include "global.h"
#include "print_utils.h"

#include <CGAL/ch_graham_andrew.h>

#include <CGAL/minkowski_sum_2.h>

#include <CGAL/Small_side_angle_bisector_decomposition_2.h>
#include <CGAL/Polygon_convex_decomposition_2.h>

#define M_PI       3.14159265358979323846

 
/*
typedef Kernel::Construct_direction_2         Construct_direction_2;
typedef Kernel::Orientation_2                 Compute_orientation_2;
typedef Kernel::Compare_x_2                   Compare_x_2;
typedef Kernel::Compare_y_2                   Compare_y_2;
typedef Kernel::Compare_xy_2                  Compare_xy_2;

// Polygon-related types:
typedef Polygon_2::Vertex_circulator          Vertex_circulator;
typedef std::list<Polygon_2>                           Polygons_list;
typedef Polygons_list::iterator               Polygons_iterator;
*/

using namespace std;

extern IplImage ***imagesNFPs;
extern Layout layout;
extern int stockList;
//extern double res;


//Conversion functions
Component pointVec2Component(vector<cv::Point> poly, bool rev=false);
Polygon_with_holes_2 piece2PolygonWithHoles(Piece a, double inv=1);
vector<list<Polygon_2>> decomposePiecesAngBis (vector<Polygon_with_holes_2> pieces);
Piece polygonWithHoles2Piece(Polygon_with_holes_2 pwh);
Piece polygon2Piece(Polygon_2 pwh);
vector<Piece> contourVecPieceVec(vector<vector<cv::Point>> contours, vector<cv::Vec4i> hierarchy);
vector<Piece> contourVecPieceVec(vector<vector<cv::Point>> contours);

vector<vector<cv::Point>> getFeasiblePositions(IplImage * image);
cv::Point bottomLeft(vector<vector<cv::Point>> feasiblePositions);
GLdouble* bottomLeft(vector<vector<GLdouble*>> feasiblePositions);
//Contour and Threshold functions
IplImage *getOpenCVImage(int xb, int yb, int width, int height, int channel=GL_BLUE);
pair<vector<vector<cv::Point>>, vector<cv::Vec4i>> getContours(IplImage *img, double maxDist);
void drawContour(IplImage *img, vector<cv::Point> contour, cv::Scalar color);
//void showContours(IplImage *thrImg);
//void updateThresholdImage(int bar, void *img);
void evaluateThresholdWindow(IplImage *img, int stepInit);
vector<Polygon_2> invertPolygonWithHoles(Polygon_with_holes_2 layout, double maxPieceWidth, double maxPieceHeight);

//NFPS
Polygon_2 minkowskiSumConvexPoly2_ConvexHull2(Polygon_2 a, Polygon_2 b);
//vector<Polygon_2> calcNFPsMinkowskiSum(list<Polygon_2> deca, list<Polygon_2> decb);
//Polygon_2 minkowskiSumConvexPoly1(Polygon_2 a, Polygon_2 b);
//Polygon_2 minkowskiSumConvexPoly2_GrahamAndrew(Polygon_2 a, Polygon_2 b);
//void minkowskiSumConvexPolyOPENCL(Polygon_2 a, Polygon_2 b);
//Polygon_2 minkowskiSumConvexPoly3(Polygon_2 a, Polygon_2 b);
//vector<Polygon_2> minkowskiSumsWithDecomposition3(list<Polygon_2>   subPolyPa, list<Polygon_2>   subPolyPb, double *time);


vector<list<Polygon_2>> decomposePiecesGreene (vector<Piece> pieces);
vector<list<Polygon_2>> decomposePiecesOptimal (vector<Piece> pieces);
vector<list<Polygon_2>> decomposePiecesHertMehl(vector<Piece> pieces);
vector<list<Polygon_2>> decomposePiecesAngBis (vector<Piece> pieces);
vector<Polygon_2> minkowskiSumsWithDecompositionNoConversion(list<Polygon_2>   subPolyPa, list<Polygon_2>   subPolyPb);
vector<Polygon_2> minkowskiSumsWithDecompositionNoConversion(vector<Polygon_2>   subPolyPa, vector<Polygon_2>   subPolyPb);
/*
Point_2 chooseRandomPiece(vector<int> piecesToPlace, map<Point_2, PolyInfo> pieces);
Point_2 chooseLargerPiece(vector<int> piecesToPlace, map<Point_2, PolyInfo> pieces);
Point_2 choosePieceGreaterWidth(vector<int> piecesToPlace, map<Point_2, PolyInfo> pieces);
Point_2 choosePieceGreaterHeight(vector<int> piecesToPlace, map<Point_2, PolyInfo> pieces);
*/
vector<Polygon_2>** decomposePiecesAngBis (Polygon_with_holes_2** pieces, int dim1, int dim2);
Polygon_2  sortEdgesConvexPoly(Polygon_2 a, Polygon_2 b);
vector<Polygon_2> sortEdgesWithDecomposition(vector<Polygon_2>   subPolyPa, vector<Polygon_2>   subPolyPb);
vector<Polygon_2> sortEdgesWithDecomposition(list<Polygon_2>   subPolyPa, list<Polygon_2>   subPolyPb);