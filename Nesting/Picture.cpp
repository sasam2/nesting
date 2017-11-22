#include "Picture.h"

DrawingWithRotations::DrawingWithRotations(){}

DrawingWithRotations::DrawingWithRotations(int nrOfPolygons, int nrOfRotations)
{
	displayLists=vector<vector<int>>(nrOfPolygons);
	boundingBoxes=vector<vector<BoundingBox>>(nrOfPolygons);

	for(int i=0; i < nrOfPolygons; i++)
	{
		displayLists[i]=vector<int>(nrOfRotations);
		boundingBoxes[i]=vector<BoundingBox>(nrOfRotations);
	}
}

DrawingWithRotations::~DrawingWithRotations(){}

int DrawingWithRotations::addDisplayList(int polygon, int rotation, int dl, BoundingBox bb)
{
	if(displayLists.size()<=polygon || polygon<0 || displayLists[0].size()<=rotation || rotation<0 )
	{
		cout<<"Poligono ou rotacao invalida! Poligono([0,"<<displayLists.size()<<"]): "<<polygon<<". Rotacao([0, "<<displayLists[0].size()<<"]): "<<rotation<<"."<<endl;
		return -1;
	}
	
	displayLists[polygon][rotation]=dl;
	boundingBoxes[polygon][rotation]=bb;
}

void DrawingWithRotations::addListPlacement(int polygon, int rotation, GLdouble *position, GLfloat *color)
{
	order.push_back(pair<int, int>(polygon,rotation));
	positions.push_back(position);
	colors.push_back(color);
}

void DrawingWithRotations::popLastPlacement()
{
	order.pop_back();
	positions.pop_back();
	colors.pop_back();
}

void DrawingWithRotations::resetPlacements(){
	for(int i=0; i< order.size(); i++)
		delete[] positions[i];
	positions.clear();
	
	for(int i=0; i< order.size(); i++)
		delete[] colors[i];
	colors.clear();

	order.clear();
}

vector<GLdouble*> DrawingWithRotations::getPositions(){
	return positions;
}

vector<pair<int,int>> DrawingWithRotations::getOrder(){
	return order;
}

vector<vector<int>> DrawingWithRotations::getLists(){
	return displayLists;
}

vector<GLfloat*> DrawingWithRotations::getColors(){
	return colors;
}

BoundingBox DrawingWithRotations::getViewPort(){
	return viewPort;
}

void DrawingWithRotations::setPiecesPositions(vector<GLdouble*> pos, vector<pair<int, int>> ord){
	if(pos.size() == ord.size())
	{
		int i;
		for(i=0; i < ord.size() && ord[i].first < displayLists.size() && ord[i].second < displayLists[0].size(); i++);
		if(i < ord.size()) 
			throw exception();
	}else{
		throw exception();
	}
	positions=pos;
	order=ord;
}

void DrawingWithRotations::addDisplayListsFromPolygonVector(vector<Polygon_2> **polys, int size1, int size2, int (*generateDisplayList) (vector<Polygon_2>)){
	if(size1!=displayLists.size() || size2!=displayLists[0].size()){
		cout<<"Array dimensions should be "<<displayLists.size()<<", "<<displayLists[0].size()<<" instead of "<<size1<<", "<<size2<<"."<<endl;
		return;
	}
	for(int i=0; i < size1; i++){
		for(int j=0; j < size2; j++){
			addDisplayList(i, j, generateDisplayList(polys[i][j]), getBoundingBox(polys[i][j]));
		}
	}
	//addDisplayList(size1-1, 0, generateDisplayList(polys[size1-1][0]), getBoundingBox(polys[size1-1][0].outer_boundary()));

}

void DrawingWithRotations::addNFPsDisplayListsFromPolygonVector(vector<Polygon_2> **polys, int size1, int size2, int (*generateDisplayList) (vector<Polygon_2>)){
	if(size1!=displayLists.size() || size2!=displayLists[0].size()){
		cout<<"Array dimensions should be "<<displayLists.size()<<", "<<displayLists[0].size()<<" instead of "<<size1<<", "<<size2<<"."<<endl;
		return;
	}
	for(int i=0; i < size1-1; i++){
		for(int j=0; j < size2; j++){
			addDisplayList(i, j, generateDisplayList(polys[i][j]), getBoundingBox(polys[i][j]));
		}
	}
	addDisplayList(size1-1, 0, generateDisplayList(polys[size1-1][0]), getBoundingBox(polys[size1-1][0]));
}

void DrawingWithRotations::addDisplayListsFromPolygonVector(Polygon_with_holes_2 **polys, int size1, int size2, int (*generateDisplayList) (Polygon_with_holes_2)){
	if(size1!=displayLists.size() || size2!=displayLists[0].size()){
		cout<<"Array dimensions should be "<<displayLists.size()<<", "<<displayLists[0].size()<<" instead of "<<size1<<", "<<size2<<"."<<endl;
		return;
	}
	for(int i=0; i < size1; i++){
		for(int j=0; j < size2; j++){
			addDisplayList(i, j, generateDisplayList(polys[i][j]), getBoundingBox(polys[i][j].outer_boundary()));
		}
	}
	//addDisplayList(size1-1, 0, generateDisplayList(polys[size1-1][0]), getBoundingBox(polys[size1-1][0].outer_boundary()))
}

void DrawingWithRotations::clear()
{
	resetPlacements();
	
	displayLists.clear();
	boundingBoxes.clear();

//	glDeleteBuffers(1, &bufferObject);
}

void DrawingWithRotations::setViewPort(BoundingBox v){
	viewPort=v;
}

BoundingBox DrawingWithRotations::generateGridPositions(){
	double margin=0;
	double maxWidth=0;
	double maxHeight=0;
	double x=0;
	double y=0;
	
	if(displayLists.empty())
		throw exception();
	resetPlacements();
	
	GLfloat *color = createColor(0.0,0.0,1.0, 1.0);
	for(int i=0; i < displayLists.size(); i++)
	{
		for(int j=0; j < displayLists[i].size(); j++)
		{
			cout<<i<<" "<<j<<" / "<< displayLists.size() <<endl;
			order.push_back(pair<int, int>(i,j));
			colors.push_back(color);
			//cout<<x<<" "<<y<<endl;
			cout<<boundingBoxes[i][j].getWidth()<<" "<<boundingBoxes[i][j].getHeight()<<endl;
			positions.push_back(createPosition(x-boundingBoxes[i][j].getX(), y-boundingBoxes[i][j].getY(), 0.0));
		
			x+=boundingBoxes[i][j].getWidth()+margin;
			maxHeight=max(maxHeight, y+boundingBoxes[i][j].getHeight()+margin);
			maxWidth=max(x, maxWidth);
			//cout<<maxWidth<<" "<<maxHeight<<endl;
			if((i+1)%3==0)
			{		
				x=0;
				y=maxHeight;
			}
		}
	}
	viewPort=BoundingBox(0,0,maxWidth,maxHeight);
	return viewPort;
}

