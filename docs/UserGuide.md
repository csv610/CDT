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
- **Academic Foundation**: It is built on rigorous research and modern C++ engineering standards.

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
| `-r` | **Include Outer**: Includes tetrahedra outside the closed input surface (outer tetrahedra are excluded by default). |
| `-s` | **Do NOT Save Skin**: Prevents saving the boundary triangles (saved by default to an OFF file). |
| `-f` | **Float-friendly**: Tries to make the output representable using standard floating-point numbers. |
| `-q` | **Rational Output**: Saves coordinates using exact rational numbers (preventing precision loss). |
| `-n` | **Binary Output**: Saves the output in a compact binary format. |
| `-m` | **MEDIT Format**: Saves the mesh in the `.mesh` format used by the MEDIT visualizer. |
| `-w` | **Log to Screen**: Displays log data directly in the terminal. |
| `-l` | **Log to File**: Saves timing and statistics to cdt_log.csv. |

---

## 4. Programming with the CDT API

If you are developing your own C++ application, you can integrate CDT directly using the provided API.

### Header Inclusion
Include the main API header in your code:
```cpp
#include "Cdt.h"
```

### Core Workflow
The general pattern for using the library is:
1. Initialize an `InputPLC` (Piecewise Linear Complex).
2. Configure `CDTOptions` and `SaveOptions`.
3. Call `createSteinerCDT`.
4. Save the result using `saveOutputFile`.

#### Example Implementation:
```cpp
#include "Cdt.h"
#include <memory>

int main() {
    // 1. Prepare the input
    InputPLC plc;
    bool verbose = true;
    if (!plc.initFromFile("model.off", verbose)) return 1;

    // 2. Configure options
    CDTOptions cdtOpts;
    cdtOpts.verbose = true;

    SaveOptions saveOpts;
    saveOpts.raw = true; // Include outer tetrahedra

    // 3. Generate the CDT (returns a smart pointer)
    std::unique_ptr<TetMesh> mesh = createSteinerCDT(plc, cdtOpts);

    if (mesh) {
        // 4. Save the result
        saveOutputFile(*mesh, "output_name", saveOpts);
    }
    
    return 0;
}
```

### Key Classes
- **`InputPLC`**: Handles loading and preprocessing the input surface. It automatically removes duplicated vertices and degenerate triangles.
- **`TetMesh`**: The structure containing the resulting tetrahedrization.
- **`CDTOptions` / `SaveOptions`**: Type-safe configuration structs for the algorithm and output.

---

## 5. Mathematical Foundations (For the Curious)

### Robustness via Exact Predicates
Floating-point arithmetic is often imprecise. In geometry, a tiny error in calculating whether a point is "left" or "right" of a line can cause the entire algorithm to crash. This library uses **Exact Geometric Predicates**, which ensures that topological decisions are always correct.

### Steiner Points
When the algorithm encounters a configuration where a Delaunay tetrahedrization cannot represent a required constraint, it "splits" edges or faces by adding a **Steiner point**. The goal of this implementation is to keep the number of these points as low as possible while ensuring a valid CDT.

---

## 6. License and Citation

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
