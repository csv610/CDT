#ifdef _MSC_VER
#define _HAS_STD_BYTE 0
#endif

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "Cdt.h"

namespace {
void printUsage(const char* programName) {
	std::cout << "CDT - Constrained Delaunay Tetrahedrization (H-Si segment recovery)\n";
	std::cout << "Usage: " << programName << " <input.off> [options]\n";
	std::cout << "Options:\n";
	std::cout << "  -v: verbose mode\n";
	std::cout << "  -b: box mode (enclose everything in a bounding box)\n";
	std::cout << "  -l: log results to cdt_log.csv\n";
	std::cout << "  -w: log results to screen\n";
	std::cout << "  -f: force floating point representability\n";
	std::cout << "  -q: rational output (TET file only)\n";
	std::cout << "  -n: binary output (TET file only)\n";
	std::cout << "  -r: raw output (include ghost tetrahedra)\n";
	std::cout << "  -s: do not save skin to OFF file (saved by default)\n";
	std::cout << "  -m: save to MEDIT format instead of TET\n";
}

std::string extractBasename(const std::string& filepath) {
	const auto pos = filepath.find_last_of("/\\");
	return (pos == std::string::npos) ? filepath : filepath.substr(pos + 1);
}

void parseCommandLine(int argc, char* argv[], CDTOptions& cdtOpts, SaveOptions& saveOpts) {
	for (int i = 2; i < argc; ++i) {
		if (argv[i] && argv[i][0] == '-') {
			for (int j = 1; argv[i][j] != '\0'; ++j) {
				switch (argv[i][j]) {
				case 'v': cdtOpts.verbose = true; break;
				case 'b': cdtOpts.boundingBox = true; break;
				case 'l': cdtOpts.logToFile = true; break;
				case 'w': cdtOpts.logToScreen = true; break;
				case 'f': cdtOpts.snapToFloat = true; break;
				case 'q': saveOpts.rational = true; break;
				case 'n': saveOpts.binary = true; break;
				case 'r': saveOpts.raw = true; break;
				case 's': saveOpts.saveSkin = false; break;
				case 'm': saveOpts.meditFormat = true; break;
				}
			}
		}
	}
}
}

int main(int argc, char* argv[])
{
	if (argc < 2 || !argv[1] || argv[1][0] == '-') {
		printUsage(argv[0] ? argv[0] : "cdt");
		return 0;
	}

	const std::string filename = argv[1];
	CDTOptions cdtOpts;
	SaveOptions saveOpts;
	parseCommandLine(argc, argv, cdtOpts, saveOpts);

	InputPLC plc;
	if (!plc.initFromFile(filename.c_str(), true)) {
		std::cerr << "Error: Could not load " << filename << "\n";
		return 1;
	}

	initFPU();

	std::unique_ptr<TetMesh> tin = createSteinerCDT(plc, cdtOpts);

	if (!tin) {
		std::cerr << "Error: Could not create CDT\n";
		return 1;
	}

	const std::string basename = extractBasename(filename);

	if (saveOutputFile(*tin, basename, saveOpts)) {
		std::cout << "Finished\n";
	}

	return 0;
}
