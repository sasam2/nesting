#include "loadFile.h"


void getFirstNonEmptyLine(istream *s, string *line)
{
	do{
		getline (*s,*line);	
	}while(line->size()==0 && s->good());
}

Layout loadLayout(char* filename)
{
	vector<string> parameters = loadConfigurationFile(filename);
	int piecesFileFormat = atoi(parameters[1].c_str());
	Problem problem = loadFile((char*)parameters[0].c_str(), piecesFileFormat);
	int stockSheetFormat = atoi(parameters[3].c_str());
	Piece stockSheet = loadFile((char*)parameters[2].c_str(), stockSheetFormat).getPieces()[0];
	vector<Piece> ss;	ss.push_back(stockSheet); //REMENDO
	int rotation = atoi(parameters[4].c_str());
	if(rotation == 0) rotation= 360;
	double resolution = atof(parameters[5].c_str());
	int margin = atoi(parameters[6].c_str());
	if(360%rotation!=0)
		throw exception();
	Layout l(problem, /*stockSheet*/ss, rotation, resolution, margin);
	return l;
}

vector<string> loadConfigurationFile(char* filename)
{
	string piecesFile, piecesFormat, stockSheetFile, stockSheetFormat, rotationStep, resolution, margin;

	vector<string> parameters;
	ifstream file (filename);
	//get pieces file
	getFirstNonEmptyLine(&file, &piecesFile);
	getFirstNonEmptyLine(&file, &piecesFile);
	getFirstNonEmptyLine(&file, &piecesFormat);
	parameters.push_back(piecesFile);
	parameters.push_back(piecesFormat);
	//get stock sheet file
	getFirstNonEmptyLine(&file, &stockSheetFile);
	getFirstNonEmptyLine(&file, &stockSheetFile);
	getFirstNonEmptyLine(&file, &stockSheetFormat);
	parameters.push_back(stockSheetFile);
	parameters.push_back(stockSheetFormat);
	//get rotations
	getFirstNonEmptyLine(&file, &rotationStep);
	getFirstNonEmptyLine(&file, &rotationStep);
	parameters.push_back(rotationStep);
	//get resolution
	getFirstNonEmptyLine(&file, &resolution);
	getFirstNonEmptyLine(&file, &resolution);
	parameters.push_back(resolution);
	//get margin
	getFirstNonEmptyLine(&file, &margin);
	getFirstNonEmptyLine(&file, &margin);
	parameters.push_back(margin);

	file.close();
	return parameters;
}

Problem loadFile(char* filename, bool mode)
{
	string line;
	ifstream file (filename);
	Problem problem;

	if (file.is_open())
	{
		int nr_polygons=0;
		int quantity=0;
		int nr_components=1;
		int nr_vertices=0;

		if(mode)
		{
			getFirstNonEmptyLine (&file,&line); //le cabecalho nr de poligonos
			getFirstNonEmptyLine (&file,&line); //le nr poligonos
			nr_polygons=atoi(line.c_str());
		}

		while ( file.good() ) //le pecas
		{
			getFirstNonEmptyLine (&file,&line);	//elimina cabecalho poligono
			getFirstNonEmptyLine (&file,&line);	//elimina cabecalho quantidade
			getFirstNonEmptyLine (&file,&line);	//le quantidade
			quantity=atoi(line.c_str());

			if(mode)
			{
				//elimina cabecalho nr componentes
				getFirstNonEmptyLine (&file,&line);
				//le nr componenetes
				getFirstNonEmptyLine (&file,&line);
				nr_components=atoi(line.c_str());
			}
			Piece current_piece;

			//current_piece.clear(); //elimina conteudo da peca anterior
			for (int i_components=0; i_components<nr_components; i_components++) //le componentes
			{
				if(mode)
					getFirstNonEmptyLine (&file,&line);	//le componenete
				
				getFirstNonEmptyLine (&file,&line);	//le cabecalho nr vertices
				getFirstNonEmptyLine (&file,&line);	//le nr vertices
				nr_vertices=atoi(line.c_str());
				getFirstNonEmptyLine (&file,&line);	//elimina cabecalho vertices

				//cria novo componente
				Component current_component;
				/*
				current_component = new double*[nr_vertices];
				for(int i = 0; i < nr_vertices; ++i)
					current_component[i] = new double[3];
				*/

				//adiciona vertices ao componente
				for (int i_vertices=0; i_vertices<nr_vertices; i_vertices++) //le vertices
				{
					GLdouble *current_vertex = new GLdouble[3];
					string coord;
					file>>coord;
					current_vertex[0]=atoi(coord.c_str());
					file>>coord;
					current_vertex[1]=atoi(coord.c_str());
					current_vertex[2]=0;
					current_component.push_back(current_vertex);
				}
				//adiciona componente a peca
				current_piece.addComponent(current_component);
			}
			problem.addPiece(current_piece, quantity); //adiciona peça ao problema
		}
		file.close();
		if(quantity==0)	problem.popPiece();
	}
	else cout << "Unable to open file "<<filename<<endl; 
	return problem;
}

