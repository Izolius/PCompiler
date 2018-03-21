#include "PCompiler.h"
#include <fstream>

void main()
{
	ifstream ifs;
	ifs.open("Code.txt");
	if (ifs) {
		CCompiler compiler;
		compiler.Compile(string(istreambuf_iterator<char>(ifs), {}));
		//cout << string(istreambuf_iterator<char>(ifs), {});
	}
}