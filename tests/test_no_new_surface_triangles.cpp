#include "test_macros.h"
#include "../include/cdt_api.h"
#include <cmath>
#include <unordered_set>
#include <vector>
#include <algorithm>

struct GeoTri {
    double v[9];
    bool operator==(const GeoTri& other) const {
        for (int i = 0; i < 9; ++i) {
            if (std::fabs(v[i] - other.v[i]) > 1e-3) return false;
        }
        return true;
    }
};

struct GeoTriHash {
    size_t operator()(const GeoTri& t) const {
        size_t h = 0;
        for (int i = 0; i < 9; ++i) {
            h ^= std::hash<double>{}(std::floor(t.v[i] * 1000 + 0.5));
        }
        return h;
    }
};

GeoTri makeGeoTri(double x0, double y0, double z0,
                  double x1, double y1, double z1,
                  double x2, double y2, double z2) {
    GeoTri t;
    t.v[0] = x0; t.v[1] = y0; t.v[2] = z0;
    t.v[3] = x1; t.v[4] = y1; t.v[5] = z1;
    t.v[6] = x2; t.v[7] = y2; t.v[8] = z2;
    return t;
}

double triangleArea(double x0, double y0, double z0,
                    double x1, double y1, double z1,
                    double x2, double y2, double z2) {
    double ax = x1 - x0, ay = y1 - y0, az = z1 - z0;
    double bx = x2 - x0, by = y2 - y0, bz = z2 - z0;
    double cx = ay * bz - az * by;
    double cy = az * bx - ax * bz;
    double cz = ax * by - ay * bx;
    return std::sqrt(cx * cx + cy * cy + cz * cz) * 0.5;
}

double triangleArea(const GeoTri& t) {
    return triangleArea(t.v[0], t.v[1], t.v[2], t.v[3], t.v[4], t.v[5], t.v[6], t.v[7], t.v[8]);
}

void extractBoundaryTris(TetMesh* mesh, std::unordered_map<GeoTri, int, GeoTriHash>& face_count) {
    uint32_t num_tets = mesh->numTets();
    for (uint32_t t = 0; t < num_tets; ++t) {
        const uint32_t* tet_nodes = mesh->getTetNodes(t);
        if (tet_nodes[0] == INFINITE_VERTEX || tet_nodes[1] == INFINITE_VERTEX ||
            tet_nodes[2] == INFINITE_VERTEX || tet_nodes[3] == INFINITE_VERTEX) continue;

        GeoTri faces[4];
        for (int i = 0; i < 4; ++i) {
            uint32_t v0 = tet_nodes[i];
            uint32_t v1 = tet_nodes[(i + 1) & 3];
            uint32_t v2 = tet_nodes[(i + 2) & 3];
            double x0, y0, z0, x1, y1, z1, x2, y2, z2;
            mesh->vertices[v0]->getApproxXYZCoordinates(x0, y0, z0);
            mesh->vertices[v1]->getApproxXYZCoordinates(x1, y1, z1);
            mesh->vertices[v2]->getApproxXYZCoordinates(x2, y2, z2);
            GeoTri ft;
            ft.v[0] = x0; ft.v[1] = y0; ft.v[2] = z0;
            ft.v[3] = x1; ft.v[4] = y1; ft.v[5] = z1;
            ft.v[6] = x2; ft.v[7] = y2; ft.v[8] = z2;
            faces[i] = ft;
        }
        for (int i = 0; i < 4; ++i) {
            face_count[faces[i]]++;
        }
    }
}

void buildInputTriSet(const double* coords, const uint32_t* triangles, int num_tris,
                      std::unordered_set<GeoTri, GeoTriHash>& input_tris) {
    for (int i = 0; i < num_tris; ++i) {
        uint32_t i0 = triangles[i * 3];
        uint32_t i1 = triangles[i * 3 + 1];
        uint32_t i2 = triangles[i * 3 + 2];
        double x0 = coords[i0 * 3], y0 = coords[i0 * 3 + 1], z0 = coords[i0 * 3 + 2];
        double x1 = coords[i1 * 3], y1 = coords[i1 * 3 + 1], z1 = coords[i1 * 3 + 2];
        double x2 = coords[i2 * 3], y2 = coords[i2 * 3 + 1], z2 = coords[i2 * 3 + 2];
        input_tris.insert(makeGeoTri(x0, y0, z0, x1, y1, z1, x2, y2, z2));
    }
}

