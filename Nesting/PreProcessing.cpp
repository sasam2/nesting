#include "PreProcessing.h"

void solveCB(int dummy)
{
	//getData
	updateSolvingConfiguration();

	//loadProblem
	string problemNameStr(problemName);
	transform(problemNameStr.begin(), problemNameStr.end(), problemNameStr.begin(), ::tolower);

	int delimiter = problemNameStr.find('.')+1;
	string extension = problemNameStr.substr(delimiter, problemNameStr.size());

	if(extension.compare("dat")==0)
		layout = loadConfigurationFileIrregularProblem(problemName);
	else
		layout = loadLayout(problemName);

	//pre process geometry
	preProcessing();

}

void heuristicCB(int h_choosen)
{
	if(!radiogroup_heuristic->get_int_val())
		radiogroup_staticheuristic->enable();
	else
		radiogroup_staticheuristic->disable();
}

void updateSolvingConfiguration()
{
	//problemName=(char*)edittext_file->get_text();
	strcpy ( problemName, (char*)edittext_file->get_text() );
	saveAll=radiogroup_heuristic->get_int_val();
	staticHeuristic=radiogroup_staticheuristic->get_int_val()+1;
	//define nfps function
	if(radiogroup_nfps->get_int_val()==0)
		nfps = sortEdgesWithDecomposition;
	else
		nfps = minkowskiSumsWithDecompositionNoConversion;
}

