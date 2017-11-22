#include "imageProcessing.h"

double intMinThreshold, intMaxThreshold;
unsigned char threshold;
int step, bar;
char *windowName="Threshold Evaluation";

Component pointVec2Component(vector<cv::Point> poly, bool rev)
{	
	Component c;

	if(rev)
		for(int i= poly.size()-1; i>=0; i--)
			c.push_back(createPosition(poly[i].x, poly[i].y, 0.0));
	else
		for(int i= 0; i<poly.size(); i++)
			c.push_back(createPosition(poly[i].x, poly[i].y, 0.0));

	return c;
}

Polygon_with_holes_2 piece2PolygonWithHoles(Piece a, double inv)
{
	Polygon_with_holes_2   pa;

	//cout<<"before"<<inv*0.0<<endl;
	//a.print();

	for(int i=0; i <a.getComponents().size(); i++)
	{
		Component c = a.getComponents()[i];
		Polygon_2 newComponent;

		for(int j=0; j <c.size(); j++)
			newComponent.push_back(Point_2(inv*c[j][0], inv*c[j][1]));

		if(i==0)
			pa=Polygon_with_holes_2(newComponent);
		else
			pa.add_hole(newComponent);
	}
	//if(inv<0)
	//{
	//cout<<"after"<<endl;
	//print_polygon_with_holes (pa);
	//}
	return pa;
}

Piece polygonWithHoles2Piece(Polygon_with_holes_2 pwh)
{
	//cout<<"before"<<endl;
	//print_polygon_with_holes (pwh);
	Piece pa = polygon2Piece(pwh.outer_boundary());
	
	//Polygon_with_holes_2::Hole_const_iterator  hit;
	for (Polygon_with_holes_2::Hole_iterator hit = pwh.holes_begin(); hit != pwh.holes_end(); ++hit)
		pa.addComponent(polygon2Piece(*hit).getComponents()[0]);
	
	//cout<<"after"<<endl;
	//pa.print();
	return pa;
}

Piece polygon2Piece(Polygon_2 pwh)
{
	Piece pa;
	//cout<<"before"<<endl;
	//print_polygon_with_holes (pwh);

	Component c;
	//Polygon_2::Vertex_const_iterator  vit;
	for (Polygon_2::Vertex_iterator vit = pwh.vertices_begin(); vit != pwh.vertices_end(); ++vit)
	{
		//cout<<vit->x()<<endl;
		//cout<<vit->y()zzendl;
		c.push_back(createPosition(CGAL::to_double(vit->x()/*.exact()*/), 
								   CGAL::to_double(vit->y()/*.exact()*/) ,
								   0.0));
	}
	pa.addComponent(c);

	return pa;
}

vector<Polygon_2> invertPolygonWithHoles(Polygon_with_holes_2 l, double maxPieceWidth, double maxPieceHeight)
{
	vector<Polygon_2> layoutForInnerFitPolygon;
	CGAL::Bbox_2 bbox = l.outer_boundary().bbox();
	Polygon_2::Vertex_circulator vc = l.outer_boundary().vertices_circulator();
	
	if(l.outer_boundary().is_counterclockwise_oriented())
		l.outer_boundary().reverse_orientation();
	
	//bboxPolyBL
	Polygon_2 bboxPoly;
	bboxPoly.push_back(Point_2(bbox.xmin()-maxPieceWidth, bbox.ymin()-maxPieceHeight));

	Point_2 firstPoint(numeric_limits<double>::max(), numeric_limits<double>::max());//vertice de baixo a direita
	Polygon_2::Vertex_circulator firstPointCirculator;			//iterator para vertice de baixo a direita
	Polygon_2::Vertex_const_circulator searchBegin = vc;		//inicio da pesquisa do vertice "firstPointCirculator"
	do{
		if(vc->y() == bbox.ymin() && vc->x() < firstPoint.x())	//se vc e um vertice de baixo a direita que "firstPoint"
		{	//gravar esse vertice em "firstPoint" e respectivo iterator em "firstPointCirculator"
			firstPoint=*vc;
			firstPointCirculator=vc;
		}
	}while(++vc != searchBegin);
	bboxPoly.push_back(firstPoint);

	vc=firstPointCirculator;	//colocar o iterator "vc" a apontar para o primeiro ponto ("firstPointCirculator")
	Polygon_2::Vertex_const_circulator begin = vc++;
	for(;vc != begin;vc++)
		bboxPoly.push_back(*vc);

	bboxPoly.push_back(Point_2(vc->x()+layout.getResolution()/2, vc->y()));
	bboxPoly.push_back(Point_2(bbox.xmin()-maxPieceWidth+layout.getResolution()/2, vc->y()-maxPieceHeight));
	bboxPoly.push_back(Point_2(bbox.xmax()+maxPieceWidth, bbox.ymin()-maxPieceHeight));
	bboxPoly.push_back(Point_2(bbox.xmax()+maxPieceWidth, bbox.ymax()+maxPieceHeight));
	bboxPoly.push_back(Point_2(bbox.xmin()-maxPieceWidth, bbox.ymax()+maxPieceHeight));
	
	layoutForInnerFitPolygon.push_back(bboxPoly);
	for (Polygon_with_holes_2::Hole_iterator hit = l.holes_begin(); hit != l.holes_end(); ++hit)
	{
		if(l.outer_boundary().is_counterclockwise_oriented())
			hit->reverse_orientation(); 
		layoutForInnerFitPolygon.push_back(Polygon_2(*hit));
	}
	//print_polygon(bboxPoly);
	return layoutForInnerFitPolygon;
}