void showBoundaryTris(const std::unordered_map<GeoTri, int, GeoTriHash>& face_count, int limit = 5) {
    int count = 0;
    for (const auto& fc : face_count) {
        if (fc.second == 1) {
            double area = triangleArea(fc.first);
            if (area > 0.01) {
                std::cerr << "Boundary tri: v=(" << fc.first.v[0] << "," << fc.first.v[1] << "," << fc.first.v[2] << ") ("
                          << fc.first.v[3] << "," << fc.first.v[4] << "," << fc.first.v[5] << ") ("
                          << fc.first.v[6] << "," << fc.first.v[7] << "," << fc.first.v[8] << ") area=" << area << std::endl;
                count++;
                if (count >= limit) break;
            }
        }
    }
}

TEST(test_no_new_surface_cube) {
    double coords[] = {
        0, 0, 0,
        1, 0, 0,
        1, 1, 0,
        0, 1, 0,
        0, 0, 1,
        1, 0, 1,
        1, 1, 1,
        0, 1, 1
    };
    uint32_t triangles[] = {
        0, 2, 1,
        4, 5, 6,
        4, 6, 7,
        0, 4, 1,
        1, 4, 5,
        2, 3, 6,
        2, 6, 7,
        0, 3, 4,
        3, 7, 4
    };

    inputPLC plc;
    plc.initFromVectors(coords, 8, triangles, 9, false);
    plc.addBoundingBoxVertices();

    initFPU();
    TetMesh* mesh = createSteinerCDT(plc, "");
    ASSERT_TRUE(mesh != nullptr);

    uint32_t num_tets = mesh->numTets();
    ASSERT_TRUE(num_tets > 0);

    std::unordered_map<GeoTri, int, GeoTriHash> face_count;
    extractBoundaryTris(mesh, face_count);

    std::unordered_set<GeoTri, GeoTriHash> input_tris;
    buildInputTriSet(coords, triangles, 9, input_tris);

    ASSERT_TRUE(input_tris.size() > 0);

    int found = 0;
    int total_input_boundary = 0;
    for (const auto& fc : face_count) {
        if (fc.second == 1) {
            double area = triangleArea(fc.first);
            if (area > 0.01) {
                total_input_boundary++;
                if (input_tris.find(fc.first) != input_tris.end()) {
                    found++;
                }
            }
        }
    }

    std::cerr << "Input triangles=" << input_tris.size() << " boundary large tris=" << total_input_boundary << " found=" << found << std::endl;

    ASSERT_EQ(found, input_tris.size());

    delete mesh;
}

TEST(test_no_new_surface_tetrahedron) {
    double coords[] = {
        0, 0, 0,
        1, 0, 0,
        0.5, 1, 0,
        0.5, 0.5, 1
    };
    uint32_t triangles[] = {
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2
    };

    inputPLC plc;
    plc.initFromVectors(coords, 4, triangles, 4, false);
    plc.addBoundingBoxVertices();

    initFPU();
    TetMesh* mesh = createSteinerCDT(plc, "");
    ASSERT_TRUE(mesh != nullptr);

    uint32_t num_tets = mesh->numTets();
    ASSERT_TRUE(num_tets > 0);

    std::unordered_map<GeoTri, int, GeoTriHash> face_count;
    extractBoundaryTris(mesh, face_count);

    std::unordered_set<GeoTri, GeoTriHash> input_tris;
    buildInputTriSet(coords, triangles, 4, input_tris);

    double input_area = 0;
    for (const auto& itri : input_tris) {
        input_area += triangleArea(itri);
    }

    ASSERT_TRUE(surfaceAreaConsistent(input_area, face_count, 0.05));

    delete mesh;
}

TEST(test_no_new_surface_octahedron) {
    double coords[] = {
        1, 0, 0,
        0, 1, 0,
        -1, 0, 0,
        0, -1, 0,
        0, 0, 1,
        0, 0, -1
    };
    uint32_t triangles[] = {
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4,
        1, 0, 5,
        2, 1, 5,
        3, 2, 5,
        0, 3, 5
    };

    inputPLC plc;
    plc.initFromVectors(coords, 6, triangles, 8, false);
    plc.addBoundingBoxVertices();

    initFPU();
    TetMesh* mesh = createSteinerCDT(plc, "");
    ASSERT_TRUE(mesh != nullptr);

    uint32_t num_tets = mesh->numTets();
    ASSERT_TRUE(num_tets > 0);

    std::unordered_map<GeoTri, int, GeoTriHash> face_count;
    extractBoundaryTris(mesh, face_count);

    std::unordered_set<GeoTri, GeoTriHash> input_tris;
    buildInputTriSet(coords, triangles, 8, input_tris);

    double input_area = 0;
    for (const auto& itri : input_tris) {
        input_area += triangleArea(itri);
    }

    ASSERT_TRUE(surfaceAreaConsistent(input_area, face_count, 0.05));

    delete mesh;
}