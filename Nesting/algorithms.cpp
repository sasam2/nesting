#include "algorithms.h"


void buildStaticLayout()
{
	ofstream myfile;
	char filename[100];
	sprintf(filename, "staticTests %s - %d.csv", problemName, staticHeuristic);
	myfile.open (filename);
	myfile<<"Iteration; Placement time ; Updating time"<<endl;

	clock_t begin=clock();
	bool lastIterationSuccessful=false;
	while(currentDrawingNFPs.size()>0)
		lastIterationSuccessful=displayStatic(&myfile);
	if(!lastIterationSuccessful)
		cout<<"ERROR: not all pieces could be placed! Terminating."<<endl;
	else
		cout<<"SUCCESS: all pieces placed!"<<endl;
	clock_t end=clock();

	myfile<<"TOTAL;"<<diffclock(end, begin)<<endl;
	double maxLength=calculateMaxLength();
	myfile<<"LENGTH;"<<calculateMaxLength()<<endl;
	myfile<<"USED AREA;"<<calculateUsedArea(maxLength)<<endl;
	myfile<<"WIDTH;"<<layout.stockSheetsBoundingBox().getHeight();
	myfile.close();
}

void buildDynamicLayout()
{
	ofstream myfile;
	char filename[100];
	sprintf(filename, "dynamicTests %s.csv", problemName);
	myfile.open (filename);
	myfile<<"Iteration; Placement time ; Updating time; Placed pieces; Avaliable types; Placed piece; Time positioning1;Time positioning2; Time drawing1; Time drawing2"<<endl;
	//*myfile<<iteration<<";"<<placementTime<<";"<<updatingTime<<";"<<currentDrawingPolys.getPositions().size()<<";"<<drawingNFPsTest.size()<<";"<<placedPiece<<";"<<findPositioningTime<<";"<<addPlacementTime<<"; "<<drawLayoutsTime<<"; "<<savePBOTime<<";"<<endl;
	
	clock_t begin=clock();
	bool lastIterationSuccessful=false;
	while(currentDrawingNFPs.size()>0)
		lastIterationSuccessful=displayDynamic(&myfile);
	if(!lastIterationSuccessful)
		cout<<"ERROR: not all pieces could be placed! Terminating."<<endl;
	else
		cout<<"SUCCESS: all pieces placed!"<<endl;
	clock_t end=clock();

	myfile<<"TOTAL;"<<diffclock(end, begin)<<endl;
	double maxLength=calculateMaxLength();
	myfile<<"LENGTH;"<<calculateMaxLength()<<endl;
	myfile<<"USED AREA;"<<calculateUsedArea(maxLength)<<endl;
	myfile<<"WIDTH;"<<layout.stockSheetsBoundingBox().getHeight();
	myfile.close();
}

double calculateMaxLength()
{
	GLdouble max=0;
	for(int i=0; i<currentDrawingPolys.getOrder().size(); i++)
	{
		int piece = currentDrawingPolys.getOrder()[i].first;
		int rotation = currentDrawingPolys.getOrder()[i].second;
		GLdouble *position = currentDrawingPolys.getPositions()[i];
		GLdouble maxy = position[0]+(polygons[piece][rotation].bbox().xmax()-polygons[piece][rotation].bbox().xmin());
		max=myMax(maxy, max);
	}
	return max;
}

double calculateUsedArea(double maxLength)
{
	double totalarea=maxLength*layout.stockSheetsBoundingBox().getHeight();
	cout<<maxLength<<" x "<<layout.stockSheetsBoundingBox().getHeight()<<" = "<<totalarea<<endl;
	double ocupiedarea=0;
	//double wastearea=totalarea;
	cout<<numberOfPolygons<<endl;
	cout<<layout.getPieces().size()<<endl;
	for(int i=0; i < numberOfPolygons; i++)
	{
		double pieceArea=polygons[i][0].outer_boundary().area();
		for(Polygon_with_holes_2::Hole_iterator h = polygons[i][0].holes_begin();
			h != polygons[i][0].holes_end();	h++)
			pieceArea+=h->area();
		ocupiedarea+=pieceArea*layout.getQuantity()[i];
		cout<<pieceArea<<" x "<<layout.getQuantity()[i]<<endl;
		//wastearea-=pieceArea*layout.getQuantity()[i];
	}
	cout<<ocupiedarea<<endl;
	//cout<<wastearea/totalarea<<endl;
	cout<<ocupiedarea/totalarea<<endl<<endl;
	return ocupiedarea/totalarea;
}