void preProcessing()
{
	//NORMALIZAR peças e detectar largura e altura maximas
	double maxPieceWidth=layout.getMaxWidth();
	double maxPieceHeight=layout.getMaxHeight();
	for(int i=0; i < layout.getPieces().size();i++)
	{
		/*BoundingBox pieceBB = layout.getPieces()[i].getBoundingBox();
		maxPieceWidth = max(maxPieceWidth,pieceBB.getWidth());		//get max width
		maxPieceHeight = max(maxPieceHeight,pieceBB.getHeight());	//get max height*/
		layout.getPieces()[i].normalizeToOrigin();					//normalizar peças
	}
	//normalizar superficies
	layout.getStockSheet()[0].normalizeToOrigin();

	GLdouble *nextOrigin = new GLdouble[3];
	nextOrigin[0]=0.0;
	nextOrigin[1]=0.0;
	nextOrigin[2]=0.0;
	for(int i=1; i < layout.getStockSheet().size();i++)
	{
		//cout<<"lastPoint[0] "<<nextOrigin[0]<<endl;

		nextOrigin[0]=nextOrigin[0]-(layout.getStockSheet()[i-1].getBoundingBox().getWidth()+maxPieceWidth);

		layout.getStockSheet()[i].normalizeToOrigin();
		layout.getStockSheet()[i].normalize(nextOrigin);

	}
	
	//RODAR peças e calcular estatisticas
	numberOfPolygons=layout.getPieces().size();
	int numberOfNFPs=numberOfPolygons+1;
	nrOfRotations = 360/layout.getRotationStep();
	double angleRadians = ((double)layout.getRotationStep())*M_PI/180;

	polygons=new Polygon_with_holes_2*[numberOfPolygons];
	map<Point_2, PolyInfo> infos;
	//vector<int> piecesAvaliability;
	
	for(int i=0; i <numberOfPolygons; i++)
	{
		//guardar nr de rotacoes disponivel para ser colocado
		piecesAvaliability.push_back(nrOfRotations);
		polygons[i]=new Polygon_with_holes_2[nrOfRotations];
		polygons[i][0]=piece2PolygonWithHoles(layout.getPieces()[i]);

		//area da peça, area da convex hull, nr de vertices concavos
		int nrOfConcaveVertices = getNumberOfConcaveVertices(polygons[i][0].outer_boundary());
		double a = polygons[i][0].outer_boundary().area();

		Polygon_2 convexHull;
		CGAL::convex_hull_2 (polygons[i][0].outer_boundary().vertices_begin(), polygons[i][0].outer_boundary().vertices_end(), back_inserter(convexHull));
		double concavity = (convexHull.area()-a)/convexHull.area();

		Polygon_2 boundingBox;
		boundingBox.push_back( Point_2(polygons[i][0].bbox().xmin(), polygons[i][0].bbox().ymin()) );
		boundingBox.push_back( Point_2(polygons[i][0].bbox().xmax(), polygons[i][0].bbox().ymin()) );
		boundingBox.push_back( Point_2(polygons[i][0].bbox().xmax(), polygons[i][0].bbox().ymax()) );
		boundingBox.push_back( Point_2(polygons[i][0].bbox().xmin(), polygons[i][0].bbox().ymax()) );
		double rectangularity = boundingBox.area()-polygons[i][0].outer_boundary().area();

		//largura e altura da BB
		CGAL::Bbox_2 pBB = polygons[i][0].bbox();
		double w = pBB.xmax()-pBB.xmin();
		double l = pBB.ymax()-pBB.ymin();
		infos[Point_2(i,0)]=PolyInfo(w,l,a,rectangularity,concavity,nrOfConcaveVertices);

		//rodar as peças e guardar rotaçoes
		for(int r=1; r <nrOfRotations; r++)
		{
			polygons[i][r] = generatePieceDiscreteRotation(polygons[i][0], angleRadians*r);
			//cout<<"clkwse "<<i<<" "<<r<<" "<<polygons[i][r].outer_boundary().is_counterclockwise_oriented()<<endl;
			pBB = polygons[i][r].bbox();
			w = pBB.xmax()-pBB.xmin();
			l = pBB.ymax()-pBB.ymin();
			infos[Point_2(i,r)]=PolyInfo(w,l,a,rectangularity,concavity,nrOfConcaveVertices);
		}
	}
	//Polygon_with_holes_2 stockSheetPoly = piece2PolygonWithHoles(layout.getStockSheet()[0]);
	Polygon_with_holes_2 *stockSheetsPoly = new Polygon_with_holes_2[layout.getStockSheet().size()]; 

	for(int ss=0; ss < layout.getStockSheet().size(); ss++){
		stockSheetsPoly[ss]=piece2PolygonWithHoles(layout.getStockSheet()[ss]);
	}

	//DECOMPOSICAO

	//decompor peças
	vector<Polygon_2>** polygonsDecompositions = decomposePiecesAngBis (polygons, numberOfPolygons, nrOfRotations);
	
	//decompor stock sheets
	//inverter
	vector<Polygon_2>* stockSheetsPolyForInnerFP = new vector<Polygon_2>[layout.getStockSheet().size()];
	for(int ss=0; ss < layout.getStockSheet().size(); ss++)
	{
		if(!(stockSheetsPoly[ss].outer_boundary()).is_simple())
		{
			cout<<"Error stock sheet "<<ss<<":\n";
			print_polygon(stockSheetsPoly[ss].outer_boundary());
		}
		stockSheetsPolyForInnerFP[ss]= invertPolygonWithHoles(stockSheetsPoly[ss], maxPieceWidth, maxPieceHeight);
	}
	
	//decompor
	vector<Polygon_2> sSPolyVec;
	CGAL::Small_side_angle_bisector_decomposition_2<Kernel> decompAB;
	
	for(int ss=0; ss<layout.getStockSheet().size(); ss++)
	{
		for(int i=0; i<stockSheetsPolyForInnerFP[ss].size(); i++)
		{
			if(!(stockSheetsPolyForInnerFP[ss][i]).is_simple())
			{
				cout<<"Error inverted stock sheet "<<ss<<":\n";
				print_polygon(stockSheetsPolyForInnerFP[ss][i]);
			}
			decompAB (stockSheetsPolyForInnerFP[ss][i], back_inserter(sSPolyVec));
		}
	}
	//CALCULO DOS NFPs e IFPs 

	//generate no fit polygons e inner fit polygons
	vector<Polygon_2>**** nfpsPiecesAndStockSheet=new vector<Polygon_2>***[numberOfPolygons];
	for(int i=0; i< numberOfPolygons; i++)//para cada poligono movel
	{
		nfpsPiecesAndStockSheet[i]=new vector<Polygon_2>**[nrOfRotations];
		for(int ri=0; ri<nrOfRotations; ri++)//para cada rotacao do poligono movel
		{
			nfpsPiecesAndStockSheet[i][ri]=new vector<Polygon_2>*[numberOfNFPs];
			for(int j=0; j<numberOfNFPs-1; j++)//para cada poligono fixo e sotck sheet
			{
				nfpsPiecesAndStockSheet[i][ri][j]=new vector<Polygon_2>[nrOfRotations];
				for(int rj=0; rj<nrOfRotations; rj++)//para cada rotacao do poligono fixo
				{
					double time;
					//nfpsPiecesAndStockSheet[i][ri][j][rj]=minkowskiSumsWithDecompositionNoConversion(polygonsDecompositions[j][rj], polygonsDecompositions[i][ri]/*, &time*/);
					//nfpsPiecesAndStockSheet[i][ri][j][rj]=sortEdgesWithDecomposition(polygonsDecompositions[j][rj], polygonsDecompositions[i][ri]);
					nfpsPiecesAndStockSheet[i][ri][j][rj]=nfps(polygonsDecompositions[j][rj], polygonsDecompositions[i][ri]);
				}
			}
			//ifp para a stock sheet
			nfpsPiecesAndStockSheet[i][ri][numberOfNFPs-1]=new vector<Polygon_2>[1];
			nfpsPiecesAndStockSheet[i][ri][numberOfNFPs-1][0]=minkowskiSumsWithDecompositionNoConversion(sSPolyVec, polygonsDecompositions[i][ri]);
		}
	}

	//CALCULATE DISPLAY LISTS
	//stock sheet
	stockList = generatePolygonDisplayList(sSPolyVec);

	BoundingBox viewport(layout.stockSheetsBoundingBox().getX(),
						layout.stockSheetsBoundingBox().getY(),
						layout.stockSheetsBoundingBox().getWidth()+(layout.getStockSheet().size()-1)*layout.getMaxWidth(),
						layout.stockSheetsBoundingBox().getHeight());
	//polys
	currentDrawingPolys=DrawingWithRotations(numberOfPolygons, nrOfRotations);
	currentDrawingPolys.addDisplayListsFromPolygonVector(polygons, numberOfPolygons, nrOfRotations, generatePolygonWHolesDisplayList);
	currentDrawingPolys.setViewPort(viewport);
	//currentDrawingPolys.setViewPort(layout.getStockSheet()[0].getBoundingBox());
	currentDrawingPolys.resetPlacements();
	
	//NFPs e IFPs
	int hV = viewport.getHeight();
	int h = hV/layout.getResolution();
	int wV = viewport.getWidth();
	int w = wV/layout.getResolution();
	GLdouble *origin = createPosition(0.0,0.0,0.0);
	for(int p=0; p <numberOfPolygons; p++)
	{
		for(int r=0; r <nrOfRotations; r++)
		{
			Point_2 pointPR(p,r);
			currentDrawingNFPs[pointPR]=DrawingWithRotations(numberOfNFPs, nrOfRotations);
			currentDrawingNFPs[pointPR].addNFPsDisplayListsFromPolygonVector(nfpsPiecesAndStockSheet[p][r], numberOfNFPs, nrOfRotations, generatePolygonDisplayList);
			currentDrawingNFPs[pointPR].setViewPort(viewport);
			//currentDrawingNFPs[pointPR].setViewPort(layout.getStockSheet()[0].getBoundingBox());
			currentDrawingNFPs[pointPR].resetPlacements();
			currentDrawingNFPs[pointPR].addListPlacement(numberOfNFPs-1, 0, origin, blue);

			if(saveAll)
			{
				glGenBuffers(1, &currentDrawingNFPs[pointPR].bufferObject);
				glBindBuffer(GL_PIXEL_PACK_BUFFER,	currentDrawingNFPs[pointPR].bufferObject);
				glBufferData(GL_PIXEL_PACK_BUFFER,	//(layout.getStockSheet()[0].getBoundingBox().getWidth()/layout.getResolution())*
													//(layout.getStockSheet()[0].getBoundingBox().getHeight()/layout.getResolution()),
													(h*(w+3)),
													0,
													GL_STREAM_READ);
				glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			}else
			{
				glGenBuffers(1, &currentDrawingNFPs[pointPR].bufferObject);
				glBindBuffer(GL_PIXEL_PACK_BUFFER,	currentDrawingNFPs[pointPR].bufferObject);
				glBufferData(GL_PIXEL_PACK_BUFFER,	//(layout.getStockSheet()[0].getBoundingBox().getWidth()/layout.getResolution())*
													//(layout.getStockSheet()[0].getBoundingBox().getHeight()/layout.getResolution()),
													(h*(w+3)),
													0,
													GL_STREAM_READ);
				glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			
			}
			
		}
	}

	//especifico
	if(!saveAll)
		preProcessingStatic(nrOfRotations, infos);
	else
		preProcessingDynamic(nrOfRotations, numberOfNFPs, infos, polygonsDecompositions, nfpsPiecesAndStockSheet);

	BoundingBox currentLayoutBB = viewport; //layout.getStockSheet()[0].getBoundingBox();//
	int resCorrectionWidth = currentLayoutBB.getWidth()/layout.getResolution();

	glutReshapeWindow(w+tx, h+ty);
	cout<<"Viewport: "<<currentLayoutBB.getWidth()<<"    "<<currentLayoutBB.getHeight()<<endl;
	cout<<"Window Dimensions: "<<w<<" "<<h<<endl;
	cout<<"Resolution: "<<layout.getResolution()<<endl;
	
	//inicializar poedeira de peças
	if(saveAll)
		putPieceDynamic(true);
}

