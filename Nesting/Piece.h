#pragma once
#include <iostream>
#include <vector>
#include <GL\glut.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
using namespace std;

typedef vector<GLdouble*> Component;

class BoundingBox
{
	GLdouble x, y, width, height;
public:
	BoundingBox(void);
	BoundingBox(GLdouble minX, GLdouble minY, GLdouble w, GLdouble h);
	~BoundingBox(void);

	GLdouble getX();
	GLdouble getY();
	GLdouble getWidth();
	GLdouble getHeight();
	void normalizeToOrigin();
	bool contains(cv::Point point);
	bool contains(GLdouble *point);

};
class PolyInfo
{
public:
	double width, height, area, concavity, rectangularity;
	int numberOfConcaveVertices;
	PolyInfo(){}
	PolyInfo(double w, double l, double a,double r, double c, int nCV){
		width=w;		height=l;		area=a;		rectangularity=r;	concavity=c;	numberOfConcaveVertices=nCV;
	}

};

class Piece
{
protected:
	 vector<Component> components;
public:
	Piece(void);
	Piece(Component c);
	Piece(vector<Component> c);
	~Piece(void);

	vector<Component> getComponents();
	void addComponent(Component c);
	void eraseComponent(int i);
	//void getBoundingBox(GLdouble** boundingBox);
	void normalize(GLdouble point[3]);
	void normalizeToOrigin();
	//Piece clone();
	void print();
	BoundingBox getBoundingBox();
	friend ostream &operator<<(ostream &c, Piece p);
};

class Decomposition: public Piece
{
	public:
	Decomposition(void);
	~Decomposition(void);
};


class Canvas: public Piece
{
public:
	Canvas(void);
	Canvas(GLdouble width, GLdouble height);
	Canvas(GLdouble minX, GLdouble minY, GLdouble width, GLdouble height);
	~Canvas(void);

};