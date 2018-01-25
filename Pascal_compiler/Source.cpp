#include "PCompiler.h"
#include <fstream>

void main()
{
	ifstream ifs;
	ifs.open("Code.txt");
	if (ifs) {
		PCompiler compiler(ifs);
		compiler.Compile();
	}
	
}