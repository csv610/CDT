#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

class reVertex {
public:
    double c[3];
    uint32_t index;

    reVertex(double* _c, uint32_t _i) : c{ _c[0], _c[1], _c[2] }, index(_i) {}

    static bool lessThanOnX(const reVertex& a, const reVertex& b) { return (a.c[0] < b.c[0]); }
    static bool lessThanOnY(const reVertex& a, const reVertex& b) { return (a.c[1] < b.c[1]); }
    static bool lessThanOnZ(const reVertex& a, const reVertex& b) { return (a.c[2] < b.c[2]); }
};

class vBlock {
public:
    uint32_t begin, end;
    uint32_t dir_split;

    vBlock(uint32_t b, uint32_t e, uint32_t d) : begin(b), end(e), dir_split(d) {}
};

inline void reorderVertices(double* coordinates, uint32_t numVertices, uint32_t* triVertices, uint32_t numTriangles) {
    std::vector<reVertex> revertices;
    revertices.reserve(numVertices);
    for (uint32_t i = 0; i < numVertices; i++) revertices.push_back(reVertex(coordinates + (i * 3), i));

    std::vector<vBlock> blocks;
    blocks.push_back(vBlock(0, numVertices, 0));

    for (uint32_t b = 0; b < blocks.size(); b++) {
        const vBlock block = blocks[b];
        const uint32_t dir_split = block.dir_split;
        if (dir_split == 0) std::sort(revertices.begin() + block.begin, revertices.begin() + block.end, reVertex::lessThanOnX);
        else if (dir_split == 1) std::sort(revertices.begin() + block.begin, revertices.begin() + block.end, reVertex::lessThanOnY);
        else std::sort(revertices.begin() + block.begin, revertices.begin() + block.end, reVertex::lessThanOnZ);
        const uint32_t block_mid = (block.begin + block.end) >> 1;
        if (block_mid != block.begin && block_mid != block.end) {
            blocks.push_back(vBlock(block.begin, block_mid, (dir_split + 1) % 3));
            blocks.push_back(vBlock(block_mid, block.end, (dir_split + 1) % 3));
        }
    }

    std::vector<uint32_t> new_index(numVertices);
    for (uint32_t i = 0; i < numVertices; i++) new_index[revertices[i].index] = i;

    for (uint32_t i = 0; i < numTriangles * 3; i++) triVertices[i] = new_index[triVertices[i]];
    for (uint32_t i = 0; i < numVertices; i++) {
        const double* pt = revertices[i].c;
        coordinates[i * 3] = pt[0];
        coordinates[i * 3 + 1] = pt[1];
        coordinates[i * 3 + 2] = pt[2];
    }
}

struct input_vertex_t {
    double coord[3];
    uint32_t original_index;
};

inline bool misAlignment(const double* p, const double* q, const double* r)
{
    return orient2d(p[0], p[1], q[0], q[1], r[0], r[1]) ||
        orient2d(p[1], p[2], q[1], q[2], r[1], r[2]) ||
        orient2d(p[0], p[2], q[0], q[2], r[0], r[2]);
}

struct MeshData {
    std::vector<double> vertices;
    std::vector<uint32_t> triangle_indices;
};

inline MeshData read_OFF_file(const char* filename, bool verbose) {
    FILE* file = fopen(filename, "r");
    if (file == nullptr)
        ip_error("read_OFF_file: FATAL ERROR cannot open input file.\n");

    char file_ext_read[3];
    char file_ext_target[] = { 'O','F','F' };
    if (fscanf(file, "%3c", file_ext_read) == 0)
        ip_error("read_OFF_file: FATAL ERROR cannot read 1st line of input file\n");

    for (uint32_t i = 0; i < 3; i++)
        if (file_ext_read[i] != file_ext_target[i])
            ip_error("read_OFF_file: FATAL ERROR 1st line of input file is different from OFF\n");

    uint32_t npts, ntri;
    if (fscanf(file, " %d %d %*d ", &npts, &ntri) == 0)
        ip_error("read_OFF_file: FATAL ERROR 2st line of input file does not contain point and triangles numbers.\n");

    if (verbose) std::cout << "file " << filename << " contains " << npts << " vertices and " << ntri << " constraints (triangles)\n";

    MeshData data;
    data.vertices.resize(npts * 3);
    data.triangle_indices.resize(ntri * 3);

    for (uint32_t i = 0; i < npts; i++) {
        if (fscanf(file, " %lf %lf %lf ",
            &data.vertices[i * 3], &data.vertices[i * 3 + 1], &data.vertices[i * 3 + 2]) == 0)
            ip_error("error reading input file\n");
    }

    uint32_t nv;
    for (uint32_t i = 0; i < ntri; i++) {
        if (fscanf(file, " %u %u %u %u ", &nv,
            &data.triangle_indices[i * 3], &data.triangle_indices[i * 3 + 1], &data.triangle_indices[i * 3 + 2]) == 0)
            ip_error("error reading input file\n");
        if (nv != 3) ip_error("Non-triangular faces not supported\n");
    }
    fclose(file);
    return data;
}

