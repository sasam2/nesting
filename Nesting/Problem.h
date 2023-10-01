#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <GL/glut.h>
#include "Piece.h"

#include <stdlib.h>
#include <time.h>

using namespace std;

#define myMax(x,y) (x)>(y)?(x):(y)
#define myMin(x,y) (x)<(y)?(x):(y)

class Problem
{
protected:
	vector<Piece> pieces;
	vector<int> quantity;
	
public:
	Problem();
	~Problem();
	void addPiece(Piece newPiece, int quant/*, double rot[]*/);
	void popPiece();
	void print();
	//int decPieceQuantity(int piece);
	//bool empty();
	vector<Piece> getPieces();
	vector<int> getQuantity();
	int totalOfPieces();
	int maxHeight();
};


class Layout: public Problem
{
	//Piece stockSheet;
	vector<Piece> stockSheet;
	int rotationStep, margin;
	double resolution;
	vector<GLfloat*> positions;
	vector<int> order;

public:
	Layout(void);
	Layout(Problem p, vector<Piece> ss, int rot);
	Layout(Problem p, vector<Piece> ss, int rot, double res);
	Layout(Problem p, vector<Piece> ss, int rot, double res, int marg);
	Layout(Problem p, vector<Piece> ss);
	Layout(vector<Piece> ps, vector<Piece> ss);
	~Layout(void);

	vector<Piece> getStockSheet();
	void setStockSheet(vector<Piece> ss);

	GLdouble *Layout::generateRandomPosition(int p);
	int chooseRandomPiece();

	void addPiecePlacement(int piece, GLfloat *position);
	int getRotationStep(){	return rotationStep;	}
	double getResolution(){	return resolution;	}
	int getMargin(){	return margin;	}

	double getMaxWidth();
	double getMaxHeight();
	BoundingBox Layout::stockSheetsBoundingBox();
};