vector<Piece> contourVecPieceVec(vector<vector<cv::Point>> contours)
{
	vector<Piece> pieces;
	
	for(int i=0; i<contours.size(); i++)
	{
		Piece p;
		p.addComponent(pointVec2Component(contours[i]));
		pieces.push_back(p);
	}
	return pieces;
}

vector<Piece> contourVecPieceVec(vector<vector<cv::Point>> contours, vector<cv::Vec4i> hierarchy)
{
	map<int, int> contoursPiecesMap;
	vector<Piece> pieces;
	
	int pieceCnt=0;
	int i;
	
	//cout<<"PAI"<<endl<<endl;
	for(i=0; i < hierarchy.size() && hierarchy[i][3]!=-1; i++);  //determinar o primeiro contorno pai
	for(; i < hierarchy.size() && i>=0; i=hierarchy[i][0])		 //contornos pai determinam o numero de peças
	{
		//cout<<"Contour "<<i<<endl;
		//cout<<"next (same level) "<<hierarchy[i][0]<<"; previous (same level)"<<hierarchy[i][1]<<"; child "<<hierarchy[i][2]<<"; parent "<<hierarchy[i][3]<<endl;
		if(hierarchy[i][3]<0) 
		{
			Piece p;
			p.addComponent(pointVec2Component(contours[i]));
			pieces.push_back(p);
			contoursPiecesMap[i]=pieceCnt++;
		}	
	}

	//cout<<"FILHO"<<endl<<endl;
	for(i=0; i < hierarchy.size() && hierarchy[i][3]==-1; i++);  //determinar o primeiro contorno filho
	for(; i < hierarchy.size() && i>=0; i=hierarchy[i][0])		 //contornos filho determinam o numero de peças
	{
		//cout<<"Contour "<<i<<endl;
		//cout<<"next (same level) "<<hierarchy[i][0]<<"; previous (same level)"<<hierarchy[i][1]<<"; child "<<hierarchy[i][2]<<"; parent "<<hierarchy[i][3]<<endl;
		pieces[contoursPiecesMap[hierarchy[i][3]]].addComponent(pointVec2Component(contours[i]));	
	}
	return pieces;
}

IplImage *getOpenCVImage(int xb, int yb, int width, int height, int channel)
{
	IplImage *img;
	int format;
	
	if(channel!= GL_BLUE && channel!= GL_RED  && channel!= GL_GREEN) 
	{
		format = GL_BGR_EXT;
		img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U,3);
	}else{
		format=channel;
		img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U,1);
	}

	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(xb,yb,width,height,format,GL_UNSIGNED_BYTE, img->imageData);
	return img;
}

