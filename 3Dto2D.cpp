#include <iostream>
#include <cmath>
#include <algorithm>

// Assume Vec3f, Vec2f, Vec2i, and Matrix44f are defined as part of your math library.
// Dummy definitions below to compile and illustrate:
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

// Dummy matrix class with required methods
class Matrix44f {
public:
    // Dummy method for illustration; you must provide actual math
    Matrix44f inverse() const {
        // Return inverse of the matrix
        Matrix44f inv;
        // ... compute actual inverse here
        return inv;
    }

    void multVecMatrix(const Vec3f &src, Vec3f &dst) const {
        // Multiply src by this matrix and store result in dst
        // ... actual transformation logic
        dst = src;  // placeholder for illustration
    }
};

// Optimized function (without in-function inverse)
bool computePixelCoordinates(
    const Vec3f &pWorld, 
    const Matrix44f &worldToCamera,  // precomputed inverse
    const float &canvasWidth,
    const float &canvasHeight,
    const int &imageWidth,
    const int &imageHeight,
    Vec2i &pRaster)
{
    // Step 1: Transform point to camera space
    Vec3f pCamera;
    worldToCamera.multVecMatrix(pWorld, pCamera);

    // Step 2: Cull points behind the camera
    if (pCamera.z >= 0)
        return false;

    // Step 3: Perspective projection to screen space
    Vec2f pScreen;
    pScreen.x = pCamera.x / -pCamera.z;
    pScreen.y = pCamera.y / -pCamera.z;

    // Step 4: Check if point is within the canvas
    if (std::abs(pScreen.x) > canvasWidth || std::abs(pScreen.y) > canvasHeight)
        return false;

    // Step 5: Normalize screen space to NDC (0 to 1)
    Vec2f pNDC;
    pNDC.x = (pScreen.x + canvasWidth / 2.f) / canvasWidth;
    pNDC.y = (pScreen.y + canvasHeight / 2.f) / canvasHeight;

    // Step 6: Convert NDC to raster space (pixel coords)
    pRaster.x = static_cast<int>(std::floor(pNDC.x * imageWidth));
    pRaster.y = static_cast<int>(std::floor((1.f - pNDC.y) * imageHeight));

    // Step 7: Clamp to image bounds
    pRaster.x = std::clamp(pRaster.x, 0, imageWidth - 1);
    pRaster.y = std::clamp(pRaster.y, 0, imageHeight - 1);

    return true;
}

int main()
{
    // Camera-to-world matrix (you should initialize this properly)
    Matrix44f cameraToWorld;

    // Compute inverse once
    Matrix44f worldToCamera = cameraToWorld.inverse();

    // World-space point to project
    Vec3f pWorld(1.0f, 0.5f, -2.0f);  // in front of the camera

    // Canvas/view plane dimensions (typically ±1 or field-of-view derived)
    float canvasWidth = 2.0f;
    float canvasHeight = 2.0f;

    // Output image dimensions
    int imageWidth = 512;
    int imageHeight = 512;

    // Output raster coordinates
    Vec2i pRaster;

    // Try projecting the world point to image space
    if (computePixelCoordinates(pWorld, worldToCamera, canvasWidth, canvasHeight, imageWidth, imageHeight, pRaster)) {
        std::cerr << "Pixel coordinates: (" << pRaster.x << ", " << pRaster.y << ")\n";
    } else {
        std::cerr << "Point (" << pWorld.x << ", " << pWorld.y << ", " << pWorld.z << ") is not visible.\n";
    }

    return 0;
}
