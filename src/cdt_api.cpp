#ifdef _MSC_VER // Workaround for known bug on MSVC
#define _HAS_STD_BYTE 0  // https://developercommunity.visualstudio.com/t/error-c2872-byte-ambiguous-symbol/93889
#endif

#include <iostream>
#include <fstream>
#include <cstring>
#include "cdt_api.h"
#include "PLC.h"
#include "logger.h"

TetMesh* createSteinerCDT(inputPLC& plc, const char *options) {
	bool log = false, bbox = false, verbose = false, snap = false, logscreen = false;
	//bool optimize = false;

	for (int i = 0; i < strlen(options); i++) switch (options[i]) {
	case 'l':
		log = true; break;
	case 'b':
		bbox = true; break;
	case 'v':
		verbose = true; break;
	case 'w':
		logscreen = true; break;
	case 'f':
		snap = true; break;
	//case 'o':
	//	optimize = true; break;
	} // Just ignore unknown options

	if (bbox) plc.addBoundingBoxVertices();

	if (logscreen) {
		log = true;
		startLogging(NULL);
	}
	else if (log) startLogging(plc.input_file_name);

	// Build a delaunay tetrahedrization of the vertices
	TetMesh  *tin = new TetMesh;
	tin->init_vertices(plc.coordinates.data(), plc.numVertices());
	tin->tetrahedrize();

	if (verbose) printf("DT of the vertices built\n");

	if (log) logTimeChunk();

	// Build a structured PLC linked to the Delaunay tetrahedrization
	PLCx Steiner_plc(*tin, plc.triangle_vertices.data(), plc.numTriangles());

	// Recover segments by inserting Steiner points in both the PLC and the tetrahedrization
	Steiner_plc.segmentRecovery_HSi(!verbose);

	if (log) logTimeChunk();

	// Recover PLC faces by locally remeshing the tetrahedrization
	bool sisMethodWorks = Steiner_plc.faceRecovery(!verbose);

	if (log) logTimeChunk();

	// Mark the tets which are bounded by the PLC.
	// If the PLC is not a valid polyhedron (i.e. it has odd-valency edges)
	// all the tets but the ghosts are marked as "internal".
	uint32_t num_inner_tets = (uint32_t)Steiner_plc.markInnerTets();

	if (log) logTimeChunk();

	if (log) {
		logMemInfo();
		logBoolean(Steiner_plc.is_polyhedron);
		logInteger(plc.numVertices());
		logInteger(Steiner_plc.input_nt);
		logInteger(Steiner_plc.numSteinerVertices());
		logInteger(tin->countNonGhostTets());
		logInteger(num_inner_tets);
		size_t nflip, nflat;
		tin->hasBadSnappedOrientations(nflip, nflat);
		logInteger((uint32_t)nflat);
		logInteger((uint32_t)nflip);
		logBoolean(sisMethodWorks);
		finishLogging();
	}

	if (snap) {
		if (!tin->optimizeNearDegenerateTets(verbose)) {
			std::cerr << "Could not force FP representability.\n";
		}
	}

	//if (optimize) tin->optimizeMesh();

	return tin;
}

bool saveOutputFile(TetMesh& tin, const char* filename, const char* options) {
	bool rational = false, binary = false, erode = false, skin = false, medit = false;
	for (int i = 0; i < strlen(options); i++) switch (options[i]) {
	case 'q':
		rational = true; break;
	case 'n':
		binary = true; break;
	case 'r':
		erode = true; break;
	case 's':
		skin = true; break;
	case 'm':
		medit = true; break;
	}

	char tetfilename[2048], offfilename[2048];

	bool ret = true;

	if (medit) {
		if (rational || binary) {
			std::cerr << "Rational and binary modes are not available when saving to MEDIT format\n";
			ret = false;
		}
		else {
			sprintf(tetfilename, "%s.mesh", filename);
			ret &= tin.saveMEDIT(tetfilename, erode);
		}
	}
	else {
		sprintf(tetfilename, "%s.tet", filename);
		if (!rational && !binary) ret &= tin.saveTET(tetfilename, erode);
		if (!rational && binary) ret &= tin.saveBinaryTET(tetfilename, erode);
		if (rational && !binary) ret &= tin.saveRationalTET(tetfilename, erode);
		if (rational && binary) {
			std::cerr << "Save to rational is supported only in ASCII mode\n";
			ret = false;
		}
	}

	if (skin) {
		sprintf(offfilename, "%s.off", filename);
		ret &= tin.saveBoundaryToOFF(offfilename);
	}

	return ret;
}