bool displayStatic(ofstream *myfile)
{
	int previousBind=0;
	static vector<int> piecesToPlace;

	if(iteration == 0)
	{
		glDisable(GL_BLEND);
		GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
		//blue=createColor(0.0,0.0,1.0,1.0);
		piecesToPlace=layout.getQuantity();
		/*
		for(map<Point_2, DrawingWithRotations>::iterator itNFPsOfPiecesToPlaceIndex=currentDrawingNFPs.begin();
		itNFPsOfPiecesToPlaceIndex != currentDrawingNFPs.end();
		itNFPsOfPiecesToPlaceIndex++)
		{
			DrawingWithRotations *d=&itNFPsOfPiecesToPlaceIndex->second;
			glClear(GL_COLOR_BUFFER_BIT);
			glColor3f(0.0,0.0,1.0);
			for(int i=0; i < d->getPositions().size(); i++)
			{
				glPushMatrix();
				//glColor4f	(d->getColors()[i][0], d->getColors()[i][1], d->getColors()[i][2], d->getColors()[i][3]);
				glTranslatef(d->getPositions()[i][0], d->getPositions()[i][1], d->getPositions()[i][2]);
				glCallList	(d->getLists()[d->getOrder()[i].first][d->getOrder()[i].second]);
				glPopMatrix	();
			}
			//d->bufferObject=drawingNFPsTest[Point_2(0,0)][itNFPsOfPiecesToPlaceIndex->first].bufferObject;
			//IplImage *imgTest = cvCreateImage(cvSize(wWidth, wHeight), IPL_DEPTH_8U,1);

			int previousBind=0;
			glReadBuffer(GL_BACK_LEFT);
			glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, d->bufferObject);
			cout<<glGetError()<<endl;
			glReadPixels(tx, ty, tw, th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
			cout<<glGetError()<<endl;
			glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);

			int p=(int)itNFPsOfPiecesToPlaceIndex->first.x();
			int r=(int)itNFPsOfPiecesToPlaceIndex->first.y();

	//d->bufferObject=drawingNFPsTest[Point_2(0,0)][itNFPsOfPiecesToPlaceIndex->first].bufferObject;
	//#ifdef DEBUG
			IplImage *imgTest = cvCreateImage(cvSize(tw, th), IPL_DEPTH_8U,1);
			char* initPtr = imgTest->imageData;
			glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, d->bufferObject);
			imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY);
			char imageName[100];
			sprintf(imageName, "INIT %d %d.jpg", p, r);
			cvSaveImage(imageName, imgTest);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
			imgTest->imageData=initPtr;
			cvReleaseImage(&imgTest);
	//#endif
		}
		*/
	}

	//definir dimensoes do viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	BoundingBox layoutBB = currentDrawingNFPs.begin()->second.getViewPort();
	gluOrtho2D(layoutBB.getX(), layoutBB.getWidth(), layoutBB.getY(), layoutBB.getHeight());
	glMatrixMode(GL_MODELVIEW);

	//escolher a proxima peça
	Point_2 nextPiece;
	int nextPolygonIndex;
	GLdouble *nextPlacement;

	//escolher uma peça e uma rotacao
	nextPiece=*piecesOrdered.begin();
	nextPolygonIndex=(int)nextPiece.x();

	//procurar posição para essa peça
	clock_t begin=clock();
	/*
	//desenhar nfps dessa peça com o layout
	glClear(GL_COLOR_BUFFER_BIT);
	DrawingWithRotations *d=&currentDrawingNFPs[nextPiece];
	glColor3f(0,0.0,1.0);
	for(int i=0; i < d->getPositions().size(); i++)
	{
		glPushMatrix();
		//glColor4f	(d->getColors()[i][0], d->getColors()[i][1], d->getColors()[i][2], d->getColors()[i][3]);
		glTranslatef(d->getPositions()[i][0], d->getPositions()[i][1], d->getPositions()[i][2]);
		glCallList	(d->getLists()[d->getOrder()[i].first][d->getOrder()[i].second]);
		glPopMatrix	();
	}
	*/
	
	//desenhar os nfps dos outros poligonos para o layout com a nova peça adicionada
	for(map<Point_2, DrawingWithRotations>::iterator itNFPsOfPiecesToPlaceIndex=currentDrawingNFPs.begin();
		itNFPsOfPiecesToPlaceIndex != currentDrawingNFPs.end();
		itNFPsOfPiecesToPlaceIndex++)
	{
		DrawingWithRotations *d=&itNFPsOfPiecesToPlaceIndex->second;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//desenhar nfps da iteracao anterior
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, d->bufferObject);
		glRasterPos3d(0.0,0.0,0.0);
		glDrawPixels(tw, th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
		
		//desenhar nfp da iteracao actual
		glColor3f(0.0, 0.0, 1.0);
		glPushMatrix();
		glTranslatef(d->getPositions().back()[0], d->getPositions().back()[1], d->getPositions().back()[2]);
		glCallList	(d->getLists()[d->getOrder().back().first][d->getOrder().back().second]);
		glPopMatrix();

		//le imagem dos nfps para o PBO
		glReadBuffer(GL_BACK_LEFT);
		glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, d->bufferObject);
	#ifdef DEBUG
		cout<<glGetError()<<endl;
	#endif
		glReadPixels(tx, ty, tw, th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
	#ifdef DEBUG
		cout<<glGetError()<<endl;
	#endif
		glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);

		//se for o pbo da peça que foi colocada, ler imagem para memoria
		if(itNFPsOfPiecesToPlaceIndex->first.x()==nextPiece.x() && itNFPsOfPiecesToPlaceIndex->first.y()==nextPiece.y())
		{
			//le PBO para openCV
			glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, d->bufferObject);

			//determinar a posicao da peça
			IplImage *imgTest = cvCreateImage(cvSize(tw, th), IPL_DEPTH_8U, 1);
			imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY);

			//DEBUG guardar imagem
			#ifdef DEBUG
				char imageName[100];
				sprintf(imageName, "iteration %d piece %d %d piecesPlaced %d .jpg", iteration, (int)nextPiece.x(), (int)nextPiece.y(), currentDrawingNFPs.begin()->second.getPositions().size());
				cvSaveImage(imageName, imgTest);
			#endif

			nextPlacement = getPiecePosition(imgTest);

			//se nao for possivel colocar essa peça terminar
			if(nextPlacement==NULL)
			{
				layoutNFPsTest.clear();
				currentDrawingNFPs.clear();
				drawingNFPsTest.clear();
				drawingPolysTest.clear();
				return false;
			}
	
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
			cvReleaseImage(&imgTest);
		}
	}

	
	clock_t end=clock();
	double timePlacement=diffclock(end, begin);
	begin=clock();


	
	
	end=clock();
	double timeUpdating=diffclock(end, begin);
	//colocar a peça nos desenhos dos poligonos e dos nfps
	currentDrawingPolys.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, blue);

