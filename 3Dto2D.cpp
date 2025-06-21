#include <iostream>
#include <cmath>
#include <algorithm>

// ------------- Vector Classes -------------
struct Vec3f {
    float x, y, z;
    Vec3f(float x = 0, float y = 0, float z = 0): x(x), y(y), z(z) {}
};

struct Vec2f {
    float x, y;
    Vec2f(float x = 0, float y = 0): x(x), y(y) {}
};

struct Vec2i {
    int x, y;
    Vec2i(int x = 0, int y = 0): x(x), y(y) {}
};

// ------------- Matrix Class (4x4) -------------
class Matrix44f {
public:
    float m[4][4] = { 0 };

    Matrix44f() {
        for (int i = 0; i < 4; ++i)
            m[i][i] = 1;
    }

    // Multiply 3D vector as point (assumes w=1)
    void multVecMatrix(const Vec3f &src, Vec3f &dst) const {
        float x = src.x, y = src.y, z = src.z;
        dst.x = x * m[0][0] + y * m[0][1] + z * m[0][2] + m[0][3];
        dst.y = x * m[1][0] + y * m[1][1] + z * m[1][2] + m[1][3];
        dst.z = x * m[2][0] + y * m[2][1] + z * m[2][2] + m[2][3];
        float w = x * m[3][0] + y * m[3][1] + z * m[3][2] + m[3][3];
        if (w != 1 && w != 0) {
            dst.x /= w;
            dst.y /= w;
            dst.z /= w;
        }
    }

    // Compute the inverse of a transformation matrix (assuming rotation + translation only)
    Matrix44f inverse() const {
        Matrix44f inv;
        // Transpose the rotation part
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                inv.m[i][j] = m[j][i];

        // Invert the translation
        inv.m[0][3] = - (m[0][3] * inv.m[0][0] + m[1][3] * inv.m[0][1] + m[2][3] * inv.m[0][2]);
        inv.m[1][3] = - (m[0][3] * inv.m[1][0] + m[1][3] * inv.m[1][1] + m[2][3] * inv.m[1][2]);
        inv.m[2][3] = - (m[0][3] * inv.m[2][0] + m[1][3] * inv.m[2][1] + m[2][3] * inv.m[2][2]);

        inv.m[3][0] = inv.m[3][1] = inv.m[3][2] = 0;
        inv.m[3][3] = 1;

        return inv;
    }
};

// ------------- Projection Logic -------------
bool computePixelCoordinates(
    const Vec3f &pWorld, 
    const Matrix44f &worldToCamera,
    const float &canvasWidth,
    const float &canvasHeight,
    const int &imageWidth,
    const int &imageHeight,
    Vec2i &pRaster)
{
    Vec3f pCamera;
    worldToCamera.multVecMatrix(pWorld, pCamera);

    if (pCamera.z >= 0)
        return false;

    Vec2f pScreen;
    pScreen.x = pCamera.x / -pCamera.z;
    pScreen.y = pCamera.y / -pCamera.z;

    if (std::abs(pScreen.x) > canvasWidth || std::abs(pScreen.y) > canvasHeight)
        return false;

    Vec2f pNDC;
    pNDC.x = (pScreen.x + canvasWidth / 2.f) / canvasWidth;
    pNDC.y = (pScreen.y + canvasHeight / 2.f) / canvasHeight;

    pRaster.x = static_cast<int>(std::floor(pNDC.x * imageWidth));
    pRaster.y = static_cast<int>(std::floor((1.f - pNDC.y) * imageHeight));

    pRaster.x = std::clamp(pRaster.x, 0, imageWidth - 1);
    pRaster.y = std::clamp(pRaster.y, 0, imageHeight - 1);

    return true;
}

// ------------- Main Program -------------
int main()
{
    // Create a camera-to-world matrix (identity = camera at origin looking -Z)
    Matrix44f cameraToWorld;

    // Example: translate the camera 2 units forward in Z
    cameraToWorld.m[2][3] = 2.0f; // Move camera back (so it sees negative Z)

    Matrix44f worldToCamera = cameraToWorld.inverse();

    Vec3f pWorld(1.0f, 0.5f, -2.0f);  // a 3D point in front of the camera

    float canvasWidth = 2.0f, canvasHeight = 2.0f;
    int imageWidth = 512, imageHeight = 512;

    Vec2i pRaster;

    if (computePixelCoordinates(pWorld, worldToCamera, canvasWidth, canvasHeight, imageWidth, imageHeight, pRaster)) {
        std::cerr << "Pixel coordinates: (" << pRaster.x << ", " << pRaster.y << ")\n";
    } else {
        std::cerr << "Point (" << pWorld.x << ", " << pWorld.y << ", " << pWorld.z << ") is not visible.\n";
    }

    return 0;
}
