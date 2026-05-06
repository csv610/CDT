# CDT User Guide: A Student's Companion to Constrained Delaunay Tetrahedrization

Welcome to the **CDT (Constrained Delaunay Tetrahedrization)** project. This guide is designed to help undergraduate students and researchers understand, build, and utilize this robust implementation of 3D triangulation algorithms.

---

## 1. Introduction: What is CDT?

In computational geometry, a **Delaunay Tetrahedrization** of a set of points is a decomposition of the 3D space into tetrahedra such that no point is inside the circumsphere of any tetrahedron. 

A **Constrained Delaunay Tetrahedrization (CDT)** extends this concept by requiring that certain "constraints"—usually triangles forming a surface—are present as faces in the final tetrahedrization. This is a notoriously difficult problem because a CDT doesn't always exist for every input without adding extra points, known as **Steiner points**.

This software implements the state-of-the-art algorithm described in:
> *“Constrained Delaunay Tetrahedrization: A robust and practical approach”* by L. Diazzi, D. Panozzo, A. Vaxman and M. Attene (SIGGRAPH Asia 2023).

### Why use this library?
- **Robustness**: It uses exact predicates to handle degenerate cases and numerical precision issues.
- **Practicality**: It focuses on real-world inputs (OFF files) and produces usable volumetric meshes.
- **Academic Foundation**: It is built on rigorous research.

---

## 2. Getting Started

### Prerequisites
You will need:
- A C++ compiler (GCC 11+, Clang, or MSVC 2022+)
- [CMake](https://cmake.org/) (version 3.10 or higher)

### Building the Project
Open your terminal and run the following commands:

```bash
# 1. Configure the project
cmake -B build -S .

# 2. Build the executable
cmake --build build --config Release
```

The resulting executable, `cdt`, will be located in the `build` directory (or `build/Release` on Windows).

**Note**: CMake will automatically find [Assimp](https://github.com/assimp/assimp) for loading additional 3D model formats. On macOS, install via `brew install assimp`; on Ubuntu, `apt install libassimp-dev`.

---

## 3. Using the Command Line Tool

The primary way to interact with the project is via the `cdt` executable.

### Basic Command
To generate a tetrahedrization for a surface mesh (in OFF format):

```bash
./cdt input_mesh.off
```

This will produce `input_mesh.off.tet`, which contains the volumetric mesh.

### Command Line Options
You can customize the behavior using flags:

| Flag | Description |
| :--- | :--- |
| `-a` | **Assimp**: Use Assimp to load 3D models (supports OBJ, GLTF, FBX, STL, etc.). |
| `-v` | **Verbose**: Prints detailed progress to the console. |
| `-b` | **Bounding Box**: Adds 8 vertices to enclose the model in a box. Helpful for visualization. |
| `-k` | **Keep Boundary**: Preserves the original input boundary without creating new vertices/faces. |
| `-r` | **Remove Outer**: Removes tetrahedra outside the closed input surface. |
| `-s` | **Save Skin**: Saves the boundary triangles between the inside and outside to an OFF file. |
| `-f` | **Float-friendly**: Tries to make the output representable using standard floating-point numbers. |
| `-q` | **Rational Output**: Saves coordinates using exact rational numbers (preventing precision loss). |
| `-n` | **Binary Output**: Saves the output in a compact binary format. |
| `-m` | **MEDIT Format**: Saves the mesh in the `.mesh` format used by the MEDIT visualizer. |
| `-w` | **Log to Screen**: Displays log data directly in the terminal. |
| `-l` | **Log to File**: Saves timing and statistics to cdt_log.csv. |

**Examples:**
```bash
# Basic usage with OFF file
./cdt -v -r -s my_model.off

# Load OBJ file using Assimp
./cdt -a -v my_model.obj

# Keep original boundary without creating new triangles
./cdt -a -k my_model.obj
```
*Note: The `-k` flag skips segment and face recovery, preserving the original boundary.*

---

## 4. Understanding Data Formats

### Input Formats

#### OFF (Object File Format) - Default
The default input format is OFF (Object File Format). The input should be a triangulated surface. A simple OFF file looks like this:
```
OFF
8 12 0
-0.5 -0.5 -0.5
 0.5 -0.5 -0.5
  ...
3 0 1 2
3 2 3 0
...
```

#### Other Formats via Assimp
Using the `-a` flag, you can load many additional 3D model formats:
- **OBJ** - Wavefront OBJ (most common)
- **GLTF/GLB** - GL Transmission Format (web-standard)
- **FBX** - Filmbox (common in game engines)
- **STL** - Stereolithography (common for 3D printing)
- **3DS, DAE, X** - Other 3D formats

Example:
```bash
./cdt -a my_model.obj
```

### Output: .tet
The default output format is `.tet`. It lists:
1. The number of vertices and their coordinates.
2. The number of tetrahedra and the indices of their four vertices.

---

## 5. Programming with the CDT API

If you are developing your own C++ application, you can integrate CDT directly using the provided API.

### Header Inclusion
Include the main API header in your code:
```cpp
#include "cdt_api.h"
```

### Core Workflow
The general pattern for using the library is:
1. Initialize an `inputPLC` (Piecewise Linear Complex).
2. Call `createSteinerCDT`.
3. Save the result using `saveOutputFile`.

#### Example Implementation:
```cpp
#include "cdt_api.h"

int main() {
    // 1. Prepare the input
    inputPLC plc;
    bool verbose = true;
    plc.initFromFile("model.off", verbose);

    // 2. Generate the CDT
    // Options string matches CLI flags (e.g., "vb" for verbose + bounding box)
    TetMesh* mesh = createSteinerCDT(plc, "v");

    if (mesh) {
        // 3. Save the result
        saveOutputFile(*mesh, "output_name", "r");
        delete mesh;
    }

    return 0;
}
```

### Key Classes
- **`inputPLC`**: Handles loading and preprocessing the input surface. It automatically removes duplicated vertices and degenerate triangles.
- **`TetMesh`**: The structure containing the resulting tetrahedrization.

---

## 6. Mathematical Foundations (For the Curious)

### Robustness via Exact Predicates
Floating-point arithmetic is often imprecise. In geometry, a tiny error in calculating whether a point is "left" or "right" of a line can cause the entire algorithm to crash. This library uses **Exact Geometric Predicates**, which ensures that topological decisions are always correct.

### Steiner Points
When the algorithm encounters a configuration where a Delaunay tetrahedrization cannot represent a required constraint, it "splits" edges or faces by adding a **Steiner point**. The goal of this implementation is to keep the number of these points as low as possible while ensuring a valid CDT.

---

## 7. Troubleshooting

- **"Non-triangular faces not supported"**: Ensure your input OFF file only contains triangles (the number `3` at the start of each face line).
- **"Degenerate triangles"**: The library detects these and removes them, but highly "dirty" geometry might still cause issues. Try cleaning your input mesh in a tool like MeshLab first.
- **Build Errors**: Ensure your compiler supports C++11 or higher.

---

## 8. License and Citation

This project is distributed under the **GPL/LGPL** license. 

If you use this software for academic research, please cite:
```bibtex
@article{CDT_SIGA23,
  author    = {Diazzi, Luca and Panozzo, Daniele and Vaxman, Amir and Attene, Marco},
  title     = {Constrained Delaunay Tetrahedrization: A robust and practical approach},
  journal   = {ACM Trans. Graph.},
  volume    = {42},
  number    = {6},
  year      = {2023},
  publisher = {ACM}
}
```
