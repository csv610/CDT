#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <string>
#include <queue>

struct Point2D {
    float x, y;
};

struct Polygon {
    std::vector<Point2D> vertices;
};

// 8x8 Minimal font for A-Z and a-z
const uint64_t font_8x8[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0-15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16-31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 32-47 (space, etc.)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48-63 (numbers)
    0, // 64 (@)
    0x182442427E424242, // A
    0x7C22223C2222227C, // B
    0x3C4240404040423C, // C
    0x7824222222222478, // D
    0x7E4040784040407E, // E
    0x7E40407840404040, // F
    0x3C4240404E42423C, // G
    0x4242427E42424242, // H
    0x3E0808080808083E, // I
    0x1E04040404044438, // J
    0x4448506050484444, // K
    0x404040404040407E, // L
    0x42665A4242424242, // M
    0x4262524A46424242, // N
    0x3C4242424242423C, // O
    0x7C22223C40404040, // P
    0x3C42424242524A3C, // Q
    0x7C22223C48444242, // R
    0x3C42403C0202423C, // S
    0x7E18181818181818, // T
    0x424242424242423C, // U
    0x4242424242241810, // V
    0x424242425A664242, // W
    0x4242241818244242, // X
    0x4242241818181818, // Y
    0x7E0204081020407E, // Z
    0, 0, 0, 0, 0, 0,   // 91-96
    0x00003C023E46463B, // a
    0x40407C424242427C, // b
    0x00003C424040423C, // c
    0x02023E424242423E, // d
    0x00003C427E40423C, // e
    0x0E103C1010101010, // f
    0x00003E42423E023E, // g
    0x40407C4242424242, // h
    0x0800080808080808, // i
    0x0400040404044438, // j
    0x4044485060504844, // k
    0x101010101010100E, // l
    0x0000665A42424242, // m
    0x00007C4242424242, // n
    0x00003C424242423C, // o
    0x00007C42427C4040, // p
    0x00003E42423E0202, // q
    0x00005C6240404040, // r
    0x00003E403C023E00, // s
    0x10103C101010100E, // t
    0x000042424242423E, // u
    0x0000424242422418, // v
    0x000042425A664242, // w
    0x0000422418182442, // x
    0x00004242423E023C, // y
    0x00007E040810207E  // z
};

void draw_character(uint8_t* image, int W, int H, char c, int posX, int posY, int scale) {
    if (c < 0 || c >= 128) return;
    uint64_t bits = font_8x8[(int)c];
    if (bits == 0) return;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if ((bits >> ((7 - y) * 8 + (7 - x))) & 1) {
                for (int sy = 0; sy < scale; ++sy) {
                    for (int sx = 0; sx < scale; ++sx) {
                        int nx = posX + x * scale + sx;
                        int ny = posY + y * scale + sy;
                        if (nx >= 0 && nx < W && ny >= 0 && ny < H) {
                            image[ny * W + nx] = 255;
                        }
                    }
                }
            }
        }
    }
}

std::vector<Point2D> trace_boundary(const uint8_t* image, int width, int height, int startX, int startY, std::vector<bool>& visited_global) {
    std::vector<Point2D> boundary;
    const int dx[] = {-1, 0, 1, 1, 1, 0, -1, -1};
    const int dy[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int curX = startX, curY = startY;
    int backDir = 6; 
    do {
        boundary.push_back({(float)curX, (float)curY});
        visited_global[curY * width + curX] = true;
        bool found = false;
        for (int i = 0; i < 8; ++i) {
            int dir = (backDir + i) % 8;
            int nx = curX + dx[dir], ny = curY + dy[dir];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && image[ny * width + nx] > 127) {
                curX = nx, curY = ny;
                backDir = (dir + 5) % 8;
                found = true;
                break;
            }
        }
        if (!found) break;
    } while (curX != startX || curY != startY);
    return boundary;
}

void simplify_polygon(Polygon& poly) {
    if (poly.vertices.size() < 3) return;
    std::vector<Point2D> simplified;
    simplified.push_back(poly.vertices[0]);
    for (size_t i = 1; i < poly.vertices.size(); ++i) {
        Point2D p1 = simplified.back(), p2 = poly.vertices[i];
        Point2D p3 = (i + 1 < poly.vertices.size()) ? poly.vertices[i + 1] : poly.vertices[0];
        float cross = (p2.x - p1.x) * (p3.y - p2.y) - (p2.y - p1.y) * (p3.x - p2.x);
        if (std::abs(cross) > 1e-3) simplified.push_back(p2);
    }
    poly.vertices = simplified;
}

std::vector<Polygon> extract_alphabet_polygons(const uint8_t* image, int width, int height) {
    std::vector<Polygon> polygons;
    std::vector<bool> visited(width * height, false);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = y * width + x;
            if (image[idx] > 127 && !visited[idx]) {
                Polygon poly;
                poly.vertices = trace_boundary(image, width, height, x, y, visited);
                if (poly.vertices.size() >= 3) {
                    simplify_polygon(poly);
                    if (poly.vertices.size() >= 3) polygons.push_back(poly);
                }
                std::queue<std::pair<int, int>> q;
                q.push({x, y});
                while (!q.empty()) {
                    auto [cx, cy] = q.front(); q.pop();
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            int nx = cx + dx, ny = cy + dy;
                            if (nx >= 0 && nx < width && ny >= 0 && ny < height && image[ny * width + nx] > 127 && !visited[ny * width + nx]) {
                                visited[ny * width + nx] = true;
                                q.push({nx, ny});
                            }
                        }
                    }
                }
            }
        }
    }
    return polygons;
}

void save_to_off(const std::vector<Polygon>& polygons, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    size_t total_v = 0, total_e = 0;
    for (const auto& poly : polygons) {
        total_v += poly.vertices.size();
        total_e += poly.vertices.size();
    }
    file << "OFF\n" << total_v << " " << total_e << " 0\n";
    for (const auto& poly : polygons) {
        for (const auto& v : poly.vertices) file << v.x << " " << v.y << " 0\n";
    }
    int offset = 0;
    for (const auto& poly : polygons) {
        size_t n = poly.vertices.size();
        for (size_t i = 0; i < n; ++i) {
            file << "2 " << offset + i << " " << offset + (i + 1) % n << "\n";
        }
        offset += n;
    }
    std::cout << "Saved " << total_v << " vertices and " << total_e << " edges to " << filename << std::endl;
}

int main(int argc, char** argv) {
    const int W = 512, H = 512;
    std::vector<uint8_t> image(W * H, 0);
    std::string text = "Abc";
    if (argc > 1) text = argv[1];
    
    int scale = 30;
    int curX = 10;
    int curY = 100;
    for (char c : text) {
        if (curX + scale * 8 > W) {
            curX = 10;
            curY += scale * 10;
        }
        draw_character(image.data(), W, H, c, curX, curY, scale);
        curX += scale * 9;
    }

    auto polygons = extract_alphabet_polygons(image.data(), W, H);
    std::cout << "Extracted " << polygons.size() << " polygon components for text: " << text << std::endl;
    save_to_off(polygons, "alphabet.off");
    return 0;
}
