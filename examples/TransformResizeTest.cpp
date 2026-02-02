/**
 * Transform and Resize Test Example
 * Demonstrates rotation, flipping, and scaling operations
 */

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/Transform.h>
#include<hgl/2d/Resize.h>
#include<hgl/2d/TGA.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>
#include<iostream>
#include<cmath>
#include<vector>

using namespace hgl;
using namespace hgl::bitmap;
using namespace hgl::bitmap::transform;
using namespace hgl::bitmap::resize;

/**
 * Create a test pattern with gradient and geometric shapes
 */
BitmapRGB8* CreateTestPattern(int width, int height)
{
    auto* bitmap = new BitmapRGB8();
    bitmap->Create(width, height);

    auto* data = bitmap->GetData();

    // Create gradient background
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Color3ub pixel;
            pixel.r = static_cast<uint8>(255 * x / width);
            pixel.g = static_cast<uint8>(255 * y / height);
            pixel.b = 128;

            data[y * width + x] = pixel;
        }
    }

    // Draw a white circle in the center
    float cx = width / 2.0f;
    float cy = height / 2.0f;
    float radius = std::min(width, height) / 4.0f;

    Color3ub whiteColor(255, 255, 255);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float dx = x - cx;
            float dy = y - cy;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist < radius)
            {
                data[y * width + x] = whiteColor;
            }
        }
    }

    // Draw corner markers
    Color3ub redColor(255, 0, 0);
    Color3ub greenColor(0, 255, 0);

    // Top-left red square
    for (int y = 0; y < 20; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            data[y * width + x] = redColor;
        }
    }

    // Bottom-right green square
    for (int y = height - 20; y < height; y++)
    {
        for (int x = width - 20; x < width; x++)
        {
            data[y * width + x] = greenColor;
        }
    }

    return bitmap;
}

/**
 * Test rotation functionality
 */
void TestRotation()
{
    std::cout << "Testing rotation..." << std::endl;

    auto* original = CreateTestPattern(256, 256);

    // Test 90 degree clockwise rotation
    auto rotated90 = transform::Rotate(*original, RotateAngle::Rotate90CW);
    SaveTga("transform_rotate90cw.tga", &rotated90);
    std::cout << "  Rotated 90° CW: " << rotated90.GetWidth() << "x" << rotated90.GetHeight() << std::endl;

    // Test 180 degree rotation
    auto rotated180 = transform::Rotate(*original, RotateAngle::Rotate180);
    SaveTga("transform_rotate180.tga", &rotated180);
    std::cout << "  Rotated 180°: " << rotated180.GetWidth() << "x" << rotated180.GetHeight() << std::endl;

    // Test 90 degree counter-clockwise rotation
    auto rotated270 = transform::Rotate(*original, RotateAngle::Rotate90CCW);
    SaveTga("transform_rotate90ccw.tga", &rotated270);
    std::cout << "  Rotated 90° CCW: " << rotated270.GetWidth() << "x" << rotated270.GetHeight() << std::endl;

    delete original;
}

/**
 * Test flip/mirror functionality
 */
void TestFlip()
{
    std::cout << "Testing flip/mirror operations..." << std::endl;

    auto* original = CreateTestPattern(256, 192);

    // Test horizontal flip
    auto flippedH = transform::Flip(*original, FlipDirection::Horizontal);
    SaveTga("transform_flip_horizontal.tga", &flippedH);
    std::cout << "  Flipped horizontally: " << flippedH.GetWidth() << "x" << flippedH.GetHeight() << std::endl;

    // Test vertical flip
    auto flippedV = transform::Flip(*original, FlipDirection::Vertical);
    SaveTga("transform_flip_vertical.tga", &flippedV);
    std::cout << "  Flipped vertically: " << flippedV.GetWidth() << "x" << flippedV.GetHeight() << std::endl;

    // Test both flip
    auto flippedBoth = transform::Flip(*original, FlipDirection::Both);
    SaveTga("transform_flip_both.tga", &flippedBoth);
    std::cout << "  Flipped both: " << flippedBoth.GetWidth() << "x" << flippedBoth.GetHeight() << std::endl;

    delete original;
}

/**
 * Test resize with different filters
 */
void TestResize()
{
    std::cout << "Testing resize operations..." << std::endl;

    auto* original = CreateTestPattern(512, 512);

    // Test nearest neighbor (fast, pixelated)
    auto nearest = Resize(*original, 256, 256, FilterType::NearestNeighbor);
    SaveTga("resize_nearest_neighbor.tga", &nearest);
    std::cout << "  Nearest neighbor resize: " << nearest.GetWidth() << "x" << nearest.GetHeight() << std::endl;

    // Test bilinear (balanced quality/speed)
    auto bilinear = Resize(*original, 256, 256, FilterType::Bilinear);
    SaveTga("resize_bilinear.tga", &bilinear);
    std::cout << "  Bilinear resize: " << bilinear.GetWidth() << "x" << bilinear.GetHeight() << std::endl;

    // Test bicubic (high quality)
    auto bicubic = Resize(*original, 256, 256, FilterType::Bicubic);
    SaveTga("resize_bicubic.tga", &bicubic);
    std::cout << "  Bicubic resize: " << bicubic.GetWidth() << "x" << bicubic.GetHeight() << std::endl;

    // Test Lanczos3 (highest quality)
    auto lanczos = Resize(*original, 256, 256, FilterType::Lanczos3);
    SaveTga("resize_lanczos3.tga", &lanczos);
    std::cout << "  Lanczos3 resize: " << lanczos.GetWidth() << "x" << lanczos.GetHeight() << std::endl;

    // Test upscaling
    auto upscaled = Resize(*original, 1024, 1024, FilterType::Bilinear);
    SaveTga("resize_upscale.tga", &upscaled);
    std::cout << "  Upscaled 2x: " << upscaled.GetWidth() << "x" << upscaled.GetHeight() << std::endl;

    delete original;
}

/**
 * Test combined transformations
 */
void TestCombinedTransforms()
{
    std::cout << "Testing combined transformations..." << std::endl;

    auto* original = CreateTestPattern(256, 256);

    // Rotate then resize
    auto rotated = transform::Rotate(*original, RotateAngle::Rotate90CW);
    auto rotResized = Resize(rotated, 128, 128, FilterType::Bilinear);
    SaveTga("transform_combined_rotate_resize.tga", &rotResized);
    std::cout << "  Rotate + Resize: " << rotResized.GetWidth() << "x" << rotResized.GetHeight() << std::endl;

    // Flip then rotate
    auto flipped = transform::Flip(*original, FlipDirection::Horizontal);
    auto flipRotated = transform::Rotate(flipped, RotateAngle::Rotate180);
    SaveTga("transform_combined_flip_rotate.tga", &flipRotated);
    std::cout << "  Flip + Rotate: " << flipRotated.GetWidth() << "x" << flipRotated.GetHeight() << std::endl;

    delete original;
}

int main(int argc, char** argv)
{
    std::cout << "=== CM2D Transform and Resize Test ===" << std::endl;
    std::cout << std::endl;

    TestRotation();
    std::cout << std::endl;

    TestFlip();
    std::cout << std::endl;

    TestResize();
    std::cout << std::endl;

    TestCombinedTransforms();
    std::cout << std::endl;

    std::cout << "All tests completed!" << std::endl;

    return 0;
}
