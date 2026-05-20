#ifndef _CDT_H_
#define _CDT_H_

#include <memory>
#include <string>
#include "Delaunay.h"
#include "InputPLC.h"

struct CDTOptions {
    bool verbose = false;
    bool boundingBox = false;
    bool logToFile = false;
    bool logToScreen = false;
    bool snapToFloat = false;
};

struct SaveOptions {
    bool rational = false;
    bool binary = false;
    bool raw = false;
    bool saveSkin = true;
    bool meditFormat = false;
};

// createSteinerCDT
// 
// 'plc' is a valid input PLC to the process. Validity is assumed but not verified!
// 'options' is a struct controlling the process options.
std::unique_ptr<TetMesh> createSteinerCDT(InputPLC& plc, const CDTOptions& options);

// saveOutputFile
// 
// 'tin' is a characterized tet mesh produced by the function above.
// 'filename' is the name of the output file without extension.
// 'options' is a struct controlling the saving options.
bool saveOutputFile(TetMesh& tin, const std::string& filename, const SaveOptions& options);

#endif // _CDT_H_
