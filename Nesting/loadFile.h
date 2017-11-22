#pragma once
#include "Problem.h"

void getFirstNonEmptyLine(istream *s, string *line);
Problem loadFile(char* filename, bool mode);
Layout loadLayout(char* filename);
vector<string> loadConfigurationFile(char* filename);

Piece loadPieceFileIrregularProblem(char* filename);
Component readComponentIrregularProblem(ifstream *file);
Layout loadConfigurationFileIrregularProblem(char* filename);