#ifdef DEBUG
	cout<<"Piece: "<<currentDrawingPolys.getOrder().back().first<<" "<<currentDrawingPolys.getOrder().back().second<<endl;
	cout<<"Position: "<<currentDrawingPolys.getPositions().back()[0]<<" "<<currentDrawingPolys.getPositions().back()[1]<<endl;
	cout<<"List: "<<currentDrawingPolys.getLists()[currentDrawingPolys.getOrder().back().first][currentDrawingPolys.getOrder().back().second]<<endl;
#endif
	
	//currentDrawingNFPs
	for(map<Point_2, DrawingWithRotations>::iterator itDrawing= currentDrawingNFPs.begin(); 
		itDrawing != currentDrawingNFPs.end();
		itDrawing++)
	{
		itDrawing->second.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, blue);
	}
	
	//apagar a peça da lista de peças a colocar
	piecesOrdered.erase(piecesOrdered.begin());
	if(piecesOrdered.empty()){
		currentDrawingNFPs.clear();
		//glDeleteBuffers(1, &pboNFPs);
	}
	
	*myfile<<iteration<<"; "<<timePlacement<<";"<<timeUpdating<<endl;

	iteration++;
	return true;
}

Point_2 putPieceDynamic(bool reset)
{
	static int *placedPieces;
	static GLfloat *blue;
	static GLfloat *green;
	static vector<int> piecesToPlace;
	
	if(reset)
	{
		blue=createColor(0.0,0.0,1.0,1.0/numberOfPolygons);
		green=createColor(0.0,1.0,0.0,0.1);
		delete placedPieces;
		placedPieces=new int[layout.getPieces().size()];
		
		for(int i=0; i < layout.getPieces().size(); i++)
		{
			placedPieces[i] = 0;
			piecesToPlace.push_back(layout.getQuantity()[i]);
		}
		return Point_2();
	}else
	{
		Point_2 nextPiece= dynamicPieceSelection();

		int nextPolygonIndex=nextPiece.x();
		placedPieces[nextPolygonIndex]++;
		piecesToPlace[nextPolygonIndex]--;
#ifdef DEBUG
		cout<<"Next piece: "<<nextPiece.x()<<" "<<nextPiece.y()<<endl;
		cout<<"Pieces to place:"<<endl;
		for(int i=0; i <numberOfPolygons; i++){
			cout<<"Peca "<<i<<": "<<piecesToPlace[i]<<"/"<<layout.getQuantity()[i]<<"; ";
		}
		cout<<endl<<endl;
#endif
		//save next placement pieces drawings
		GLdouble *nextPlacement = drawingPolysTest[nextPiece].getPositions().back();
#ifdef DEBUG
		cout<<"Next piece position: "<<nextPlacement[0]<<" "<<nextPlacement[1]<<endl;
#endif
		for(map<Point_2, DrawingWithRotations>::iterator itImage= currentDrawingNFPs.begin(); 
			itImage != currentDrawingNFPs.end();
			itImage++)
		{
			GLuint bufferObjectBackup=itImage->second.bufferObject;
			itImage->second.bufferObject=drawingNFPsTest[nextPiece][itImage->first].bufferObject;
			drawingNFPsTest[nextPiece][itImage->first].bufferObject=bufferObjectBackup;
			
			/*
			int previousBind;
			IplImage *imgTest = cvCreateImage(cvSize(wWidth, wHeight), IPL_DEPTH_8U,1);
			glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, itImage->second.bufferObject);
			
			imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY);
			
			char imageName[100];
			sprintf(imageName, "iteration %d currentNFPs %d %d.jpg", iteration, (int)itImage->first.x(), (int)itImage->first.y());
			cvSaveImage(imageName, imgTest);
			
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
			cvReleaseImage(&imgTest);
			*/
		}

		//if no more pieces nextPiece, erase from map those pictures! 
		if(piecesToPlace[nextPolygonIndex]==0)
		{
#ifdef DEBUG
			cout<<"Deleting piece "<<nextPolygonIndex<<endl;
#endif
			piecesAvaliability[nextPolygonIndex] = -1;
			vector<Point_2> keysToDelete;
			for(map<Point_2, DrawingWithRotations>::iterator iterase= currentDrawingNFPs.begin();
				iterase != currentDrawingNFPs.end();
				iterase++)
			{
				if((int)iterase->first.x() == nextPolygonIndex){
					keysToDelete.push_back(iterase->first);
				}
			}

			for(int i=0; i < keysToDelete.size(); i++)
			{
				//cvReleaseImage(&currentLayoutNFPs[keysToDelete[i]]);
				//currentLayoutNFPs.erase(keysToDelete[i]);	//map<int, IplImage*> 
				layoutNFPsTest.erase(keysToDelete[i]);		//map<int, IplImage*>
				currentDrawingNFPs.erase(keysToDelete[i]);	//map<int, Drawing> 
				drawingNFPsTest.erase(keysToDelete[i]);		//map<int, map<int, Drawing>>
				drawingPolysTest.erase(keysToDelete[i]);	//map<int, Drawing>
			}

			for(map<Point_2, map<Point_2, DrawingWithRotations>>::iterator it = drawingNFPsTest.begin(); it!= drawingNFPsTest.end(); it++)
			{
				for(int i=0; i < keysToDelete.size(); i++)
				{
					it->second.erase(keysToDelete[i]);
				}
			}
		}

		//add new placement to drawings
		currentDrawingPolys.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, blue);

		//currentDrawingNFPs
		for(map<Point_2, DrawingWithRotations>::iterator itDrawing= currentDrawingNFPs.begin(); 
				itDrawing != currentDrawingNFPs.end();
				itDrawing++)
		{
			itDrawing->second.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, blue);
		}

		//drawingPolysTest
		for(map<Point_2, DrawingWithRotations>::iterator itDrawing= drawingPolysTest.begin(); 
				itDrawing != drawingPolysTest.end();
				itDrawing++)
		{
			if(itDrawing->first != nextPiece)
			{
				itDrawing->second.popLastPlacement();
				itDrawing->second.addListPlacement(nextPiece.x(), nextPiece.y(), nextPlacement, blue);
			}
		}

		//drawingNFPsTest
		for(map<Point_2, map<Point_2, DrawingWithRotations>>::iterator itDrawingNFPsTest= drawingNFPsTest.begin(); 
			itDrawingNFPsTest != drawingNFPsTest.end();
			itDrawingNFPsTest++)
		{
			for(map<Point_2, DrawingWithRotations>::iterator itDrawing= itDrawingNFPsTest->second.begin(); 
				itDrawing != itDrawingNFPsTest->second.end();
				itDrawing++)
			{
				//if(itDrawing->first != nextPiece)
				//{
				itDrawing->second.popLastPlacement();
				itDrawing->second.addListPlacement(nextPiece.x(),nextPiece.y(), nextPlacement, blue);
				//}
			}
		}
		return nextPiece;
	}
	//cout<<"Sizes after after "<<currentLayoutNFPs.size()<<" "<<currentDrawingNFPs.size()<<" "<<layoutNFPsTest.size()<<" "<<drawingNFPsTest.size()<<" "<<drawingPolysTest.size()<<endl;
}

