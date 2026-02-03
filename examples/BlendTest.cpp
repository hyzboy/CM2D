/**
 * Blend Color Test Example
 * Demonstrates the CM2D blending system with various blend modes
 */

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/Blend.h>
#include<hgl/2d/BitmapSave.h>
#include<hgl/2d/TGA.h>
#include<hgl/color/Color4ub.h>
#include<hgl/math/AlphaBlend.h>
#include<iostream>
#include<hgl/utf.h>

using namespace hgl;
using namespace hgl::bitmap;

/**
 * Create a gradient test image showing different blend modes
 */
void CreateBlendModeTestImage()
{
    std::cout << "Creating blend mode test image..." << std::endl;

    // Create RGBA8 bitmap (512x512)
    BitmapRGBA8 blendTestBitmap;
    blendTestBitmap.Create(512, 512);

    auto* data = blendTestBitmap.GetData();

    // Define test colors
    Color4ub srcColor(255, 0, 0, 200);      // Red with alpha
    Color4ub dstColor(0, 0, 255, 200);      // Blue with alpha
    Color4ub bgColor(128, 128, 128, 255);   // Gray background

    // Blend modes to test
    AlphaBlendMode modes[] = {
        AlphaBlendMode::Normal,
        AlphaBlendMode::Add,
        AlphaBlendMode::Subtract,
        AlphaBlendMode::Multiply,
        AlphaBlendMode::Screen,
        AlphaBlendMode::Overlay,
    };

    int modeCount = sizeof(modes) / sizeof(modes[0]);
    int cols = 3;
    int rows = (modeCount + cols - 1) / cols;
    int cellWidth = 512 / cols;
    int cellHeight = 512 / rows;

    // Fill background
    for (int i = 0; i < 512 * 512; i++)
    {
        data[i] = bgColor;
    }

    // Draw blended cells
    BlendColor<Color4ub> blender;

    for (int modeIdx = 0; modeIdx < modeCount; modeIdx++)
    {
        blender.SetBlendMode(modes[modeIdx]);

        int row = modeIdx / cols;
        int col = modeIdx % cols;
        int startX = col * cellWidth;
        int startY = row * cellHeight;

        for (int y = 0; y < cellHeight; y++)
        {
            for (int x = 0; x < cellWidth; x++)
            {
                int px = startX + x;
                int py = startY + y;

                if (px >= 0 && px < 512 && py >= 0 && py < 512)
                {
                    // Create gradient effect
                    float alpha = (float)x / cellWidth;
                    Color4ub result = blender(srcColor, dstColor, alpha);
                    data[py * 512 + px] = result;
                }
            }
        }
    }

    // Save as TGA
    bool success = SaveTga("blend_modes_test.tga", &blendTestBitmap);
    std::cout << "Blend modes test image saved: " << (success ? "✓ Success" : "✗ Failed") << std::endl;
}

/**
 * Create an alpha transparency test image
 */
void CreateAlphaTransparencyTest()
{
    std::cout << "Creating alpha transparency test..." << std::endl;

    BitmapRGBA8 alphaBitmap;
    alphaBitmap.Create(256, 256);

    auto* data = alphaBitmap.GetData();

    // Create a checkerboard background pattern
    Color4ub whitePixel(255, 255, 255, 255);
    Color4ub darkPixel(200, 200, 200, 255);

    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            Color4ub bgColor = ((x / 16) + (y / 16)) % 2 == 0 ? whitePixel : darkPixel;
            data[y * 256 + x] = bgColor;
        }
    }

    // Draw semi-transparent rectangles with different alphas
    Color4ub rectColor(255, 128, 0, 128);  // Orange with alpha=128

    BlendColor<Color4ub> blender;
    blender.SetBlendMode(AlphaBlendMode::Normal);

    // Top-left quarter: alpha=64
    for (int y = 0; y < 128; y++)
    {
        for (int x = 0; x < 128; x++)
        {
            Color4ub result = blender(rectColor, data[y * 256 + x], 0.25f);
            data[y * 256 + x] = result;
        }
    }

    // Top-right quarter: alpha=128
    for (int y = 0; y < 128; y++)
    {
        for (int x = 128; x < 256; x++)
        {
            Color4ub result = blender(rectColor, data[y * 256 + x], 0.5f);
            data[y * 256 + x] = result;
        }
    }

    // Bottom-left quarter: alpha=192
    for (int y = 128; y < 256; y++)
    {
        for (int x = 0; x < 128; x++)
        {
            Color4ub result = blender(rectColor, data[y * 256 + x], 0.75f);
            data[y * 256 + x] = result;
        }
    }

    // Bottom-right quarter: alpha=255 (fully opaque)
    for (int y = 128; y < 256; y++)
    {
        for (int x = 128; x < 256; x++)
        {
            Color4ub result = blender(rectColor, data[y * 256 + x], 1.0f);
            data[y * 256 + x] = result;
        }
    }

    bool success = SaveTga("alpha_transparency_test.tga", &alphaBitmap);
    std::cout << "Alpha transparency test saved: " << (success ? "✓ Success" : "✗ Failed") << std::endl;
}

/**
 * Create a color blend gradient
 */
void CreateColorBlendGradient()
{
    std::cout << "Creating color blend gradient..." << std::endl;

    BitmapRGB8 gradientBitmap;
    gradientBitmap.Create(256, 256);

    auto* data = gradientBitmap.GetData();

    // Top to bottom: blend from red to blue
    // Left to right: blend from red to green

    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            Color3ub pixel;
            pixel.r = static_cast<uint8>(255 * (1.0f - x / 256.0f));           // Red decreases left to right
            pixel.g = static_cast<uint8>(255 * (x / 256.0f));                  // Green increases left to right
            pixel.b = static_cast<uint8>(255 * (y / 256.0f));                  // Blue increases top to bottom

            data[y * 256 + x] = pixel;
        }
    }

    bool success = SaveTga("color_gradient_test.tga", &gradientBitmap);
    std::cout << "Color blend gradient saved: " << (success ? "✓ Success" : "✗ Failed") << std::endl;
}

int main(int argc, char** argv)
{
    std::cout << "=== CM2D Blend System Test ===" << std::endl;
    std::cout << std::endl;

    CreateBlendModeTestImage();
    CreateAlphaTransparencyTest();
    CreateColorBlendGradient();

    std::cout << std::endl;
    std::cout << "All tests completed!" << std::endl;

    return 0;
}