void preProcessingStatic(int nrOfRotations, map<Point_2, PolyInfo> infos)
{
	int greaterDimension;
	if(layout.getStockSheet()[0].getBoundingBox().getWidth() > layout.getStockSheet()[0].getBoundingBox().getHeight()){
		greaterDimension=1;
	}else{
		greaterDimension=2;
	}

	map<int, int> piecesToPlace;
	for(int p=0; p < numberOfPolygons; p++)
		piecesToPlace[p]=layout.getQuantity()[p];

	switch(staticHeuristic)
	{
		case WIDER:
			//cout<<infos.size();
			for(int p=0; p < numberOfPolygons; p++)
			{
				Point_2 best(p, 0);
				for(int r=1; r < nrOfRotations; r++)
				{
					Point_2 currentP(p,r);
					if(infos[best].width > infos[currentP].width)
						best = currentP;
				}
				list<Point_2>::iterator j=piecesOrdered.begin();
				for(;	j != piecesOrdered.end() && infos[*j].width > infos[best].width; j++);
				for(int c=0; c < layout.getQuantity()[p]; c++)
					j=piecesOrdered.insert(j, best);

			}
		break;

		case HIGHER:
			//cout<<infos.size();
			for(int p=0; p < numberOfPolygons; p++)
			{
				Point_2 best(p, 0);
				for(int r=1; r < nrOfRotations; r++)
				{
					Point_2 currentP(p,r);
					if(infos[best].height > infos[currentP].height)
						best = currentP;
				}
				list<Point_2>::iterator j=piecesOrdered.begin();
				for(;	j != piecesOrdered.end() && infos[*j].height > infos[best].height; j++);
				for(int c=0; c < layout.getQuantity()[p]; c++)
					j=piecesOrdered.insert(j, best);
			}
		break;

		case LARGER:
			for(int p=0; p < numberOfPolygons; p++)
			{
				Point_2 best(p, 0);	//ver qual das rotaçoes e a melhor (a que tiver a dimensao mais estreita na direccao da dimensao mais larga da stock sheet)
						
				for(int r=1; r < nrOfRotations; r++)
				{
					Point_2 currentP(p,r);
					//cout<<best<<" "<<infos[best].width<<" "<<currentP<<" "<<infos[currentP].width<<endl;
					
					if(greaterDimension==1)
					{
						if(infos[best].width > infos[currentP].width )
							best = currentP;
					}else
					{
						if(infos[best].height > infos[currentP].height )
						best = currentP;
					}
				}
				list<Point_2>::iterator j=piecesOrdered.begin();	//introduzir n vezes essa peça com essa rotacao na lista, ordenada por areas
				for(;	j != piecesOrdered.end() && infos[*j].area > infos[best].area; j++);
				for(int c=0; c < layout.getQuantity()[p]; c++)
					j=piecesOrdered.insert(j, best);
			}
		break;

		case MORE_IRREGULAR:
			for(int p=0; p < numberOfPolygons; p++)
			{
				Point_2 best(p, 0);	//ver qual das rotaçoes e a melhor (a que tiver a dimensao mais larga na direccao da dimensao mais larga da stock sheet)
				for(int r=1; r < nrOfRotations; r++)
				{
					Point_2 currentP(p,r);
					if(greaterDimension==1)
					{
						if(infos[best].width > infos[currentP].width )
							best = currentP;
					}else
					{
						if(infos[best].height > infos[currentP].height )
						best = currentP;
					}
				}
				list<Point_2>::iterator j=piecesOrdered.begin();	//introduzir n vezes essa peça com essa rotacao na lista, ordenada por areas
				for(;	j != piecesOrdered.end() && infos[*j].concavity > infos[best].concavity; j++);
				for(int c=0; c < layout.getQuantity()[p]; c++)
					j=piecesOrdered.insert(j, best);
			}
		break;

		case MORE_RECTANGULAR:
			for(int p=0; p < numberOfPolygons; p++)
			{
				Point_2 best(p, 0);	//ver qual das rotaçoes e a melhor (a que tiver a dimensao mais larga na direccao da dimensao mais larga da stock sheet)
				for(int r=1; r < nrOfRotations; r++)
				{
					Point_2 currentP(p,r);
					if(greaterDimension==1)
					{
						if(infos[best].width > infos[currentP].width )
							best = currentP;
					}else
					{
						if(infos[best].height > infos[currentP].height )
							best = currentP;
					}
				}
				list<Point_2>::iterator j=piecesOrdered.begin();	//introduzir n vezes essa peça com essa rotacao na lista, ordenada por areas
				for(;	j != piecesOrdered.end() && infos[*j].rectangularity > infos[best].rectangularity; j++);
				for(int c=0; c < layout.getQuantity()[p]; c++)
					j=piecesOrdered.insert(j, best);
			}
		break;

		case RANDOM:
			while(!piecesToPlace.empty())
			{
				//escolher peça
				int pieceIndex = rand()%piecesToPlace.size();
				int piece;
				map<int, int>::iterator it = piecesToPlace.begin();
				for(int i=0;
					it != piecesToPlace.end() && i < pieceIndex;
					it++, i++);
				piece=it->first;

				//escolher rotacao
				int rotation = rand()%nrOfRotations;
				piecesOrdered.push_back(Point_2(piece, rotation));

				//decrementar quantidade
				if(--it->second == 0)
					piecesToPlace.erase(it);	//se ja nao houver peças desse tipo apagar peça do mapa
			}
		break;

		default:
		break;
	}
}