Point_2 dynamicPieceSelection()
{
	//cout<<"Iteration: "<<iteration<<endl;
	Point_2 pieceToplace(-1,-1);
	int previousWastePixels=numeric_limits<int>::max();
	
	IplImage *imgTest = cvCreateImage(cvSize(tw, th), IPL_DEPTH_8U,1);
	void* initPtr = imgTest->imageData;
	double intMinThreshold, intMaxThreshold;
	for(map<Point_2, GLuint>::iterator itImg = layoutNFPsTest.begin();
		itImg != layoutNFPsTest.end();
		++itImg)
	{
		
		int previousBind;
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, itImg->second);
		
		//cout<<"PBO: "<<(int)itImg->second<<endl;
		imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
		//cout<<"ptr: "<<(int)imgTest->imageData<<endl;
		cvMinMaxLoc(imgTest, &intMinThreshold, &intMaxThreshold);
		intMaxThreshold--;

		cv::Mat imgTestMat = cv::cvarrToMat(imgTest);

		cv::threshold(imgTestMat, imgTestMat, intMaxThreshold, 255, cv::THRESH_BINARY);
#ifdef DEBUG
		char imageName[100];
		sprintf(imageName, "NFPs PBO %d %d.jpg", (int)itImg->first.x(), (int)itImg->first.y());
		cvSaveImage(imageName, imgTest);
#endif
		int wastePixels = cvCountNonZero(imgTest);

		if(wastePixels<previousWastePixels){
#ifdef DEBUG
			cout<<itImg->first<<" previousWastePixels: "<<previousWastePixels<<endl;
#endif
			pieceToplace=itImg->first;
			previousWastePixels=wastePixels;
		}
#ifdef DEBUG
		if(intMaxThreshold<254) cout<<"MaxThreshold in dynamicPieceSelection WTF?\n";
#endif
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
		double intMaxThreshold, intMinThreshold;
		
	}
	imgTest->imageData = (char*)initPtr;
	cvReleaseImage(&imgTest);

	return pieceToplace;
}