inline MeshData read_assimp_file(const char* filename, bool verbose) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if (!scene) {
        ip_error("read_assimp_file: FATAL ERROR cannot load file");
    }

    if (scene->mNumMeshes == 0) {
        ip_error("read_assimp_file: FATAL ERROR no meshes in file");
    }

    uint32_t total_vertices = 0;
    uint32_t total_faces = 0;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];
        total_vertices += mesh->mNumVertices;
        total_faces += mesh->mNumFaces;
    }

    if (verbose) std::cout << "file " << filename << " contains " << total_vertices << " vertices and " << total_faces << " triangles\n";

    MeshData data;
    data.vertices.resize(total_vertices * 3);
    data.triangle_indices.resize(total_faces * 3);

    uint32_t vertex_offset = 0;
    uint32_t face_offset = 0;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D v = mesh->mVertices[i];
            data.vertices[(vertex_offset + i) * 3] = v.x;
            data.vertices[(vertex_offset + i) * 3 + 1] = v.y;
            data.vertices[(vertex_offset + i) * 3 + 2] = v.z;
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            if (face.mNumIndices != 3) {
                ip_error("read_assimp_file: FATAL ERROR non-triangular face found\n");
            }
            data.triangle_indices[(face_offset + i) * 3] = vertex_offset + face.mIndices[0];
            data.triangle_indices[(face_offset + i) * 3 + 1] = vertex_offset + face.mIndices[1];
            data.triangle_indices[(face_offset + i) * 3 + 2] = vertex_offset + face.mIndices[2];
        }

        vertex_offset += mesh->mNumVertices;
        face_offset += mesh->mNumFaces;
    }

    return data;
}

inline int vertex_compare(const void* void_v1, const void* void_v2)
{
    const input_vertex_t* v1 = (const input_vertex_t*)void_v1;
    const input_vertex_t* v2 = (const input_vertex_t*)void_v2;
    const double dx = v1->coord[0] - v2->coord[0];
    const double dy = v1->coord[1] - v2->coord[1];
    const double dz = v1->coord[2] - v2->coord[2];
    return (4 * ((dx > 0) - (dx < 0)) +
        2 * ((dy > 0) - (dy < 0)) +
        ((dz > 0) - (dz < 0)));
}

inline int triOrder(const void* t1, const void* t2) {
    const uint32_t* a = (const uint32_t*)t1;
    const uint32_t* b = (const uint32_t*)t2;

    if (a[0] < b[0]) return -1;
    if (a[0] > b[0]) return 1;
    if (a[1] < b[1]) return -1;
    if (a[1] > b[1]) return 1;
    if (a[2] < b[2]) return -1;
    return (a[2] > b[2]);
}

inline bool coincident_points(const input_vertex_t* a, const input_vertex_t* b)
{
    return !vertex_compare(a->coord, b->coord);
}

inline std::pair<std::vector<input_vertex_t>, std::vector<uint32_t>> read_nodes_and_constraints(
    const double* coords_A, uint32_t npts_A,
    const uint32_t* tri_idx_A, uint32_t ntri_A, bool verbose) {

    std::vector<input_vertex_t> tmp(npts_A);
    for (uint32_t i = 0; i < npts_A; i++) {
        tmp[i].coord[0] = coords_A[i * 3];
        tmp[i].coord[1] = coords_A[i * 3 + 1];
        tmp[i].coord[2] = coords_A[i * 3 + 2];
        tmp[i].original_index = i;
    }

    if (npts_A > 1) {
        std::sort(tmp.begin(), tmp.end(), [](const input_vertex_t& a, const input_vertex_t& b) {
            if (a.coord[0] != b.coord[0]) return a.coord[0] < b.coord[0];
            if (a.coord[1] != b.coord[1]) return a.coord[1] < b.coord[1];
            return a.coord[2] < b.coord[2];
        });
    }

    std::vector<uint32_t> diff(npts_A, 0);
    uint32_t vrts_counter = 0;
    for (uint32_t i = 1; i < npts_A; i++) {
        if (coincident_points(&tmp[i - 1], &tmp[i])) vrts_counter++;
        diff[i] = vrts_counter;
    }

    std::vector<uint32_t> map(npts_A);
    for (uint32_t i = 0; i < npts_A; i++) map[tmp[i].original_index] = i;

    std::vector<input_vertex_t> vertices;
    vertices.reserve(npts_A - vrts_counter);
    vertices.push_back(tmp[0]);
    for (uint32_t i = 1; i < npts_A; i++) {
        if (!coincident_points(&tmp[i - 1], &tmp[i])) {
            vertices.push_back(tmp[i]);
        }
    }
    uint32_t num_vertices = (uint32_t)vertices.size();

    if (verbose) std::cout << "Using " << num_vertices << " unique vertices\n";

    std::vector<uint32_t> tri_vertices;
    tri_vertices.reserve(ntri_A * 3);

    for (uint32_t j = 0; j < ntri_A; j++) {
        uint32_t i1 = tri_idx_A[j * 3];
        uint32_t i2 = tri_idx_A[j * 3 + 1];
        uint32_t i3 = tri_idx_A[j * 3 + 2];

        uint32_t idx1 = map[i1] - diff[map[i1]];
        uint32_t idx2 = map[i2] - diff[map[i2]];
        uint32_t idx3 = map[i3] - diff[map[i3]];

        const double* v1c = vertices[idx1].coord;
        const double* v2c = vertices[idx2].coord;
        const double* v3c = vertices[idx3].coord;

        if (!misAlignment(v1c, v2c, v3c)) {
            ip_error("Model has degenerate triangles. Unsupported!\n");
            continue;
        }
        tri_vertices.push_back(idx1);
        tri_vertices.push_back(idx2);
        tri_vertices.push_back(idx3);
    }

    qsort(tri_vertices.data(), tri_vertices.size() / 3, sizeof(uint32_t) * 3, triOrder);

    std::vector<uint32_t> unique_tri;
    unique_tri.reserve(tri_vertices.size());
    for (size_t i = 0; i < tri_vertices.size(); i += 3) {
        if (i + 3 >= tri_vertices.size()) break;
        if (triOrder(tri_vertices.data() + i, tri_vertices.data() + i + 3) != 0) {
            unique_tri.push_back(tri_vertices[i]);
            unique_tri.push_back(tri_vertices[i + 1]);
            unique_tri.push_back(tri_vertices[i + 2]);
        }
    }

    uint32_t num_valid_tris = (uint32_t)unique_tri.size() / 3;
    if (verbose) std::cout << "Using " << num_valid_tris << " non-degenerate constraints\n";

    return {std::move(vertices), std::move(unique_tri)};
}

