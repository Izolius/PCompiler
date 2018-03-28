#include "PCompiler.h"
#include <fstream>

void main()
{
	CCompiler compiler;
	if (compiler.Compile("Code.txt", "code.asm"))
		system("code.exe");
}