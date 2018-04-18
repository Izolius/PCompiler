#include "CCompiler.h"
#include <fstream>

void main(int argc, char *argv[])
{
	string name = "test";
	string source = name + ".pas";
	string dest = name + ".asm";
	string exec = name + ".exe";
	if (argc >=2) {
		source = argv[1];
		size_t pos = source.find_last_of('.');
		if (pos != -1) {
			dest = source.substr(0, pos) + ".asm";
			exec = source.substr(0, pos) + ".exe";
		}
	}
	if (argc >= 3) {
		dest = argv[2];
		size_t pos = dest.find_last_of('.');
		if (pos != -1) {
			exec = dest.substr(0, pos) + ".exe";
		}
	}
	CCompiler compiler;
	compiler.Compile(source, dest);
	//if (compiler.Compile(source, dest))
	//	system(exec.c_str());
}