void preProcessingDynamic(int nrOfRotations, int numberOfNFPs, map<Point_2, PolyInfo> infos, vector<Polygon_2>** polygonsDecompositions, vector<Polygon_2>**** nfpsPiecesAndStockSheet)
{
	GLdouble *origin = createPosition(0.0,0.0,0.0);
	GLfloat *colorStatic = createColor(0.0,0.0,1.0,1.0);

	BoundingBox viewport(layout.stockSheetsBoundingBox().getX(),
						layout.stockSheetsBoundingBox().getY(),
						layout.stockSheetsBoundingBox().getWidth()+(layout.getStockSheet().size()-1)*layout.getMaxWidth(),
						layout.stockSheetsBoundingBox().getHeight());
	
	int hV = viewport.getHeight();
	int h = hV/layout.getResolution();
	int wV = viewport.getWidth();
	int w = wV/layout.getResolution();

	for(int p=0; p <numberOfPolygons; p++)
	{
		for(int r=0; r <nrOfRotations; r++)
		{
			Point_2 pointPR(p,r);

			drawingPolysTest[pointPR]=DrawingWithRotations(numberOfPolygons, nrOfRotations);
			drawingPolysTest[pointPR].addDisplayListsFromPolygonVector(polygonsDecompositions, numberOfPolygons, nrOfRotations, generatePolygonDisplayList);
			drawingPolysTest[pointPR].setViewPort(viewport);
			//drawingPolysTest[pointPR].setViewPort(layout.getStockSheet()[0].getBoundingBox());
			drawingPolysTest[pointPR].resetPlacements();
			

			glGenBuffers(1, &layoutNFPsTest[pointPR]);
			glBindBuffer(GL_PIXEL_PACK_BUFFER,	layoutNFPsTest[pointPR]);
			glBufferData(GL_PIXEL_PACK_BUFFER,	//(layout.getStockSheet()[0].getBoundingBox().getWidth()/layout.getResolution())*
												//(layout.getStockSheet()[0].getBoundingBox().getHeight()/layout.getResolution()), 
												(h*(w+3)),
												0,
												GL_STREAM_READ);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
			
			for(int p2=0; p2 <numberOfPolygons; p2++)
			{
				for(int r2=0; r2 <nrOfRotations; r2++)
				{
					Point_2 pointPR2(p2,r2);

					drawingNFPsTest[pointPR][pointPR2]=DrawingWithRotations(numberOfNFPs, nrOfRotations);
					drawingNFPsTest[pointPR][pointPR2].addNFPsDisplayListsFromPolygonVector(nfpsPiecesAndStockSheet[p2][r2], numberOfNFPs, nrOfRotations, generatePolygonDisplayList);
					drawingNFPsTest[pointPR][pointPR2].setViewPort(viewport);
					//drawingNFPsTest[pointPR][pointPR2].setViewPort(layout.getStockSheet()[0].getBoundingBox());
					drawingNFPsTest[pointPR][pointPR2].resetPlacements();
					drawingNFPsTest[pointPR][pointPR2].addListPlacement(numberOfNFPs-1, 0, origin, colorStatic);

					//glBufferDataARB with NULL pointer reserves only memory space.
					glGenBuffers(1, &drawingNFPsTest[pointPR][pointPR2].bufferObject);
					glBindBuffer(GL_PIXEL_PACK_BUFFER, drawingNFPsTest[pointPR][pointPR2].bufferObject);
					glBufferData(GL_PIXEL_PACK_BUFFER,	//(drawingNFPsTest[pointPR][pointPR2].getViewPort().getWidth()/layout.getResolution())*
														//(drawingNFPsTest[pointPR][pointPR2].getViewPort().getHeight()/layout.getResolution()), 
														(h*(w+3)),
														0, 
														GL_STREAM_READ);
					glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
				}
			}
		}
	}
	//delete[] origin;
	//delete[] colorStatic;
}