pair<vector<vector<cv::Point>>, vector<cv::Vec4i>> getContours(IplImage *img, double maxDist)
{
	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(cv::Mat(img), contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	vector<Piece> pieces = contourVecPieceVec(contours);
	ofstream f;
	/*
	f.open("pieces.txt");
	f<<"Antes: "<<endl;
	for(int i=0; i < pieces.size(); i++)
	{
		f<<"Piece "<<i<<endl;
		f<<pieces[i];
	}
	*/
	vector<vector<cv::Point>> contoursSimplification;
	for(int i=0; i < contours.size(); i++)
	{
		contoursSimplification.push_back(vector<cv::Point>());
		cv::approxPolyDP(contours[i], contoursSimplification[i], maxDist, true);
	}

	
	//cout<<"Depois: "<<endl;
	pieces = contourVecPieceVec(contoursSimplification);
	/*
	for(int i=0; i < pieces.size(); i++)
	{
		cout<<"Piece "<<i<<endl;
		pieces[i].print();
	}
	*/
	return pair<vector<vector<cv::Point>>, vector<cv::Vec4i>> (contoursSimplification, hierarchy);
}

void drawContour(IplImage *img, vector<cv::Point> contour, cv::Scalar color)
{
	cv::Mat imgMat(img);
	vector<vector<cv::Point>> contours;
	contours.push_back(contour);
	cv::drawContours(imgMat, contours, 0, color);
}

vector<vector<cv::Point>> showContours(IplImage *thrImg)
{
	cv::Mat thrImgMat(thrImg);
	cv::Mat cntrImgMat(thrImgMat.rows, thrImgMat.cols, CV_8UC1, cv::Scalar(0));
	cv::Mat invCntrImgMat(thrImgMat.rows, thrImgMat.cols, CV_8UC1, cv::Scalar(0));
	cv::Mat displayImgMat(thrImgMat.rows*2/*3*/, thrImgMat.cols, CV_8UC1, cv::Scalar(0));
	cv::Mat roi;

	vector<vector<cv::Point>> contours;
	vector<vector<cv::Point>> invContours;
	vector<cv::Vec4i> hierarchy;
	vector<cv::Vec4i> invHierarchy;
	
	//desenhar imagem inicial
	roi = cv::Mat(displayImgMat, cv::Rect(cv::Point(0, 0), thrImgMat.size()));
	thrImgMat.copyTo(roi);

	//desenhar contornos invertidos
	cvThreshold(thrImg, &(IplImage)invCntrImgMat, 0, 255, CV_THRESH_BINARY_INV);
	pair<vector<vector<cv::Point>>, vector<cv::Vec4i>> contoursPair = getContours(&(IplImage)invCntrImgMat, 1.0);
	contours = contoursPair.first;
	cv::drawContours(invCntrImgMat, contours, -1, cv::Scalar(255));
	roi = cv::Mat(displayImgMat, cv::Rect(cv::Point(0, invCntrImgMat.rows/*2*/), invCntrImgMat.size()));
	invCntrImgMat.copyTo(roi);

	//mostrar imagem
	cvConvertImage(&(IplImage)displayImgMat,  &(IplImage)displayImgMat, CV_CVTIMG_FLIP);
#ifdef DEBUG
	cvShowImage(windowName, &(IplImage)displayImgMat);
#endif
	cntrImgMat.release();
	invCntrImgMat.release();
	displayImgMat.release();
	return contours;
}

void updateThresholdImage(int bar, void *img)
{
	IplImage *thrImg = cvCreateImage(cvSize(((IplImage*)img)->width, ((IplImage*)img)->height), IPL_DEPTH_8U,1);
	threshold = (unsigned char)(intMinThreshold+(step*bar));
	cvThreshold(img, thrImg, threshold, 0, CV_THRESH_TOZERO);
	//cvShowImage(windowName, thrImg);
	showContours(thrImg);
	cvReleaseImage(&thrImg);
}

void evaluateThresholdWindow(IplImage *img, int stepInit)
{
	int maxBar;
	step=stepInit;
	if(img->nChannels>1) 
	{
		cout<<"Only grayscale images are admitted.\n"<<endl;
		return;
	}

	intMinThreshold=0;
	intMaxThreshold=255;
	//cvMinMaxLoc(img, &intMinThreshold, &intMaxThreshold);
	maxBar=((int)intMaxThreshold-(int)intMinThreshold)/step;

	cvNamedWindow(windowName, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(windowName, 110, 110);
	cvCreateTrackbar2("Threshold", windowName, &bar, maxBar, updateThresholdImage, img);

	updateThresholdImage(0,img);
}

vector<vector<cv::Point>> getFeasiblePositions(IplImage * image)
{
	return showContours(image);
}

cv::Point bottomLeft(vector<vector<cv::Point>> feasiblePositions)
{
	//escolher o ponto mais abaixo e mais à esquerda dos contornos
	cv::Point position(numeric_limits<int>::max(), numeric_limits<int>::max() );
	for(int i=0; i<feasiblePositions.size(); i++)
	{
		for(int j=0; j<feasiblePositions[i].size(); j++)
		{
			if(position.x>feasiblePositions[i][j].x && position.y>feasiblePositions[i][j].y)
			{
				position = feasiblePositions[i][j];
			}
		}
	}
	return position;
}

GLdouble* bottomLeft(vector<vector<GLdouble*>> feasiblePositions)
{
	//escolher o ponto mais abaixo e mais à esquerda dos contornos
	GLdouble *position = createPosition(numeric_limits<int>::max(), numeric_limits<int>::max(), 0.0);
	for(int i=0; i<feasiblePositions.size(); i++)
	{
		for(int j=0; j<feasiblePositions[i].size(); j++)
		{
			if(	feasiblePositions[i][j][0]<position[0] ||
				feasiblePositions[i][j][0]==position[0] && feasiblePositions[i][j][1]<position[1])
			{
				delete position;
				position = feasiblePositions[i][j];
			}
		}
	}
	
	return position;
}




//NFPS
Polygon_2 minkowskiSumConvexPoly2_ConvexHull2(Polygon_2 a, Polygon_2 b)
{
	//return minkowskiSumConvexPoly(a, b, CGAL::ch_graham_andrew );
	
	Translate_point_2       f_add;
	Construct_vector_2      f_vector;
	Polygon_2 sum; 
	//print_polygon(a);
	//print_polygon(b);
	for (Polygon_2::Vertex_iterator vit = a.vertices_begin(); vit != a.vertices_end(); ++vit)
		for (Polygon_2::Vertex_iterator vjt = b.vertices_begin(); vjt != b.vertices_end(); ++vjt)
			sum.push_back(f_add (*vit, f_vector(CGAL::ORIGIN, *vjt)));
	
	Polygon_2 hull;	
	//CGAL::ch_graham_andrew (sum.vertices_begin(), sum.vertices_end(), back_inserter(hull));
	CGAL::convex_hull_2 (sum.vertices_begin(), sum.vertices_end(), back_inserter(hull));
	//print_polygon(hull);
  return hull;
}

vector<list<Polygon_2>> decomposePiecesGreene (vector<Piece> pieces)
{
	
	clock_t begin, end;
	
	CGAL::Greene_convex_decomposition_2<Kernel> decomp;
	vector<list<Polygon_2>> result;
	
	for(int i=0; i< pieces.size(); i++)
	{
		//Create polygons
		Polygon_with_holes_2 pa = piece2PolygonWithHoles(pieces[i]);
		list<Polygon_2>   subPolyPa;
		decomp (pa.outer_boundary(), back_inserter(subPolyPa));
		result.push_back(subPolyPa);
	}
	return result;
}

vector<list<Polygon_2>> decomposePiecesOptimal (vector<Piece> pieces)
{
	
	clock_t begin, end;
	
	CGAL::Optimal_convex_decomposition_2<Kernel> decompO;
	vector<list<Polygon_2>> result;
	
	for(int i=0; i< pieces.size(); i++)
	{
		//Create polygons
		Polygon_with_holes_2 pa = piece2PolygonWithHoles(pieces[i]);
		list<Polygon_2>   subPolyPa;
		decompO (pa.outer_boundary(), back_inserter(subPolyPa));
		result.push_back(subPolyPa);
	}
	return result;
}

vector<list<Polygon_2>> decomposePiecesHertMehl(vector<Piece> pieces)
{
	
	clock_t begin, end;
	
	CGAL::Hertel_Mehlhorn_convex_decomposition_2<Kernel> decompHM;
	vector<list<Polygon_2>> result;
	
	for(int i=0; i< pieces.size(); i++)
	{
		//Create polygons
		Polygon_with_holes_2 pa = piece2PolygonWithHoles(pieces[i]);
		list<Polygon_2>   subPolyPa;
		decompHM (pa.outer_boundary(), back_inserter(subPolyPa));
		result.push_back(subPolyPa);
	}
	return result;
}

vector<list<Polygon_2>> decomposePiecesAngBis (vector<Piece> pieces)
{
	clock_t begin, end;
	CGAL::Small_side_angle_bisector_decomposition_2<Kernel> decompAB;
	vector<list<Polygon_2>> result;
	
	for(int i=0; i< pieces.size(); i++)
	{
		//Create polygons
		Polygon_with_holes_2 pa = piece2PolygonWithHoles(pieces[i]);
		list<Polygon_2>   subPolyPa;
		decompAB (pa.outer_boundary(), back_inserter(subPolyPa));
		result.push_back(subPolyPa);
	}
	return result;
}

vector<Polygon_2>** decomposePiecesAngBis (Polygon_with_holes_2** pieces, int dim1, int dim2)
{
	clock_t begin, end;
	CGAL::Small_side_angle_bisector_decomposition_2<Kernel> decompAB;
	vector<Polygon_2>** result;
	
	result=new vector<Polygon_2>*[dim1];
	for(int i=0; i< dim1; i++)
	{
		//cout<<i<<endl;
		result[i]=new vector<Polygon_2>[dim2];
		if(!(pieces[i][0].outer_boundary()).is_simple())
		{
			cout<<"Error polygon "<<i<<":\n";
			print_polygon(pieces[i][0].outer_boundary());
		}

		for(int j=0; j< dim2; j++)
		{
			//cout<<"   "<<j<<endl;
			//Create polygons
			vector<Polygon_2> subPolyPa;
			decompAB (pieces[i][j].outer_boundary(), back_inserter(subPolyPa));
			result[i][j]=subPolyPa;
		}
	}
	return result;
}


vector<list<Polygon_2>> decomposePiecesAngBis (vector<Polygon_with_holes_2> pieces)
{
	
	clock_t begin, end;
	CGAL::Small_side_angle_bisector_decomposition_2<Kernel> decompAB;
	vector<list<Polygon_2>> result;
	
	for(int i=0; i< pieces.size(); i++)
	{
		//Create polygons
		
		list<Polygon_2>   subPolyPa;
		decompAB (pieces[i].outer_boundary(), back_inserter(subPolyPa));
		result.push_back(subPolyPa);
	}
	return result;
}

vector<Polygon_2> minkowskiSumsWithDecompositionNoConversion(list<Polygon_2>   subPolyPa, list<Polygon_2>   subPolyPb)
{
	clock_t begin, end;
	Transformation scale(CGAL::SCALING, -1);
	
	list<Polygon_2> subPolySymPb;
	for(list<Polygon_2>::iterator jt=subPolyPb.begin(); jt != subPolyPb.end(); jt++){
		Polygon_2 p;
		for(int i=0; i <jt->size(); i++){
			p.push_back((*jt)[i].transform(scale));
		}
		subPolySymPb.push_back(p);
	}
	
	//calcular as somas
	vector<Polygon_2> sums;
	for(list<Polygon_2>::iterator it=subPolyPa.begin(); it != subPolyPa.end(); it++)
		for(list<Polygon_2>::iterator jt=subPolySymPb.begin(); jt != subPolySymPb.end(); jt++){		
			sums.push_back(minkowskiSumConvexPoly2_ConvexHull2(*it, *jt));		 
		}
	return sums;
}

vector<Polygon_2> minkowskiSumsWithDecompositionNoConversion(vector<Polygon_2>   subPolyPa, vector<Polygon_2>   subPolyPb)
{
	clock_t begin, end;
	//Transformation rotate(CGAL::ROTATION, sin(M_PI), cos(M_PI));
	Transformation scale(CGAL::SCALING, -1);

	vector<Polygon_2> subPolySymPb;
	for(vector<Polygon_2>::iterator jt=subPolyPb.begin(); jt != subPolyPb.end(); jt++){
		Polygon_2 p;
		for(int i=0; i <jt->size(); i++){
			p.push_back((*jt)[i].transform(scale));
		}
		subPolySymPb.push_back(p);
	}

	//calcular as somas
	vector<Polygon_2> sums;
	for(vector<Polygon_2>::iterator it=subPolyPa.begin(); it != subPolyPa.end(); it++)
		for(vector<Polygon_2>::iterator jt=subPolySymPb.begin(); jt != subPolySymPb.end(); jt++){		
			sums.push_back(minkowskiSumConvexPoly2_ConvexHull2(*it, *jt));		 
		}

	/*
	vector<Polygon_2> sums;
	for(list<Polygon_2>::iterator it=subPolyPa.begin(); it != subPolyPa.end(); it++)
		for(list<Polygon_2>::iterator jt=subPolyPb.begin(); jt != subPolyPb.end(); jt++){
			for(int i=0; i <jt->size(); i++){
				(*jt)[i]=(*jt)[i].transform(scale);
			}
			cout<<"ola"<<endl;
			print_polygon(*jt);
			sums.push_back(minkowskiSumConvexPoly2_ConvexHull2(*it, *jt));		 
		}
	*/
	return sums;
}

Point_2 chooseRandomPiece(vector<int> piecesToPlace, map<Point_2, PolyInfo> pieceRotationInfo)
{
	vector<Point_2> piecesOrdered;
	for(map<Point_2, PolyInfo>::iterator itInfo = pieceRotationInfo.begin();
		itInfo != pieceRotationInfo.end();
		itInfo++)
	{
		int piece = (int)itInfo->first.x();
		int rotation = (int)itInfo->first.y();

		if(piecesToPlace[piece]>0)
			piecesOrdered.push_back(itInfo->first);
	}

	if(piecesOrdered.size() == 0) throw exception();
	return piecesOrdered[rand()%piecesOrdered.size()];
}

/*
Point_2 chooseLargerPiece(vector<int> piecesToPlace, map<Point_2, PolyInfo> pieceRotationInfo)
{
	vector<Point_2> piecesOrdered;
	for(map<Point_2, PolyInfo>::iterator itInfo = pieceRotationInfo.begin();
		itInfo != pieceRotationInfo.end();
		itInfo++)
	{
		int piece = (int)itInfo->first.x();
		int rotation = (int)itInfo->first.y();

		if(piecesToPlace[piece]>0)
		{
			vector<Point_2>::iterator j=piecesOrdered.begin();
			for(;	j != piecesOrdered.end() &&
					pieceRotationInfo[*j].area > pieceRotationInfo[itInfo->first].area;
				j++);
			piecesOrdered.insert(j, itInfo->first);
		}
	}

	if(piecesOrdered.size() == 0) throw exception();
	return piecesOrdered[0];
}


Point_2 choosePieceGreaterWidth(vector<int> piecesToPlace, map<Point_2, PolyInfo> pieceRotationInfo)
{
	vector<Point_2> piecesOrdered;
	for(map<Point_2, PolyInfo>::iterator itInfo = pieceRotationInfo.begin();
		itInfo != pieceRotationInfo.end();
		itInfo++)
	{
		int piece = (int)itInfo->first.x();
		int rotation = (int)itInfo->first.y();

		if(piecesToPlace[piece]>0)
		{
			vector<Point_2>::iterator j=piecesOrdered.begin();
			for(;	j != piecesOrdered.end() &&
					pieceRotationInfo[*j].width > pieceRotationInfo[itInfo->first].width;
				j++);
			piecesOrdered.insert(j, itInfo->first);
		}
	}

	if(piecesOrdered.size() == 0) throw exception();
	return piecesOrdered[0];
}




Point_2 choosePieceGreaterHeight(vector<int> piecesToPlace, map<Point_2, PolyInfo> pieceRotationInfo)
{
	vector<Point_2> piecesOrdered;
	for(map<Point_2, PolyInfo>::iterator itInfo = pieceRotationInfo.begin();
		itInfo != pieceRotationInfo.end();
		itInfo++)
	{
		int piece = (int)itInfo->first.x();
		int rotation = (int)itInfo->first.y();

		if(piecesToPlace[piece]>0)
		{
			vector<Point_2>::iterator j=piecesOrdered.begin();
			for(;	j != piecesOrdered.end() &&
					pieceRotationInfo[*j].height > pieceRotationInfo[itInfo->first].height;
				j++);
			piecesOrdered.insert(j, itInfo->first);
		}
	}

	if(piecesOrdered.size() == 0) throw exception();
	return piecesOrdered[0];
}
*/
Polygon_2 removeRedundantVertices(Polygon_2 a)
{

	Polygon_2 a2;
	a2.push_back(a[0]);
	int a2Index=0;
	//print_polygon(a);
	for(int i=1; i < a.size(); i++)
	{
		//cout<<"i: "<<i<<"/"<<a.size()<<endl;
		//cout<<(i+1)%a.size()<<endl<<(i+2)%a.size()<<endl;
		
		int i2=i%a.size();
		int i3=(i+1)%a.size();
		
		Point_2 p1=a2[a2Index];
		Point_2 p2=a[i2];
		Point_2 p3=a[i3]; //p3 = a2[0] se i == a.size()-2 (penultimo elem)

		Vector_2 edge1(p1, p2);
		Vector_2 edge2(p2, p3);
		
		if(edge1.direction()!=edge2.direction())
		{
			if(i2<a.size())
			{
				a2.push_back(p2);
				a2Index++;
			}
		}else
		{
			if(i2>=a.size())
			{
				a2.erase(a2.vertices_begin());
			}
		}
	}
	return a2;

}

Polygon_2  sortEdgesConvexPoly(Polygon_2 a, Polygon_2 b)
{
	Construct_vector_2 f_vector;
	list<Vector_2> slopes;

	CGAL::Bbox_2 bboxA = a.bbox();
	Point_2 lastPoint(numeric_limits<double>::max(), numeric_limits<double>::max());//vertice da direita mais abaixo
	list<Vector_2>::iterator lastPointIterator;										//iterator para vertice da direita mais abaixo
	list<Vector_2>::iterator itLastInsertedElement;
	
	a=removeRedundantVertices(a);
	b=removeRedundantVertices(b);
	
	//Ordenar e adicionar vectores do poligono A
	Polygon_2::Vertex_const_circulator a_it_begin = a.vertices_circulator();
	Polygon_2::Vertex_circulator a_it = a_it_begin;
	do{

		Point_2 p0 = *a_it;
		Point_2 p1 = *(++a_it);
		Vector_2 slope(p0, p1);

		list<Vector_2>::iterator it;
		for(it = slopes.begin(); it != slopes.end() && it->direction()<slope.direction(); it++);
		itLastInsertedElement = slopes.insert(it, slope);

		if(p0.x() == bboxA.xmax() && p0.y() < lastPoint.y()) //se vc e um vertice a esquerda e esta mais abaixo que "firstPoint"
		{
			//gravar esse vertice em "firstPoint" e respectivo iterator em "firstPointCirculator"
			lastPoint=p0;
			lastPointIterator=itLastInsertedElement;
		}

	}while(a_it != a_it_begin);
	
	//Ordenar e adicionar vectores do poligono B
	Polygon_2::Vertex_const_circulator b_it_begin = b.vertices_circulator();
	Polygon_2::Vertex_circulator b_it = b_it_begin;

	CGAL::Bbox_2 bboxB = b.bbox();
	Point_2 leftBottomPoint(numeric_limits<double>::max(), numeric_limits<double>::max());//vertice da direita mais abaixo
	
	list<Vector_2>::iterator itCurrentlyInsertedVector=slopes.end();
	list<Vector_2>::iterator itLastInsertedVector=slopes.end();
	list<Vector_2>::iterator itLeftBottomPoint=slopes.end();
	itLastInsertedElement=slopes.end();
	do{
		Point_2 p0 = *b_it;
		Point_2 p1 = *(--b_it);
		Vector_2 slope(p0, p1);

		list<Vector_2>::iterator it;
		for(it = slopes.begin(); it != slopes.end() && it->direction()<slope.direction(); it++);

		itLastInsertedVector = itCurrentlyInsertedVector;
		itCurrentlyInsertedVector = slopes.insert(it, slope);
		
		if(p0.x() == bboxB.xmin() && p0.y() < leftBottomPoint.y()) //se vc e um vertice a esquerda e esta mais abaixo que "firstPoint"
		{
			leftBottomPoint=p0;	//gravar esse vertice em "firstPoint" e respectivo iterator em "firstPointCirculator"
			itLeftBottomPoint=itLastInsertedVector;	//itLeftBottomPointAfter=itCurrentlyInsertedVector;
		}
	}while(b_it != b_it_begin);

	//se leftBottomPoint nao foi inicializado e porque se comecou por esse ponto e o vector que lhe corresponde e o ultimo adicionado (itCurrentlyInsertedVector)
	if(itLeftBottomPoint==slopes.end())		itLeftBottomPoint=itCurrentlyInsertedVector;
	//se leftBottomPoint tiver menor slope que lastPointIterator, defini-lo como o vector inicial
	if(itLeftBottomPoint->direction()<=lastPointIterator->direction())	lastPointIterator=itLeftBottomPoint;
	
	lastPoint=lastPoint+Vector_2(leftBottomPoint, CGAL::ORIGIN);
	
	//Somar os vectores por ordem partindo de lastPoint
	Translate_point_2 f_add;
	Polygon_2 sum;

	Polygon_2::Vertex_iterator lastInserted;
	Vector_2 lastV = CGAL::NULL_VECTOR;
	for (list<Vector_2>::iterator it=lastPointIterator; it!=slopes.end(); it++)
	{
		Vector_2 v = *it;
		Point_2 nextPoint = f_add(lastPoint, v);
		
		if(v.direction() == lastV.direction())
		{
			sum.erase(lastInserted);
		}
		lastInserted=sum.insert(sum.vertices_end(), nextPoint);
		lastPoint = *lastInserted;
		lastV = v;
	}
	for (list<Vector_2>::iterator it=slopes.begin(); it!=lastPointIterator; it++)
	{
		Vector_2 v = *it;
		Point_2 nextPoint = f_add(lastPoint, v);
		
		if(v.direction() == lastV.direction())
		{
			sum.erase(lastInserted);
		}
		lastInserted=sum.insert(sum.vertices_end(), nextPoint);
		lastPoint = *lastInserted;
		lastV = v;
	}
	
	//Eliminar o vertice inicial se este estiver entre dois vectores com a mesma direcçao
	Polygon_2::Vertex_circulator sumIt=sum.vertices_circulator();

	while(*(--sumIt)!=lastPoint);

	Point_2 after  = *(++sumIt);
	Point_2 before = *(--(--sumIt));
 
	if(Vector_2(before, after).direction() == Vector_2(lastPoint, after).direction())
		sum.erase(lastInserted);

	return sum;
}

vector<Polygon_2> sortEdgesWithDecomposition(vector<Polygon_2>   subPolyPa, vector<Polygon_2>   subPolyPb)
{
	vector<Polygon_2> sums;

	for(vector<Polygon_2>::iterator it=subPolyPa.begin(); it != subPolyPa.end(); it++)
	{
		for(vector<Polygon_2>::iterator jt=subPolyPb.begin(); jt != subPolyPb.end(); jt++)
		{
			//sort
			sums.push_back(sortEdgesConvexPoly(*it, *jt));
		}
	}
	return sums;

}

vector<Polygon_2> sortEdgesWithDecomposition(list<Polygon_2>   subPolyPa, list<Polygon_2>   subPolyPb)
{
	vector<Polygon_2> sums;

	for(list<Polygon_2>::iterator it=subPolyPa.begin(); it != subPolyPa.end(); it++)
	{
		for(list<Polygon_2>::iterator jt=subPolyPb.begin(); jt != subPolyPb.end(); jt++)
		{
			//sort
			sums.push_back(sortEdgesConvexPoly(*it, *jt));
		}
	}
	return sums;

}