GLdouble *getPiecePosition(IplImage *layoutImg)
{
	vector<vector<cv::Point>> feasiblePositions = getFeasiblePositions(layoutImg);
	vector<vector<GLdouble*>> feasiblePositionsConverted(feasiblePositions.size());

	for(int i=0; i < feasiblePositions.size(); i++)
	{
		feasiblePositionsConverted[i]=vector<GLdouble*>(feasiblePositions[i].size());
		for(int j=0; j < feasiblePositions[i].size(); j++){
			if(feasiblePositions[i][j].x==1) feasiblePositions[i][j].x=0;
			if(feasiblePositions[i][j].y==1) feasiblePositions[i][j].y=0;

			feasiblePositionsConverted[i][j] = createPosition(((double)feasiblePositions[i][j].x+layout.getMargin())*layout.getResolution(), ((double)feasiblePositions[i][j].y+layout.getMargin())*layout.getResolution(), 0.0);
		}
	}
	
	//aplicar bottom left
	if(feasiblePositionsConverted.size()>0)
		return bottomLeft(feasiblePositionsConverted);
	else
		return NULL;
}

void displayDynamic_drawCurrentLayout()
{
	glDisable(GL_BLEND);

	//definir dimensoes do viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	BoundingBox layoutBB = drawingNFPsTest.begin()->second.begin()->second.getViewPort();
	gluOrtho2D(layoutBB.getX(), layoutBB.getWidth(), layoutBB.getY(), layoutBB.getHeight());	// set to orthogonal projection
	glMatrixMode(GL_MODELVIEW);			// switch to modelview matrix
	
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );

	for(map<Point_2, DrawingWithRotations>::iterator itNFPsOfPiecesToPlaceIndex=currentDrawingNFPs.begin();
		itNFPsOfPiecesToPlaceIndex != currentDrawingNFPs.end();
		itNFPsOfPiecesToPlaceIndex++)
	{
		DrawingWithRotations *d=&itNFPsOfPiecesToPlaceIndex->second;
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.0,0.0,1.0);
		for(int i=0; i < d->getPositions().size(); i++)
		{
			glPushMatrix();
			//glColor4f	(d->getColors()[i][0], d->getColors()[i][1], d->getColors()[i][2], d->getColors()[i][3]);
			glTranslatef(d->getPositions()[i][0], d->getPositions()[i][1], d->getPositions()[i][2]);
			glCallList	(d->getLists()[d->getOrder()[i].first][d->getOrder()[i].second]);
			glPopMatrix	();
		}
		//d->bufferObject=drawingNFPsTest[Point_2(0,0)][itNFPsOfPiecesToPlaceIndex->first].bufferObject;
		//IplImage *imgTest = cvCreateImage(cvSize(wWidth, wHeight), IPL_DEPTH_8U,1);

		int previousBind=0;
		glReadBuffer(GL_BACK_LEFT);
		glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, d->bufferObject);
		cout<<glGetError()<<endl;
		glReadPixels(tx, ty, tw, th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
		cout<<glGetError()<<endl;
		glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);
		//glutSwapBuffers();
		for(map<Point_2, map<Point_2, DrawingWithRotations>>::iterator itTest=drawingNFPsTest.begin();
			itTest != drawingNFPsTest.end();
			itTest++)
		{
			glReadBuffer(GL_BACK_LEFT);
			glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, itTest->second[itNFPsOfPiecesToPlaceIndex->first].bufferObject);
			glReadPixels(tx, ty, tw, th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		//cout<<"d->bufferObject: "<<d->bufferObject<<endl;

		int p=(int)itNFPsOfPiecesToPlaceIndex->first.x();
		int r=(int)itNFPsOfPiecesToPlaceIndex->first.y();

		//d->bufferObject=drawingNFPsTest[Point_2(0,0)][itNFPsOfPiecesToPlaceIndex->first].bufferObject;
#ifdef DEBUG
		IplImage *imgTest = cvCreateImage(cvSize(tw, th), IPL_DEPTH_8U,1);
		char* initPtr = imgTest->imageData;
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, d->bufferObject);
		imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY);
		char imageName[100];
		sprintf(imageName, "INIT %d %d.jpg", p, r);
		cvSaveImage(imageName, imgTest);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
		imgTest->imageData=initPtr;
		cvReleaseImage(&imgTest);