Layout loadConfigurationFileIrregularProblem(char* filename)
{
	string scaleFactor, nrSheets, nrPatterns, rotationStep, resolution, margin;
	vector<Piece> stockSheets;
	Problem problem;
	int nrStockSheets;
	int nr_patterns;

	ifstream file (filename);

	//get pieces file
	getFirstNonEmptyLine(&file, &scaleFactor);
	getFirstNonEmptyLine(&file, &scaleFactor);

	//get stock sheets
	getFirstNonEmptyLine(&file, &nrSheets);
	int delimiter = nrSheets.find(':')+1;
	cout<<nrSheets.substr(delimiter, nrSheets.size());
	nrStockSheets=atoi(nrSheets.substr(delimiter, nrSheets.size()).c_str());
	for(int i=0; i < nrStockSheets; i++)
	{
		string stockSheet;
		getFirstNonEmptyLine(&file, &stockSheet);
		Piece current_sheet = loadPieceFileIrregularProblem((char*)stockSheet.c_str());
		stockSheets.push_back(current_sheet);
	}

	//get patterns
	getFirstNonEmptyLine(&file, &nrPatterns);
	delimiter = nrPatterns.find(':')+1;
	nr_patterns=atoi(nrPatterns.substr(delimiter, nrSheets.size()).c_str());
	for(int i=0; i < nr_patterns; i++)
	{
		string pattern;
		getFirstNonEmptyLine(&file, &pattern);
		int delimiter=pattern.find(' ')+1;
		string filename=pattern.substr(0, delimiter);
		int quantity=atoi(pattern.substr(delimiter, pattern.size()).c_str());
		Piece current_piece = loadPieceFileIrregularProblem((char*)pattern.substr(0, delimiter).c_str());
		problem.addPiece(current_piece, quantity);
	}

	//get rotations
	getFirstNonEmptyLine(&file, &rotationStep);
	getFirstNonEmptyLine(&file, &rotationStep);
	int rot = atoi(rotationStep.c_str());
	if(rot == 0) rot= 360;
	
	//get resolution
	getFirstNonEmptyLine(&file, &resolution);
	getFirstNonEmptyLine(&file, &resolution);
	double res = atof(resolution.c_str());
	
	//get margin
	getFirstNonEmptyLine(&file, &margin);
	getFirstNonEmptyLine(&file, &margin);
	int marg = atoi(margin.c_str());
	
	file.close();

	return Layout(problem, stockSheets, rot, res, marg);
}

Piece loadPieceFileIrregularProblem(char* filename)
{
	string line;
	ifstream file (filename);
	
	getFirstNonEmptyLine(&file, &line);
	getFirstNonEmptyLine(&file, &line); //ignorar barycentro
	getFirstNonEmptyLine(&file, &line); //ignorar orientacao
	
	//adiciona componente a peca
	Piece current_piece;
	current_piece.addComponent(readComponentIrregularProblem(&file));

	//get defects
	string nrDefects;
	file>>nrDefects; //number
	file>>nrDefects; //of
	file>>nrDefects; //defects
	file>>nrDefects; //<nr>
	int nr_defects = atoi(nrDefects.c_str());

	for (int i_defects=0; i_defects<nr_defects; i_defects++) //le defeitos
	{
		getFirstNonEmptyLine(&file, &line);//ignore defect code
		
		string code;
		file>>code;//defect
		file>>code;//code
		file>>code;//<code>

		string type;
		file>>type; //defect
		file>>type; //type
		file>>type; //<nr>

		if(code.compare(string("Null")))
		{
			getFirstNonEmptyLine(&file, &line);//ignore geometry
		}else
		{
			Component current_defect = readComponentIrregularProblem(&file);
			if(atoi(type.c_str())==1) //se for buraco adicionar
			{
				current_piece.addComponent(current_defect);
			}
		}
	}
	file.close();

	return current_piece;
}

Component readComponentIrregularProblem(ifstream *file)
{
	//get vertices
	string nrVertices;
	*file>>nrVertices; //number
	*file>>nrVertices; //of
	*file>>nrVertices; //vertices
	*file>>nrVertices; //<nr>
	int nr_vertices = atoi(nrVertices.c_str());
	
	Component current_component;

	//adiciona vertices ao componente
	for (int i_vertices=0; i_vertices<nr_vertices; i_vertices++) //le vertices
	{
		GLdouble *current_vertex = new GLdouble[3];
		string coord;
		*file>>coord;
		current_vertex[0]=atoi(coord.c_str());
		*file>>coord;
		current_vertex[1]=atoi(coord.c_str());
		current_vertex[2]=0;
		current_component.push_back(current_vertex);
	}

	return current_component;
}