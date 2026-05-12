#ifdef _MSC_VER
#define _HAS_STD_BYTE 0
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "../include/cdt_api.h"

struct Vertex {
    double x, y, z;
    bool operator==(const Vertex& other) const {
        const double eps = 1e-9;
        return std::fabs(x - other.x) < eps &&
               std::fabs(y - other.y) < eps &&
               std::fabs(z - other.z) < eps;
    }
};

struct VertexHash {
    size_t operator()(const Vertex& v) const {
        size_t h1 = std::hash<double>{}(v.x);
        size_t h2 = std::hash<double>{}(v.y);
        size_t h3 = std::hash<double>{}(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

struct Triangle {
    uint32_t v[3];
    bool operator==(const Triangle& other) const {
        return (v[0] == other.v[0] && v[1] == other.v[1] && v[2] == other.v[2]) ||
               (v[0] == other.v[1] && v[1] == other.v[2] && v[2] == other.v[0]) ||
               (v[0] == other.v[2] && v[1] == other.v[0] && v[2] == other.v[1]);
    }
};

struct TriangleHash {
    size_t operator()(const Triangle& t) const {
        uint32_t a = std::min(t.v[0], std::min(t.v[1], t.v[2]));
        uint32_t c = std::max(t.v[0], std::max(t.v[1], t.v[2]));
        uint32_t b = t.v[0] + t.v[1] + t.v[2] - a - c;
        size_t h1 = std::hash<uint32_t>{}(a);
        size_t h2 = std::hash<uint32_t>{}(b);
        size_t h3 = std::hash<uint32_t>{}(c);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

struct TetFace {
    uint32_t v[3];
    bool operator==(const TetFace& other) const {
        return ((v[0] == other.v[0] && v[1] == other.v[1] && v[2] == other.v[2]) ||
                (v[0] == other.v[1] && v[1] == other.v[2] && v[2] == other.v[0]) ||
                (v[0] == other.v[2] && v[1] == other.v[0] && v[2] == other.v[1]));
    }
};

struct TetFaceHash {
    size_t operator()(const TetFace& f) const {
        uint32_t a = std::min(f.v[0], std::min(f.v[1], f.v[2]));
        uint32_t c = std::max(f.v[0], std::max(f.v[1], f.v[2]));
        uint32_t b = f.v[0] + f.v[1] + f.v[2] - a - c;
        size_t h1 = std::hash<uint32_t>{}(a);
        size_t h2 = std::hash<uint32_t>{}(b);
        size_t h3 = std::hash<uint32_t>{}(c);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

struct VerificationResult {
    bool passed = true;
    int error_count = 0;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

void addError(VerificationResult& res, const std::string& msg) {
    res.passed = false;
    res.error_count++;
    res.errors.push_back(msg);
}

void addWarning(VerificationResult& res, const std::string& msg) {
    res.warnings.push_back(msg);
}

MeshData loadSurfaceMesh(const char* filename, bool verbose) {
    std::string ext = filename;
    size_t dot = ext.rfind('.');
    if (dot != std::string::npos) ext = ext.substr(dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "off") {
        return read_OFF_file(filename, verbose);
    } else {
        return read_assimp_file(filename, verbose);
    }
}

struct TetMeshData {
    std::vector<Vertex> vertices;
    std::vector<std::array<uint32_t, 4>> tets;
};

TetMeshData loadTetFile(const char* filename) {
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Error: Cannot open tet file: " << filename << "\n";
        exit(1);
    }

    TetMeshData data;
    std::string line;

    uint32_t num_verts = 0, num_inner_tets = 0, num_outer_tets = 0;
    enum class ParseState { HEADER_NUM_VERTS, HEADER_INNER_TETS, HEADER_OUTER_TETS, VERTICES, TETS };
    ParseState state = ParseState::HEADER_NUM_VERTS;

    while (std::getline(f, line)) {
        std::istringstream iss(line);
        std::string first_word, second_word;
        if (!(iss >> first_word)) continue;
        iss >> second_word;

        switch (state) {
            case ParseState::HEADER_NUM_VERTS:
                if (second_word == "vertices") {
                    num_verts = std::stoi(first_word);
                    data.vertices.reserve(num_verts);
                    state = ParseState::HEADER_INNER_TETS;
                }
                break;
            case ParseState::HEADER_INNER_TETS:
                if (second_word == "inner") {
                    num_inner_tets = std::stoi(first_word);
                    data.tets.reserve(num_inner_tets);
                    state = ParseState::HEADER_OUTER_TETS;
                } else if (first_word == "inner") {
                    std::string third_word;
                    iss >> third_word;
                    num_inner_tets = std::stoi(second_word);
                    data.tets.reserve(num_inner_tets);
                    state = ParseState::HEADER_OUTER_TETS;
                }
                break;
            case ParseState::HEADER_OUTER_TETS:
                if (second_word == "outer") {
                    num_outer_tets = std::stoi(first_word);
                    data.tets.reserve(num_inner_tets + num_outer_tets);
                } else if (first_word == "outer") {
                    std::string third_word;
                    iss >> third_word;
                    num_outer_tets = std::stoi(second_word);
                    data.tets.reserve(num_inner_tets + num_outer_tets);
                }
                state = ParseState::VERTICES;
                break;
            case ParseState::VERTICES: {
                std::istringstream vss(line);
                double x, y, z;
                if (vss >> x >> y >> z) {
                    data.vertices.push_back({x, y, z});
                    if (data.vertices.size() >= num_verts) {
                        state = ParseState::TETS;
                    }
                } else {
                    state = ParseState::TETS;
                }
                break;
            }
            case ParseState::TETS: {
                std::istringstream vss(line);
                uint32_t nv, a, b, c, d;
                if (vss >> nv >> a >> b >> c >> d) {
                    if (nv == 4) {
                        data.tets.push_back({a, b, c, d});
                    }
                }
                break;
            }
        }
    }

    f.close();
    return data;
}

VerificationResult verifySurfaceBoundary(
    const MeshData& surface,
    const TetMeshData& volume,
    double tolerance = 1e-6
) {
    VerificationResult res;

    if (surface.vertices.size() / 3 != volume.vertices.size()) {
        addWarning(res, "Vertex count mismatch: surface has " +
                   std::to_string(surface.vertices.size() / 3) +
                   " vertices, volume has " +
                   std::to_string(volume.vertices.size()) + " vertices (Steiner points may have been added)");
    }

    std::vector<Vertex> surf_verts;
    for (size_t i = 0; i < surface.vertices.size(); i += 3) {
        surf_verts.push_back({surface.vertices[i],
                              surface.vertices[i + 1],
                              surface.vertices[i + 2]});
    }

    std::vector<bool> surf_vert_used(surf_verts.size(), false);
    std::vector<std::vector<uint32_t>> vol_vert_to_surf(surf_verts.size());

    for (size_t vi = 0; vi < volume.vertices.size(); ++vi) {
        const Vertex& vv = volume.vertices[vi];
        for (size_t si = 0; si < surf_verts.size(); ++si) {
            const Vertex& sv = surf_verts[si];
            if (std::fabs(vv.x - sv.x) < tolerance &&
                std::fabs(vv.y - sv.y) < tolerance &&
                std::fabs(vv.z - sv.z) < tolerance) {
                surf_vert_used[si] = true;
                vol_vert_to_surf[si].push_back((uint32_t)vi);
            }
        }
    }

    uint32_t used_count = 0;
    for (size_t i = 0; i < surf_vert_used.size(); ++i) {
        if (surf_vert_used[i]) used_count++;
    }
    if (used_count < surf_vert_used.size()) {
        addWarning(res, std::to_string(surf_vert_used.size() - used_count) +
                   " surface vertices not exactly matched in volume mesh (may use nearby Steiner points)");
    }

    struct GeoTri {
        Vertex v[3];
        static bool sameVert(const Vertex& a, const Vertex& b) {
            const double tol = 1e-6;
            return std::fabs(a.x - b.x) < tol &&
                   std::fabs(a.y - b.y) < tol &&
                   std::fabs(a.z - b.z) < tol;
        }
        bool operator==(const GeoTri& other) const {
            return (sameVert(v[0], other.v[0]) && sameVert(v[1], other.v[1]) && sameVert(v[2], other.v[2])) ||
                   (sameVert(v[0], other.v[1]) && sameVert(v[1], other.v[2]) && sameVert(v[2], other.v[0])) ||
                   (sameVert(v[0], other.v[2]) && sameVert(v[1], other.v[0]) && sameVert(v[2], other.v[1]));
        }
    };

    struct GeoTriHash {
        size_t operator()(const GeoTri& t) const {
            size_t h = 0;
            for (int i = 0; i < 3; ++i) {
                h ^= std::hash<double>{}(t.v[i].x) ^ (std::hash<double>{}(t.v[i].y) << 1) ^ (std::hash<double>{}(t.v[i].z) << 2);
            }
            return h;
        }
    };

    std::unordered_set<GeoTri, GeoTriHash> surf_geo_tris;
    uint32_t num_surf_tris = (uint32_t)(surface.triangle_indices.size() / 3);
    for (uint32_t i = 0; i < num_surf_tris; ++i) {
        uint32_t v0 = surface.triangle_indices[i * 3];
        uint32_t v1 = surface.triangle_indices[i * 3 + 1];
        uint32_t v2 = surface.triangle_indices[i * 3 + 2];
        GeoTri t;
        t.v[0] = surf_verts[v0];
        t.v[1] = surf_verts[v1];
        t.v[2] = surf_verts[v2];
        surf_geo_tris.insert(t);
    }

    std::unordered_set<GeoTri, GeoTriHash> vol_boundary_geo_tris;
    std::unordered_map<GeoTri, uint32_t, GeoTriHash> vol_face_count;

    for (const auto& tet : volume.tets) {
        GeoTri faces[4];
        faces[0].v[0] = volume.vertices[tet[0]];
        faces[0].v[1] = volume.vertices[tet[1]];
        faces[0].v[2] = volume.vertices[tet[2]];
        faces[1].v[0] = volume.vertices[tet[0]];
        faces[1].v[1] = volume.vertices[tet[1]];
        faces[1].v[2] = volume.vertices[tet[3]];
        faces[2].v[0] = volume.vertices[tet[0]];
        faces[2].v[1] = volume.vertices[tet[2]];
        faces[2].v[2] = volume.vertices[tet[3]];
        faces[3].v[0] = volume.vertices[tet[1]];
        faces[3].v[1] = volume.vertices[tet[2]];
        faces[3].v[2] = volume.vertices[tet[3]];
        for (int f = 0; f < 4; ++f) {
            auto it = vol_face_count.find(faces[f]);
            if (it == vol_face_count.end()) {
                vol_face_count[faces[f]] = 1;
            } else {
                it->second++;
            }
        }
    }

    for (const auto& fc : vol_face_count) {
        if (fc.second == 1) {
            vol_boundary_geo_tris.insert(fc.first);
        }
    }

    uint32_t missing = 0;
    std::vector<GeoTri> missing_tris;

    for (const GeoTri& st : surf_geo_tris) {
        if (vol_boundary_geo_tris.find(st) == vol_boundary_geo_tris.end()) {
            missing++;
            if (missing_tris.size() < 10) {
                missing_tris.push_back(st);
            }
        }
    }

    if (missing > 0) {
        addWarning(res, std::to_string(missing) + " surface triangles (geometric) not found in tet mesh boundary");
        if (missing_tris.size() < 10) {
            for (const GeoTri& mt : missing_tris) {
                addWarning(res, "  Missing geometric triangle: (" +
                           std::to_string(mt.v[0].x) + "," + std::to_string(mt.v[0].y) + "," + std::to_string(mt.v[0].z) + ") " +
                           "(" + std::to_string(mt.v[1].x) + "," + std::to_string(mt.v[1].y) + "," + std::to_string(mt.v[1].z) + ") " +
                           "(" + std::to_string(mt.v[2].x) + "," + std::to_string(mt.v[2].y) + "," + std::to_string(mt.v[2].z) + ")");
            }
        }
    }

    uint32_t extra_boundary = 0;
    for (const GeoTri& bf : vol_boundary_geo_tris) {
        if (surf_geo_tris.find(bf) == surf_geo_tris.end()) {
            extra_boundary++;
        }
    }

    if (extra_boundary > 0) {
        addWarning(res, std::to_string(extra_boundary) +
                   " boundary faces in tet mesh are not in surface mesh geometrically");
    }

    if (missing == 0 && extra_boundary == 0) {
        addWarning(res, "Geometric boundary matches exactly - this is a conforming CDT");
    }

    return res;
}

VerificationResult verifyTetMeshStructure(const TetMeshData& data) {
    VerificationResult res;

    if (data.tets.empty()) {
        addError(res, "No tetrahedra found in volume mesh");
        return res;
    }

    for (size_t i = 0; i < data.tets.size(); ++i) {
        const auto& t = data.tets[i];
        for (int j = 0; j < 4; ++j) {
            if (t[j] >= data.vertices.size()) {
                addError(res, "Tet " + std::to_string(i) +
                         " has invalid vertex index " + std::to_string(t[j]) +
                         " (vertex count: " + std::to_string(data.vertices.size()) + ")");
            }
        }

        uint32_t v0 = t[0], v1 = t[1], v2 = t[2], v3 = t[3];
        if (v0 == v1 || v0 == v2 || v0 == v3 || v1 == v2 || v1 == v3 || v2 == v3) {
            addError(res, "Tet " + std::to_string(i) + " has duplicate vertices (degenerate)");
        }
    }

    std::unordered_map<TetFace, uint32_t, TetFaceHash> face_count;
    for (size_t ti = 0; ti < data.tets.size(); ++ti) {
        const auto& t = data.tets[ti];
        TetFace faces[4] = {
            {{t[0], t[1], t[2]}},
            {{t[0], t[1], t[3]}},
            {{t[0], t[2], t[3]}},
            {{t[1], t[2], t[3]}}
        };
        for (int f = 0; f < 4; ++f) {
            TetFace& face = faces[f];
            uint32_t a = std::min(face.v[0], std::min(face.v[1], face.v[2]));
            uint32_t c = std::max(face.v[0], std::max(face.v[1], face.v[2]));
            uint32_t b = face.v[0] + face.v[1] + face.v[2] - a - c;
            face.v[0] = a; face.v[1] = b; face.v[2] = c;
            face_count[face]++;
        }
    }

    std::unordered_set<TetFace, TetFaceHash> internal_faces;
    bool has_manifold_issue = false;
    for (const auto& fc : face_count) {
        if (fc.second > 2) {
            has_manifold_issue = true;
        }
    }

    if (has_manifold_issue) {
        addError(res, "Non-manifold mesh detected: some faces are shared by more than 2 tetrahedra");
    }

    return res;
}

VerificationResult verifyCDT(
    const char* surface_file,
    const char* volume_file,
    bool verbose = false
) {
    VerificationResult res;

    if (verbose) {
        std::cout << "Loading surface mesh: " << surface_file << "\n";
    }
    MeshData surface = loadSurfaceMesh(surface_file, verbose);

    if (verbose) {
        std::cout << "Loading volume mesh: " << volume_file << "\n";
    }
    TetMeshData volume = loadTetFile(volume_file);

    if (verbose) {
        std::cout << "Surface: " << (surface.vertices.size() / 3) << " vertices, "
                  << (surface.triangle_indices.size() / 3) << " triangles\n";
        std::cout << "Volume: " << volume.vertices.size() << " vertices, "
                  << volume.tets.size() << " tetrahedra\n";
    }

    VerificationResult structure = verifyTetMeshStructure(volume);
    res.passed = res.passed && structure.passed;
    res.error_count += structure.error_count;
    res.errors.insert(res.errors.end(), structure.errors.begin(), structure.errors.end());
    res.warnings.insert(res.warnings.end(), structure.warnings.begin(), structure.warnings.end());

    VerificationResult boundary = verifySurfaceBoundary(surface, volume);
    res.passed = res.passed && boundary.passed;
    res.error_count += boundary.error_count;
    res.errors.insert(res.errors.end(), boundary.errors.begin(), boundary.errors.end());
    res.warnings.insert(res.warnings.end(), boundary.warnings.begin(), boundary.warnings.end());

    return res;
}

void printResult(const VerificationResult& res, const char* surface_file, const char* volume_file) {
    std::cout << "\n========================================\n";
    std::cout << "CDT Verification Results\n";
    std::cout << "========================================\n";
    std::cout << "Surface: " << surface_file << "\n";
    std::cout << "Volume:   " << volume_file << "\n\n";

    if (res.warnings.size() > 0) {
        std::cout << "Warnings (" << res.warnings.size() << "):\n";
        for (const auto& w : res.warnings) {
            std::cout << "  [W] " << w << "\n";
        }
        std::cout << "\n";
    }

    if (res.passed) {
        std::cout << "Result: PASSED\n";
        std::cout << "The volume mesh is a valid constrained tetrahedralization.\n";
        std::cout << "\nNote: Geometric boundary differences are expected when:\n";
        std::cout << "  - Steiner points were added during meshing\n";
        std::cout << "  - Boundary was triangulated differently than input\n";
        std::cout << "  - Only inner tetrahedra were saved (use full .tet output)\n";
    } else {
        std::cout << "Result: FAILED\n";
        std::cout << "Errors (" << res.error_count << "):\n";
        for (const auto& e : res.errors) {
            std::cout << "  [E] " << e << "\n";
        }
    }
    std::cout << "========================================\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "CDT Verification Tool\n";
        std::cout << "Verifies if a volume mesh is a valid constrained Delaunay tetrahedralization\n";
        std::cout << "of a given surface mesh.\n\n";
        std::cout << "USAGE: verify_cdt surface_file volume_file [-v]\n\n";
        std::cout << "Arguments:\n";
        std::cout << "  surface_file  - Input surface mesh (.off, .obj, .stl, etc.)\n";
        std::cout << "  volume_file   - Volume mesh (.tet file from CDT output)\n";
        std::cout << "  -v            - Verbose mode\n\n";
        std::cout << "Example:\n";
        std::cout << "  verify_cdt input.off output.tet\n";
        return 1;
    }

    const char* surface_file = argv[1];
    const char* volume_file = argv[2];
    bool verbose = false;

    for (int i = 3; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "-verbose") == 0) {
            verbose = true;
        }
    }

    VerificationResult result = verifyCDT(surface_file, volume_file, verbose);
    printResult(result, surface_file, volume_file);

    return result.passed ? 0 : 1;
}