#endif
	}
}

bool displayDynamic(ofstream *myfile)
{
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	//cout<<"tw th "<<tw<<" "<<th<<endl;

	int previousBind=0;
	GLfloat blue[4] = {0.0, 0.0, 1.0, 1.0};
	set<Point_2> pointsToDelete;
	if(iteration==0)	displayDynamic_drawCurrentLayout();

	//definir dimensoes do viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	BoundingBox layoutBB = drawingNFPsTest.begin()->second.begin()->second.getViewPort();
	gluOrtho2D(layoutBB.getX(), layoutBB.getWidth(), layoutBB.getY(), layoutBB.getHeight());
	glMatrixMode(GL_MODELVIEW);

	double findPositioningTime, addPlacementTime, drawLayoutsTime, savePBOTime;
	
	findPositioningTime=0;
	addPlacementTime=0;
	drawLayoutsTime=0;
	savePBOTime=0;

	//experimentar colocar todas as peças
	clock_t begin=clock();


	for(map<Point_2, map<Point_2, DrawingWithRotations>>::iterator itPolygonBeingTestedIndex=drawingNFPsTest.begin();
		itPolygonBeingTestedIndex != drawingNFPsTest.end();
		itPolygonBeingTestedIndex++)
	{

#ifdef DEBUG
		cout<<"Sizes iteration "<<iteration<<": "<<drawingNFPsTest.size()<<" "<<layoutNFPsTest.size()<<endl;
#endif

		Point_2 polygonRotationBeingTested=itPolygonBeingTestedIndex->first;
		int polygonBeingTestedPiecesIndex=polygonRotationBeingTested.x();
		int rotationBeingTestedPiecesIndex=polygonRotationBeingTested.y();

		//determinar a posiçao da peça a experimentar
		clock_t lala = clock();
		GLdouble *position;
		IplImage *imgTest = cvCreateImage(cvSize(tw, th), IPL_DEPTH_8U,1);
		char* initPtr = imgTest->imageData;
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, currentDrawingNFPs[polygonRotationBeingTested].bufferObject);
		imgTest->imageData = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_ONLY);
		
		
