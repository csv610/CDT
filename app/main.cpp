#ifdef _MSC_VER // Workaround for known bug on MSVC
#define _HAS_STD_BYTE 0  // https://developercommunity.visualstudio.com/t/error-c2872-byte-ambiguous-symbol/93889
#endif

#include <iostream>
#include <string>
#include <cstring>
#include "cdt_api.h"
#include "numerics.h"

int main(int argc, char* argv[])
{
	initFPU();

	if (argc < 2) {
		std::cout << "CDT - Create a constrained Delaunay tetrahedrization out of a triangulated OFF file.\n";
		std::cout << "USAGE: CDT [-lbvfqnrs] filename.off\n";
		std::cout << "Example 1: CDT -bv test.off\n";
		std::cout << "Example 2: CDT -b -v test.off\n";
		std::cout << "OPTIONS:\n";
		std::cout << "-l: log results to cdt_log.csv\n";
		std::cout << "-a: use Assimp to load model (supports OBJ, FBX, GLTF, STL, etc.)\n";
		std::cout << "-b: add eight vertices to enclose everything in a box\n";
		std::cout << "-v: verbose mode\n";
		std::cout << "-w: log on screen instead of file (implies -l)\n";
		std::cout << "-f: try to make the output representable using floating point\n";
		std::cout << "-q: rational output\n";
		std::cout << "-k: keep boundary (do not create new vertices/faces on boundary)\n";
		std::cout << "-n: binary output\n";
		std::cout << "-m: use MEDIT format instead of TET\n";
		std::cout << "-r: remove outer tetrahedra from output (if input is closed)\n";
		std::cout << "-s: saves skin to an ASCII OFF file (triangles between IN and OUT)\n";
		std::cout << "OUTPUT:\n";
		std::cout << "Output has same name (and path) as input with an extension appended.\n";
		std::cout << "E.g. CDT my_dir/test.off produces my_dir/test.off.tet\n";
		std::cout << "E.g. CDT -s my_dir/test.off produces my_dir/test.off.tet and my_dir/test.off.off\n";
		return 0;
	}

	char filename[2048] = "";

	std::string options = "";

	for (int i = 1; i < argc; i++)
		if (argv[i][0] == '-') {
			for (int j = 1; (size_t)j < strlen(argv[i]); j++) options += argv[i][j];
		}
		else strncpy(filename, argv[i], 2047);

    if (strlen(filename) == 0) {
        std::cerr << "Error: No input file specified.\n";
        return 1;
    }

	// Load a valid PLC from file
	inputPLC plc;
	if (options.find('a') != std::string::npos) {
		plc.initFromAssimp(filename, options.find('v') != std::string::npos);
	} else {
		plc.initFromFile(filename, options.find('v') != std::string::npos);
	}

	TetMesh* tin = createSteinerCDT(plc, options.c_str());

	if (saveOutputFile(*tin, filename, options.c_str()))
		printf("Finished\n");

    delete tin;

	return 0;
}
