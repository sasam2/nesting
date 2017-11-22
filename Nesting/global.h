#pragma once

#include "Piece.h"
#include "Problem.h"

#include <iomanip>

#include <boost/shared_ptr.hpp>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_2.h>
#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/convex_hull_2.h>
//#include "print_utils.h"

#define myMax(x,y) (x)>(y)?(x):(y)
#define myMin(x,y) (x)<(y)?(x):(y)

//struct Kernel : public CGAL::Exact_predicates_inexact_constructions_kernel {}; //Da erro ao compilar com a decomposiçao de greene
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

typedef Kernel::Point_3                               Point_3;
typedef Kernel::Point_2                               Point_2;
typedef CGAL::Polygon_2<Kernel>                       Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel>            Polygon_with_holes_2;


// Kernel types:
typedef CGAL::Vector_3<Kernel>				  Vector_3;
typedef CGAL::Vector_2<Kernel>				  Vector_2;
  
typedef Kernel::Direction_2                   Direction_2;
  
typedef Kernel::Construct_translated_point_2  Translate_point_2;
typedef Kernel::Construct_vector_2            Construct_vector_2;

//typedef Kernel::Vector_2::Rep	Vector_2;

typedef boost::shared_ptr<Polygon_2> PolygonPtr ;
typedef CGAL::Straight_skeleton_2<Kernel> Ss ;
typedef boost::shared_ptr<Ss> SsPtr ;
typedef std::vector<PolygonPtr> PolygonPtrVector ;
typedef CGAL::Aff_transformation_2<Kernel>  Transformation ;

//int generatePieceDisplayList(Piece p/*, double color[4]*/);
int* generateProblemDisplayLists(Problem prob);

double diffclock(clock_t clock1,clock_t clock2);
double calculateThreshold(Problem p);

GLdouble *createPosition(double x, double y, double z);
GLfloat *createColor(float r, float g, float b, float a);
GLdouble *transformVertex(GLdouble mat[4][4], GLdouble *vertex);

//vector<Piece> generatePieceDiscreteRotations(Piece p, vector<double> angles);
//Piece generatePieceDiscreteRotation(Piece p, double angle);

Piece invertPiece(Piece p);
BoundingBox getBoundingBox(Polygon_2 p);
BoundingBox getBoundingBox(vector<Polygon_2> p);

//Piece shrinkPiece(Piece p, float step);
Polygon_with_holes_2 generatePieceDiscreteRotation(Polygon_with_holes_2 p, double currentAngle);
vector<Polygon_2> generateDecomposedPieceDiscreteRotation(vector<Polygon_2> p, double currentAngle);
int getNumberOfConcaveVertices(Polygon_2 p);

