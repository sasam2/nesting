#include "global.h"

#define min(a,b) (a)<(b)? a: b

#define ax(a,b) (a)>(b)? a: b

double calculateThreshold(Problem p)
{
	return  1.0/(double)((p.totalOfPieces()/*/nColorComponents*/)+1);
}
double diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
	return diffms;
}
GLdouble *createPosition(double x, double y, double z)
{
	GLdouble *position = new GLdouble[4];
	position[0]=x;	position[1]=y;		position[2]=z;		position[3]=0.0;
	return position;
}
GLfloat *createColor(float r, float g, float b, float a)
{
	GLfloat *color = new GLfloat[4];
	color[0]=r;	color[1]=g;	color[2]=b; color[3]=a;
	return color;
}
GLdouble *transformVertex(GLdouble mat[4][4], GLdouble *vertex)
{
	GLdouble *point = createPosition(0.0, 0.0, 0.0);

	for(int i=0; i<3; i++)							//altura da matriz
        for(int k=0; k<3; k++)						//largura da matriz, altura do vertice
        {
			//cout<<i<<" "<<k<<" "<<mat[i][k]<<"*"<<vertex[k]<<"="<<point[i]<<endl;
			point[i]+=mat[i][k]*vertex[k];
		}

	return point;
}
Polygon_with_holes_2 generatePieceDiscreteRotation(Polygon_with_holes_2 p, double currentAngle)
{
	//print_polygon_with_holes(p);
	Transformation rotate(CGAL::ROTATION, sin(currentAngle), cos(currentAngle));
	//cout<<"Angle="<<currentAngle<<endl;

	//rodar outer boundary
	Polygon_2 rotatedPOuterBoundary;
	for (Polygon_2::Vertex_iterator vit = p.outer_boundary().vertices_begin(); vit != p.outer_boundary().vertices_end(); ++vit)
	{
		Point_2 currentPoint=*vit;
		rotatedPOuterBoundary.push_back(rotate(currentPoint));
	}
	//cout<<"Rotated outer boundary: "<<endl;
	//print_polygon(rotatedPOuterBoundary);

	Vector_2 lowLeft(-rotatedPOuterBoundary.bbox().xmin(), -rotatedPOuterBoundary.bbox().ymin());
	Transformation normalizeToOrigin(CGAL::TRANSLATION, lowLeft);
	//cout<<"Vector: "<<lowLeft.hx()<<" "<<lowLeft.hy()<<endl;
	//Polygon_2 rotatedPOuterBoundaryNormalized;
	for (Polygon_2::Vertex_iterator vit = rotatedPOuterBoundary.vertices_begin(); vit !=rotatedPOuterBoundary.vertices_end(); ++vit)
	{
		Point_2 currentPoint=*vit;
		*vit=normalizeToOrigin(currentPoint);
		//cout<<"Before: "<<currentPoint<<" after: "<<*vit<<endl;
	}
	//cout<<"Rotated outer boundary normalized: "<<endl;
	//print_polygon(rotatedPOuterBoundary);

	Polygon_with_holes_2 rotatedP(rotatedPOuterBoundary);
	for (Polygon_with_holes_2::Hole_iterator hit = p.holes_begin(); hit != p.holes_end(); ++hit)
	{
		Polygon_2 currentHole;
		for (Polygon_2::Vertex_iterator vit = hit->vertices_begin(); vit != hit->vertices_end(); ++vit)
		{
			Point_2 currentPoint=*vit;
			currentHole.push_back(normalizeToOrigin(rotate(currentPoint)));
		}
		rotatedP.add_hole(currentHole);
	}

	//print_polygon_with_holes(rotatedP);

	return rotatedP;
}
vector<Polygon_2> generateDecomposedPieceDiscreteRotation(vector<Polygon_2> p, double currentAngle)
{
	//print_polygon_with_holes(p);
	Transformation rotate(CGAL::ROTATION, sin(currentAngle), cos(currentAngle));
	//cout<<"Angle="<<currentAngle<<endl;

	vector<Polygon_2> rotatedP;
	Point_2 bottomLeftCorner(numeric_limits<double>::infinity(), numeric_limits<double>::infinity());
	for(int i = 0; i < rotatedP.size(); i++)
	{
		//rodar outer boundary
		Polygon_2 rotatedPOuterBoundary;
		for (Polygon_2::Vertex_iterator vit = p[i].vertices_begin(); vit != p[i].vertices_end(); ++vit)
		{
			Point_2 currentPoint=*vit;
			rotatedPOuterBoundary.push_back(rotate(currentPoint));
		}
		rotatedP.push_back(rotatedPOuterBoundary);
		if(rotatedPOuterBoundary.bbox().xmin()<bottomLeftCorner.x())
			bottomLeftCorner=Point_2(rotatedPOuterBoundary.bbox().xmin(), bottomLeftCorner.y());
		if(rotatedPOuterBoundary.bbox().ymin()<bottomLeftCorner.y())
			bottomLeftCorner=Point_2( bottomLeftCorner.x(),rotatedPOuterBoundary.bbox().ymin());
	}

	Vector_2 lowLeft(-bottomLeftCorner.x(), -bottomLeftCorner.y());
	Transformation normalizeToOrigin(CGAL::TRANSLATION, lowLeft);
	for(int i = 0; i < rotatedP.size(); i++)
	{
		for (Polygon_2::Vertex_iterator vit = rotatedP[i].vertices_begin(); vit !=rotatedP[i].vertices_end(); ++vit)
		{
			Point_2 currentPoint=*vit;
			*vit=normalizeToOrigin(currentPoint);
		}
	}

	return rotatedP;
}
Piece invertPiece(Piece p)
{
	Piece pInv;
	for(int i=0; i < p.getComponents().size(); i++)
	{
		Component c;
		for(int j=0; j < p.getComponents()[i].size(); j++)
		{
			c.push_back(createPosition(-p.getComponents()[i][j][0], -p.getComponents()[i][j][1], -p.getComponents()[i][j][2]));
		}
		pInv.addComponent(c);
	}
	return pInv;		
}
BoundingBox getBoundingBox(Polygon_2 p)
{
	GLdouble maxXY[2];
	GLdouble minXY[2];
	
	maxXY[0]=0.0;
	maxXY[1]=0.0;
	
	minXY[0]=numeric_limits<double>::infinity();
	minXY[1]=numeric_limits<double>::infinity();
	
	//escolhe os pontos dos cantos inferior esquerdo e superior direito
	for (Polygon_2::Vertex_iterator vit = p.vertices_begin(); vit != p.vertices_end(); ++vit)
	{
		//cout<<"component "<<i<<" "<<components[i][j][0]<<" "<<components[i][j][1]<<endl;
		double vitX=CGAL::to_double(vit->x()/*.exact()*/);
		double vitY=CGAL::to_double(vit->y()/*.exact()*/);

		minXY[0]=min(vitX,minXY[0]);
		minXY[1]=min(vitX,minXY[0]);

		maxXY[0]=max(vitX,minXY[0]);
		maxXY[1]=max(vitX,minXY[0]);
	}

	BoundingBox bb(minXY[0], minXY[1], maxXY[0]-minXY[0], maxXY[1]-minXY[1]);
	return bb;
}
BoundingBox getBoundingBox(vector<Polygon_2> poly)
{
	CGAL::Bbox_2 nfpBBpart(0,0,0,0);
	for(int p=0; p <poly.size(); p++)	
	{
		//print_polygon (poly[p]);
		nfpBBpart=nfpBBpart+poly[p].bbox();
		//cout<<nfpBBpart.xmin()<<" "<<nfpBBpart.ymin()<<"; "<<nfpBBpart.xmax()<<" "<<nfpBBpart.ymax()<<endl;
	}
	BoundingBox nfpBB(nfpBBpart.xmin(), nfpBBpart.ymin(), nfpBBpart.xmax()-nfpBBpart.xmin(), nfpBBpart.ymax()- nfpBBpart.ymin());
	return nfpBB;
}



int getNumberOfConcaveVertices(Polygon_2 p)
{
	int numberOfConcaveVertices=0;

	vector<Point_2> vertices = p.container();
	for(int v=0; v <vertices.size(); v++)
	{
		Point_3 p0 = Point_3(vertices[(v-1+vertices.size())%vertices.size()].x(), vertices[(v-1+vertices.size())%vertices.size()].y(), 0);
		Point_3 p1 = Point_3(vertices[v].x(), vertices[v].y(), 0);
		Point_3 p2 = Point_3(vertices[(v+1)%vertices.size()].x(), vertices[(v+1)%vertices.size()].y(), 0);

		Vector_3 v0(p0, p1);
		Vector_3 v1(p1, p2);
		Vector_3 crossProduct = CGAL::cross_product(v0, v1);
			
		if(crossProduct.z()<0) //se o produto vectorial e positivo, e um vertice concavo
		{
			numberOfConcaveVertices++;
		}
	}
	return numberOfConcaveVertices;
}
