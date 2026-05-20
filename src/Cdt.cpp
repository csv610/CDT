#ifdef _MSC_VER // Workaround for known bug on MSVC
#define _HAS_STD_BYTE 0  // https://developercommunity.visualstudio.com/t/error-c2872-byte-ambiguous-symbol/93889
#endif

#include "Cdt.h"
#include "Plc.h"
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <cstring>

std::unique_ptr<TetMesh> createSteinerCDT(InputPLC& plc, const CDTOptions& options) {
	bool log = options.logToFile || options.logToScreen;
	bool verbose = options.verbose;

	if (options.boundingBox) plc.addBoundingBoxVertices();

	if (options.logToScreen) {
		startLogging(nullptr);
	}
	else if (options.logToFile) {
		startLogging(plc.input_file_name);
	}

	// Build a delaunay tetrahedrization of the vertices
	auto tin = std::make_unique<TetMesh>();
	tin->init_vertices(plc.coordinates.data(), plc.numVertices());
	tin->tetrahedrize();

	if (verbose) printf("DT of the vertices built\n");

	if (log) logTimeChunk();

	// Build a structured PLC linked to the Delaunay tetrahedrization
	Plcx Steiner_plc(*tin, plc.triangle_vertices.data(), plc.numTriangles());

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

	if (options.snapToFloat) {
		if (!tin->optimizeNearDegenerateTets(verbose)) {
			std::cerr << "Could not force FP representability.\n";
		}
	}

	return tin;
}

bool saveOutputFile(TetMesh& tin, const std::string& filename, const SaveOptions& options) {
	char tetfilename[2048], offfilename[2048];
	bool ret = true;
	bool erode = !options.raw;

	if (options.meditFormat) {
		if (options.rational || options.binary) {
			std::cerr << "Rational and binary modes are not available when saving to MEDIT format\n";
			ret = false;
		}
		else {
			snprintf(tetfilename, sizeof(tetfilename), "%s.mesh", filename.c_str());
			ret &= tin.saveMEDIT(tetfilename, erode);
		}
	}
	else {
		snprintf(tetfilename, sizeof(tetfilename), "%s.tet", filename.c_str());
		if (!options.rational && !options.binary) ret &= tin.saveTET(tetfilename, erode);
		if (!options.rational && options.binary) ret &= tin.saveBinaryTET(tetfilename, erode);
		if (options.rational && !options.binary) ret &= tin.saveRationalTET(tetfilename, erode);
		if (options.rational && options.binary) {
			std::cerr << "Save to rational is supported only in ASCII mode\n";
			ret = false;
		}
	}

	if (options.saveSkin) {
		snprintf(offfilename, sizeof(offfilename), "%s.off", filename.c_str());
		ret &= tin.saveBoundaryToOFF(offfilename);
	}

	return ret;
}