class inputPLC {
public:
    std::vector<double> coordinates;
    std::vector<uint32_t> triangle_vertices;
    const char* input_file_name = "";

    uint32_t numVertices() const { return (uint32_t)coordinates.size() / 3; }
    uint32_t numTriangles() const { return (uint32_t)triangle_vertices.size() / 3; }

    inputPLC() = default;

    inputPLC(const char* filename) { initFromFile(filename, true); }

    bool initFromFile(const char* filename, bool verbose) {
        input_file_name = filename;

        MeshData data = read_OFF_file(filename, verbose);
        if (data.vertices.empty()) ip_error("Input file has no vertices\n");
        if (data.triangle_indices.empty()) ip_error("Input file has no triangles\n");

        postProcess(data.vertices.data(), (uint32_t)data.vertices.size() / 3,
                    data.triangle_indices.data(), (uint32_t)data.triangle_indices.size() / 3, verbose);

        return true;
    }

    bool initFromVectors(double* vertex_p, uint32_t npts, uint32_t* tri_vertices_p, uint32_t ntri, bool verbose) {
        input_file_name = "";
        postProcess(vertex_p, npts, tri_vertices_p, ntri, verbose);
        return true;
    }

    bool initFromAssimp(const char* filename, bool verbose) {
        input_file_name = filename;

        MeshData data = read_assimp_file(filename, verbose);
        if (data.vertices.empty()) ip_error("Input file has no vertices\n");
        if (data.triangle_indices.empty()) ip_error("Input file has no triangles\n");

        postProcess(data.vertices.data(), (uint32_t)data.vertices.size() / 3,
                    data.triangle_indices.data(), (uint32_t)data.triangle_indices.size() / 3, verbose);

        return true;
    }

    void postProcess(double* vertices_p, uint32_t npts, uint32_t* tri_vertices_p, uint32_t ntri, bool verbose) {
        auto [vertices, valid_tri_vertices] = read_nodes_and_constraints(
            vertices_p, npts, tri_vertices_p, ntri, verbose);

        coordinates.resize(vertices.size() * 3);
        for (size_t i = 0; i < vertices.size(); i++) {
            coordinates[i * 3] = vertices[i].coord[0];
            coordinates[i * 3 + 1] = vertices[i].coord[1];
            coordinates[i * 3 + 2] = vertices[i].coord[2];
        }

        triangle_vertices = std::move(valid_tri_vertices);

        reorderVertices(coordinates.data(), numVertices(), triangle_vertices.data(), numTriangles());
    }

    void addBoundingBoxVertices() {
        double bbmin[3] = { DBL_MAX, DBL_MAX, DBL_MAX };
        double bbmax[3] = { -DBL_MAX, -DBL_MAX, -DBL_MAX };
        for (uint32_t i = 0; i < numVertices(); i++) {
            const double* v = coordinates.data() + i * 3;
            for (int j = 0; j < 3; j++) {
                if (v[j] < bbmin[j]) bbmin[j] = v[j];
                if (v[j] > bbmax[j]) bbmax[j] = v[j];
            }
        }
        const double bbox[3] = { bbmax[0] - bbmin[0], bbmax[1] - bbmin[1], bbmax[2] - bbmin[2] };
        for (int j = 0; j < 3; j++) {
            bbmin[j] -= bbox[j] * 0.05;
            bbmax[j] += bbox[j] * 0.05;
        }

        const int idx[] = { 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1 };

        for (int j = 0; j < 24; j++)
            coordinates.push_back(idx[j] ? (bbmax[j%3]) : (bbmin[j%3]));
    }
};