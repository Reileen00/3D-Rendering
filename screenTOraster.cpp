#include <iostream>
#include <cmath>

struct Vec2f {
    float x, y;
};

struct Vec2i {
    int x, y;
};

struct Vec3f {
    float x, y, z;
    Vec3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

int main() {
    int width = 64;
    int height = 64;

    Vec3f P = Vec3f(-1, 2, 10);

    Vec2f P_proj;
    P_proj.x = P.x / P.z;
    P_proj.y = P.y / P.z;

    Vec2f P_proj_nor;
    P_proj_nor.x = (P_proj.x + 1) / 2;
    P_proj_nor.y = (1 - P_proj.y) / 2;

    Vec2i P_proj_raster;
    P_proj_raster.x = static_cast<int>(P_proj_nor.x * width);
    P_proj_raster.y = static_cast<int>(P_proj_nor.y * height);

    if (P_proj_raster.x == width)  P_proj_raster.x = width - 1;
    if (P_proj_raster.y == height) P_proj_raster.y = height - 1;

    std::cout << "Projected raster coordinates: (" << P_proj_raster.x << ", " << P_proj_raster.y << ")" << std::endl;

    return 0;
}
