#include "Problem.h"

//Scope::Problem
Problem::Problem()
{
}

Problem::~Problem()
{
}

void Problem::addPiece(Piece newPiece, int quant/*, double rot[]*/){
	pieces.push_back(newPiece);
	quantity.push_back(quant);
}

void Problem::popPiece(){
	pieces.pop_back();
	quantity.pop_back();
}

void Problem::print(){
	for(int i=0; i < pieces.size(); i++)
	{
		cout<<"Piece "<<i<<endl;
		pieces[i].print();
		cout<<"   Quantity "<<quantity[i]<<endl;
	}

}
/*
int Problem::decPieceQuantity(int piece){
	if(quantity[piece]) 
		return --quantity[piece];
	else
		return -1;
}
*/
/*
bool Problem::empty(){
	int i;
	for(i=0; i<quantity.size() && !quantity[i]; i++);
	return i==quantity.size();
}*/

vector<Piece> Problem::getPieces(){
	return pieces;
}

vector<int> Problem::getQuantity(){
	return quantity;
}

int Problem::totalOfPieces(){
	int acc=0;
	for(int i=0; i <quantity.size(); i++)
		acc+=quantity[i];
	return acc;
}

int Problem::maxHeight()
{
	int maxHeight=0;
	/*
	GLdouble **boundingBox = new GLdouble*[4];
	for(int i=0;i<4;i++)
		boundingBox[i]=new GLdouble[3];
	*/
	for(int i=0; i< pieces.size(); i++)
	{
		BoundingBox pieceBB=pieces[i].getBoundingBox();
		int maxY=pieceBB.getY()+pieceBB.getHeight();
		if(maxY>maxHeight)
			maxHeight=maxY;
	}
	/*
	for(int i=0;i<4;i++)
		delete[] boundingBox[i];
	delete[] boundingBox;
	*/
	return maxHeight;
}


Layout::Layout(void)
{
}

Layout::Layout(vector<Piece> ps, vector<Piece> ss)
{
	pieces= ps;
	stockSheet = ss;
}


Layout::~Layout(void)
{
}


Layout::Layout(Problem p, vector<Piece> ss)
{
	//problem=p;
	pieces=p.getPieces();
	quantity=p.getQuantity();
	stockSheet=ss;
	rotationStep=360;
	resolution=stockSheet[0].getBoundingBox().getHeight()/1000;
	margin=0;
}

Layout::Layout(Problem p, vector<Piece> ss, int rot)
{
	pieces=p.getPieces();
	quantity=p.getQuantity();
	stockSheet=ss;
	rotationStep=rot;
	resolution=stockSheet[0].getBoundingBox().getHeight()/1000;
	margin=0;
}

Layout::Layout(Problem p, vector<Piece> ss, int rot, double res)
{
	pieces=p.getPieces();
	quantity=p.getQuantity();
	stockSheet=ss;
	rotationStep=rot;
	resolution=res;
	margin=0;
}

Layout::Layout(Problem p, vector<Piece> ss, int rot, double res, int marg)
{
	pieces=p.getPieces();
	quantity=p.getQuantity();
	stockSheet=ss;
	rotationStep=rot;
	resolution=res;
	margin=marg;
}

vector<Piece> Layout::getStockSheet()
{
	return stockSheet;
}

GLdouble *Layout::generateRandomPosition(int p)
{
	GLdouble *point=new GLdouble[3];
	BoundingBox pieceBB = pieces[p].getBoundingBox();
	BoundingBox stockSheetBB = stockSheet[0].getBoundingBox();
	/*
	if(pieceBB.getX() || pieceBB.getY())
	{
		p=p.clone();
		p.normalizeToOrigin();
		pieceBB = p.getBoundingBox();
	}
	*/
	point[0]= ((float)rand()/RAND_MAX)*(stockSheetBB.getWidth()-pieceBB.getWidth()) + (stockSheetBB.getX());
	point[1]= ((float)rand()/RAND_MAX)*(stockSheetBB.getHeight()-pieceBB.getHeight()) + (stockSheetBB.getY());
	point[2]= 0.0;

	return point;
}

int Layout::chooseRandomPiece()
{
	return rand()%pieces.size();
}


void Layout::setStockSheet(vector<Piece> ss)
{
	stockSheet=ss;
}

double Layout::getMaxWidth()
{
	double maxPieceWidth=0;

	for(int i=0; i < pieces.size();i++)
	{
		BoundingBox pieceBB = pieces[i].getBoundingBox();
		maxPieceWidth = max(maxPieceWidth,pieceBB.getWidth());		//get max width
	}

	return maxPieceWidth;
}

double Layout::getMaxHeight()
{
	double maxPieceHeight=0;

	for(int i=0; i < pieces.size();i++)
	{
		BoundingBox pieceBB = pieces[i].getBoundingBox();
		maxPieceHeight = max(maxPieceHeight,pieceBB.getHeight());	//get max height
	}

	return maxPieceHeight;
}

BoundingBox Layout::stockSheetsBoundingBox()
{
	double minX, minY;
	minX=0;
	minY=0;
	double w=0;
	double h=0;

	for(int p=0; p <stockSheet.size(); p++)
	{
		BoundingBox spBB = stockSheet[p].getBoundingBox();

		minX = myMin(minX, spBB.getX());
		minY = myMin(minY, spBB.getY());

		h = myMax(h, spBB.getHeight());
		w+=spBB.getWidth();
	}

	return BoundingBox(minX, minY, w, h);
}