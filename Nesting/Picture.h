#pragma once

#include <GL/glut.h>
#include <vector>
#include <algorithm>


#include "Problem.h"
#include "global.h"

using std::vector;
using std::count;

class DrawingWithRotations
{
	vector<vector<int>> displayLists;
	vector<vector<BoundingBox>> boundingBoxes;

	vector<GLdouble*> positions;
	vector<GLfloat*> colors;
	vector<pair<int, int>> order;

	BoundingBox viewPort;

public:
	GLuint bufferObject;

	DrawingWithRotations();
	DrawingWithRotations(int nrOfPolygons, int nrOfRotations);
	~DrawingWithRotations();
	
	int addDisplayList(int polygon, int rotation, int dl, BoundingBox bb);
	void addListPlacement(int polygon, int rotation, GLdouble *position, GLfloat *color);
	void popLastPlacement();
	//int countPlacedList(int list);
	void resetPlacements();
	void clear();
	vector<GLdouble*> getPositions();
	vector<pair<int,int>> getOrder();
	vector<vector<int>> getLists();
	vector<GLfloat*> getColors();
	BoundingBox getViewPort();
	void setPiecesPositions(vector<GLdouble*> pos, vector<pair<int, int>> ord);
	void addDisplayListsFromPolygonVector(vector<Polygon_2> **nfps, int size1, int size2, int (*generateDisplayList) (vector<Polygon_2>));
	void addDisplayListsFromPolygonVector(Polygon_with_holes_2 **polys, int size1, int size2, int (*generateDisplayList) (Polygon_with_holes_2));
	void addNFPsDisplayListsFromPolygonVector(vector<Polygon_2> **polys, int size1, int size2, int (*generateDisplayList) (vector<Polygon_2>));

	void setViewPort(BoundingBox v);

	BoundingBox generateGridPositions();
};

/*
class DrawingInterface
{
	public:
	virtual DrawingInterface();
	virtual ~DrawingInterface();

	//virtual int addDisplayList(int dl, BoundingBox bb);
	//virtual  addListPlacement(int list, GLdouble *position, GLfloat *color);
	virtual void popLastPlacement();
	virtual int countPlacedList(int list);
	virtual void resetPlacements();
	virtual vector<GLdouble*> getPositions();
	virtual vector<int> getOrder();
	virtual vector<int> getLists();
	virtual vector<GLfloat*> getColors();
	virtual BoundingBox getViewPort();
	virtual void setPiecesPositions(vector<GLdouble*> positions, vector<int> order);
	virtual void addDisplayListsFromLayout(Layout l, int (*generateDisplayList) (Piece));
	virtual void addDisplayListsFromPolygonVector(vector<Polygon_2> **nfps, int size1, int size2, int (*generateDisplayList) (vector<Polygon_2>));
	virtual void addDisplayListsFromPolygonVector(vector<Polygon_2> *nfps, int size, int (*generateDisplayList) (vector<Polygon_2>));
	virtual void setViewPort(BoundingBox v);
	virtual BoundingBox generateGridPositions();

}*/