#ifdef DEBUG
		char imageName[100];
		sprintf(imageName, "iteration %d currentNFPs beforePositionCalc %d %d.jpg", iteration, polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex);
		cvSaveImage(imageName, imgTest);
#endif

		position = getPiecePosition(imgTest);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
		imgTest->imageData=initPtr;
		cvReleaseImage(&imgTest);

		clock_t lalala = clock();
		findPositioningTime+=diffclock(lalala, lala);
		
		//se nao for possivel posicionar a peça, marca-la para nao a testar nas iteraçoes seguintes
		if(position==0)
		{
			//cout<<"LE FU\n";
			//nao e possivel colocar 
			if(--piecesAvaliability[polygonBeingTestedPiecesIndex]==0)
			{
				//cout<<"ERROR: not all pieces could be placed! Terminating."<<endl;
				layoutNFPsTest.clear();
				currentDrawingNFPs.clear();
				drawingNFPsTest.clear();
				drawingPolysTest.clear();
				return false;
			}else
			{
				pointsToDelete.insert(itPolygonBeingTestedIndex->first);
			}
		}else 
		{
			//se for possivel posicionar a peça, desenhar as imagens dos nfps para todas as peças possiveis de colocar a seguir
			//posicionar a peça no layout de teste das peças
#ifdef DEBUG
			cout<<"Position "<<position[0]<<" "<<position[1]<<endl;
#endif
			DrawingWithRotations *d = &drawingPolysTest[polygonRotationBeingTested];

			//adiciona posicao ao layout
			d->addListPlacement(polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex, position, blue);

			//desenhar os nfps dos outros poligonos para o layout com a nova peça adicionada
			for(map<Point_2, DrawingWithRotations>::iterator itDrawing = itPolygonBeingTestedIndex->second.begin(); 
				itDrawing != itPolygonBeingTestedIndex->second.end();
				itDrawing++)
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

				//se este poligono nao tiver sido marcado para apagar
				if(pointsToDelete.find(itDrawing->first) == pointsToDelete.end())
				{
					lala = clock();
					//adicionar nfp do poligono
					DrawingWithRotations *dnfps = &itDrawing->second;
					dnfps->addListPlacement(polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex, position, blue);
					
					//desenhar nfps da iteracao anterior
					glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
					glBindBuffer(GL_PIXEL_UNPACK_BUFFER, currentDrawingNFPs[itDrawing->first].bufferObject/*dnfps->bufferObject*/);
					glRasterPos3d(0.0,0.0,0.0);
					glDrawPixels(tw, th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
					glBindBuffer(GL_PIXEL_UNPACK_BUFFER, previousBind);
					glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &previousBind);
					
					//desenhar nfp da iteracao actual
					glColor3f(0.0, 0.0, 1.0);
					glPushMatrix();
					glTranslatef(dnfps->getPositions().back()[0], dnfps->getPositions().back()[1], dnfps->getPositions().back()[2]);
					glCallList	(dnfps->getLists()[dnfps->getOrder().back().first][dnfps->getOrder().back().second]);
					glPopMatrix();

					#ifdef DEBUG
						imgTest = getOpenCVImage(tx, ty, tw, th, GL_BLUE);
						char imageName[100];
						sprintf(imageName, "iteration %d currentNFPs beforePositionCalc %d %d - %d%d.jpg", iteration, polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex, (int)itDrawing->first.x(), (int)itDrawing->first.y());
						cvSaveImage(imageName, imgTest);
						cvReleaseImage(&imgTest);
					#endif

					//renderizar nfps do poligono
					lalala = clock();
					addPlacementTime += diffclock(lalala, lala);
					
					//adicionar todas as imagem dos nfps dos poligonos no buffer de acumulacao
					GLfloat alpha = 1.0/itPolygonBeingTestedIndex->second.size();
					glAccum(GL_ACCUM, alpha);
					lalala = clock();
					drawLayoutsTime += diffclock(lalala, lala);

					//guardar a imagem dos nfps do poligonos
					lala = clock();
					glReadBuffer(GL_BACK_LEFT);
					glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
					glBindBuffer(GL_PIXEL_PACK_BUFFER, dnfps->bufferObject);
					glReadPixels(tx, ty, tw, th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
					glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);
					lalala = clock();
					savePBOTime += diffclock(lalala, lala);
				}
			}

			//ler a imagem dos nfps de todos os poligonos acumulados
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glAccum(GL_RETURN, 1.0);
			glClear(GL_ACCUM_BUFFER_BIT);

	#ifdef DEBUG
			IplImage *imgTest = getOpenCVImage(wWidth, wHeight, GL_BLUE);
			char imageName[100];
			sprintf(imageName, "iteration %d NFPs pieces %d %d overlaping.jpg", iteration, polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex);
			cvSaveImage(imageName, imgTest);
			cvReleaseImage(&imgTest);
	#endif
			
			//desenhar a preto as peças do layout
			//glClear(GL_DEPTH_BUFFER_BIT);
			glColor3f(0.0,0.0,0.0);
			for(int i=0; i < d->getPositions().size(); i++)
			{
				glPushMatrix();
				glTranslatef(d->getPositions()[i][0], d->getPositions()[i][1], d->getPositions()[i][2]);
				glCallList	(d->getLists()[d->getOrder()[i].first][d->getOrder()[i].second]);
				glPopMatrix();
			}
			
	#ifdef DEBUG
			imgTest = getOpenCVImage(tx, ty, tw, th, GL_BLUE);
			char imageName[100];
			sprintf(imageName, "iteration %d NFPs pieces %d %d overlaping difference.jpg", iteration, polygonBeingTestedPiecesIndex, rotationBeingTestedPiecesIndex);
			cvSaveImage(imageName, imgTest);
			cvReleaseImage(&imgTest);
	#endif

			//ler imagem da soma dos nfps com a diferença do espaço ocupado pelas peças 
			glReadBuffer(GL_BACK_LEFT);
			glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &previousBind);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, layoutNFPsTest[polygonRotationBeingTested]);
			glReadPixels(tx, ty, tw, th, GL_BLUE, GL_UNSIGNED_BYTE, 0);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, previousBind);
		}
	}
	clock_t end = clock();
	double placementTime = diffclock(end, begin);

	begin = clock();
	//apagar as peças que ja nao podem ser colocadas
	for(set<Point_2>::iterator it = pointsToDelete.begin(); it != pointsToDelete.end(); it++)
	{
		layoutNFPsTest.erase(*it);		//map<int, IplImage*>
		currentDrawingNFPs.erase(*it);	//map<int, Drawing> 
		drawingNFPsTest.erase(*it);		//map<int, map<int, Drawing>>
		drawingPolysTest.erase(*it);	//map<int, Drawing>
	}
	
	//apagar peças que ja nao podem ser colocadas das hipoteses dos layouts a ser testados
	for(map<Point_2, map<Point_2, DrawingWithRotations>>::iterator it = drawingNFPsTest.begin(); it!= drawingNFPsTest.end(); it++)
	{
		for(set<Point_2>::iterator itd = pointsToDelete.begin(); itd != pointsToDelete.end(); itd++)
		{
			it->second.erase(*itd);
		}
	}
	Point_2 placedPiece = putPieceDynamic(false);

	end = clock();

	double updatingTime = diffclock(end, begin);
	*myfile<<iteration<<";"<<placementTime<<";"<<updatingTime<<";"<<currentDrawingPolys.getPositions().size()<<";"<<drawingNFPsTest.size()<<";"<<placedPiece<<";"<<findPositioningTime<<";"<<addPlacementTime<<"; "<<drawLayoutsTime<<"; "<<savePBOTime<<";"<<endl;
	iteration++;
	return true;
}
