#include "Piece.h"

//Piece
Piece::Piece(void)
{
}

Piece::Piece(Component c)
{
	addComponent(c);
}


Piece::Piece(vector<Component> c)
{
	components=c;
}


Piece::~Piece(void)
{
}

vector<Component> Piece::getComponents()
{
	return components;
}

void Piece::eraseComponent(int i)
{
	vector<Component>::iterator it=components.begin();
	it+=i;
	components.erase(it);
}

void Piece::addComponent(Component c)
{
	components.push_back(c);
}

BoundingBox Piece::getBoundingBox()
{

	GLdouble maxXY[2];
	GLdouble minXY[2];
	
	maxXY[0]=0.0;
	maxXY[1]=0.0;
	
	minXY[0]=numeric_limits<double>::infinity( );
	minXY[1]=numeric_limits<double>::infinity( );
	
	
	//escolhe os pontos dos cantos inferior esquerdo e superior direito
	for(int i=0; i < components.size(); i++)
	{
		for(int j=0; j < components[i].size(); j++)
		{
			//cout<<"component "<<i<<" "<<components[i][j][0]<<" "<<components[i][j][1]<<endl;
			if(components[i][j][0]<minXY[0]) minXY[0]=components[i][j][0];
			if(components[i][j][1]<minXY[1]) minXY[1]=components[i][j][1];

			if(components[i][j][0]>maxXY[0]) maxXY[0]=components[i][j][0];
			if(components[i][j][1]>maxXY[1]) maxXY[1]=components[i][j][1];
		}
	}

	BoundingBox bb(minXY[0], minXY[1], maxXY[0]-minXY[0], maxXY[1]-minXY[1]);
	return bb;
}

void Piece::normalize(GLdouble point[3]){
	//cout<<"Normalize to point "<<point[0]<<" "<<point[1]<<" "<<point[2]<<endl;
	for(int i=0; i <components.size(); i++)
	{ 
		for(int j=0; j < components[i].size(); j++)
		{
			components[i][j][0]-=point[0];
			components[i][j][1]-=point[1];
			components[i][j][2]-=point[2];
			//cout<<"A"<<j+i<<" "<<components[i][j][0]<<" "<<components[i][j][1]<<" "<<components[i][j][2]<<endl;
		}
	}
}

void Piece::normalizeToOrigin()
{
	BoundingBox pieceBB = getBoundingBox();
	GLdouble referencePoint[3] = {0.0, 0.0, 0.0};

	referencePoint[0]=pieceBB.getX();
	referencePoint[1]=pieceBB.getY();
	normalize(referencePoint);
}



/*
Piece Piece::clone()
{
	Piece p;

	for(int i=0; i <components.size(); i++)
	{
		Component c;
		for(int j=0; j < components[i].size(); j++)
		{
			GLdouble *vertex = new GLdouble[3];
			memcpy (vertex, components[i][j], 3*sizeof(GLdouble) );
		}
		p.addComponent(c);
	}

	return p;

}
*/
void Piece::print(){/*
	for(int i=0; i <components.size(); i++){
			cout<<"   Component"<<endl;
			for(int j=0; j < components[i].size(); j++)
				cout<<"      "<<components[i][j][0]<<' '<<components[i][j][1]<<' '<<components[i][j][2]<<endl;
		}*/
	cout<<*this;
}

ostream &operator<<(ostream &c, Piece p){
	for(int i=0; i <p.components.size(); i++){
			c<<"   Component"<<endl;
			for(int j=0; j < p.components[i].size(); j++)
				c<<"      "<<p.components[i][j][0]<<' '<<p.components[i][j][1]<<' '<<p.components[i][j][2]<<endl;
		}
	return c;
}

//Rectangle
Canvas::Canvas(void){
	Piece();
}

Canvas::Canvas(GLdouble width, GLdouble height)
{
	Component c;
	GLdouble *pt=new GLdouble[3];

	pt[0]=0.0; pt[1]=0.0; pt[2]=0.0;
	c.push_back(pt);

	pt=new GLdouble[3];
	pt[0]=0.0; pt[1]=height; pt[2]=0.0;
	c.push_back(pt);

	pt=new GLdouble[3];
	pt[0]=width; pt[1]=height; pt[2]=0.0;
	c.push_back(pt);
	
	pt=new GLdouble[3];
	pt[0]=width; pt[1]=0.0; pt[2]=0.0;
	c.push_back(pt);

	components.push_back(c);	
}

Canvas::Canvas(GLdouble minX, GLdouble minY, GLdouble width, GLdouble height){
	Component c;
	GLdouble *pt=new GLdouble[3];

	pt[0]=minX; pt[1]=minY; pt[2]=0.0;
	c.push_back(pt);

	pt=new GLdouble[3];
	pt[0]=minX; pt[1]=minY+height; pt[2]=0.0;
	c.push_back(pt);

	pt=new GLdouble[3];
	pt[0]=minX+width; pt[1]=minY+height; pt[2]=0.0;
	c.push_back(pt);
	
	pt=new GLdouble[3];
	pt[0]=minX+width; pt[1]=minY; pt[2]=0.0;
	c.push_back(pt);

	components.push_back(c);

}

Canvas::~Canvas(void){}

//BoundingBox
BoundingBox::BoundingBox(void){}

BoundingBox::BoundingBox(GLdouble minX, GLdouble minY, GLdouble w, GLdouble h)
{
	x=minX;
	y=minY;
	width=w;
	height=h;
}

BoundingBox::~BoundingBox(void)
{
}

GLdouble BoundingBox::getX()
{
	return x;
}

GLdouble BoundingBox::getY()
{
	return y;
}

GLdouble BoundingBox::getWidth()
{
	return width;
}

GLdouble BoundingBox::getHeight()
{
	return height;
}

void BoundingBox::normalizeToOrigin()
{
	width-=x;
	height-=y;
	x=0;
	y=0;
}
bool BoundingBox::contains(cv::Point point)
{
	return point.x>=x && point.x<=(x+width) && point.y>=y && point.y<=(y+height);
}
bool BoundingBox::contains(GLdouble *point)
{
	return point[0]>=x && point[0]<=(x+width) && point[1]>=y && point[1]<=(y+height);
}

//decomposition
Decomposition::Decomposition(void)
{
}

Decomposition::~Decomposition(void)
{
}
