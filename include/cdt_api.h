#ifndef CDT_API_H
#define CDT_API_H

#include "delaunay.h"
#include "inputPLC.h"

/**
 * createSteinerCDT
 * 
 * 'plc' is a valid input PLC to the process. Validity is assumed but not verified!
 * 'options' is a (possibly empty) string of characters, each controlling
 * one option as follows:
 * l: log results to cdt_log.csv
 * b: add eight vertices to enclose everything in a box
 * v: verbose mode
 * f: try to make the output representable using floating point
 * w: log to screen
 */
TetMesh* createSteinerCDT(inputPLC& plc, const char *options);

/**
 * saveOutputFile
 * 
 * 'tin' is a characterized tet mesh produced by the function above.
 * 'filename' is the name of the output file without extension.
 * The file produced will be called 'filename.tet' and/or
 * 'filename.off' (if 's' option is used).
 * 'options' is a (possibly empty) string of characters, each controlling
 * one option as follows:
 * q: rational output
 * n: binary output
 * r: remove outer tetrahedra from output (if input is closed)
 * s: saves skin to an ASCII OFF file (triangles between IN and OUT)
 * m: saves mesh to MEDIT format instead of TET
 */
bool saveOutputFile(TetMesh& tin, const char* filename, const char* options);

#endif // CDT_API_H
