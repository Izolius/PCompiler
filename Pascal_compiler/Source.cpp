#include "PCompiler.h"
#include <fstream>

void main(int argc, char *argv[])
{
	string name = "test";
	string source = name + ".pas";
	string dest = name + ".asm";
	string exec = name + ".exe";
	if (argc == 3) {
		source = argv[1];
		dest = argv[2];
	}
	CCompiler compiler;
	if (compiler.Compile(source, dest))
		system(exec.c_str());
}