/**
 * Draw Geometry Test Example
 * Demonstrates drawing primitives and geometric shapes
 */

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/DrawGeometry.h>
#include<hgl/2d/BitmapSave.h>
#include<hgl/2d/TGA.h>
#include<hgl/utf.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>
#include<iostream>
#include<cmath>
#include<algorithm>
#include<vector>

using namespace hgl;
using namespace hgl::bitmap;

/**
 * Draw a line using Bresenham algorithm
 */
void DrawLine(BitmapRGB8& bitmap, int x0, int y0, int x1, int y1, const Color3ub& color)
{
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int x = x0;
    int y = y0;

    auto* data = bitmap.GetData();
    int width = bitmap.GetWidth();
    int height = bitmap.GetHeight();

    while (true)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            data[y * width + x] = color;
        }

        if (x == x1 && y == y1)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

/**
 * Draw a filled circle
 */
void DrawFilledCircle(BitmapRGB8& bitmap, int cx, int cy, int radius, const Color3ub& color)
{
    auto* data = bitmap.GetData();
    int width = bitmap.GetWidth();
    int height = bitmap.GetHeight();

    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            if (x * x + y * y <= radius * radius)
            {
                int px = cx + x;
                int py = cy + y;

                if (px >= 0 && px < width && py >= 0 && py < height)
                {
                    data[py * width + px] = color;
                }
            }
        }
    }
}

/**
 * Draw a hollow circle (just outline)
 */
void DrawCircleOutline(BitmapRGB8& bitmap, int cx, int cy, int radius, const Color3ub& color)
{
    auto* data = bitmap.GetData();
    int width = bitmap.GetWidth();
    int height = bitmap.GetHeight();

    // Midpoint circle algorithm
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    auto drawCirclePoints = [&](int px, int py)
    {
        int points[][2] = {
            {cx + px, cy + py}, {cx - px, cy + py},
            {cx + px, cy - py}, {cx - px, cy - py},
            {cx + py, cy + px}, {cx - py, cy + px},
            {cx + py, cy - px}, {cx - py, cy - px}
        };

        for (auto& p : points)
        {
            if (p[0] >= 0 && p[0] < width && p[1] >= 0 && p[1] < height)
            {
                data[p[1] * width + p[0]] = color;
            }
        }
    };

    while (x <= y)
    {
        drawCirclePoints(x, y);

        if (d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

/**
 * Draw a filled rectangle
 */
void DrawFilledRect(BitmapRGB8& bitmap, int x1, int y1, int x2, int y2, const Color3ub& color)
{
    int minX = std::min(x1, x2);
    int maxX = std::max(x1, x2);
    int minY = std::min(y1, y2);
    int maxY = std::max(y1, y2);

    auto* data = bitmap.GetData();
    int width = bitmap.GetWidth();
    int height = bitmap.GetHeight();

    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            if (x >= 0 && x < width && y >= 0 && y < height)
            {
                data[y * width + x] = color;
            }
        }
    }
}

/**
 * Test basic drawing primitives
 */
void TestDrawingPrimitives()
{
    std::cout << "Testing drawing primitives..." << std::endl;

    BitmapRGB8 primitivesBitmap;
    primitivesBitmap.Create(512, 512);

    auto* data = primitivesBitmap.GetData();

    // Clear with white background
    Color3ub white(255, 255, 255);
    for (int i = 0; i < 512 * 512; i++)
    {
        data[i] = white;
    }

    // Draw colored lines forming a star pattern
    Color3ub red(255, 0, 0);
    Color3ub green(0, 255, 0);
    Color3ub blue(0, 0, 255);

    int cx = 256;
    int cy = 256;
    int len = 150;

    // Draw 8-pointed star
    for (int i = 0; i < 8; i++)
    {
        float angle = (i / 8.0f) * 2 * 3.14159f;
        int x = cx + (int)(len * std::cos(angle));
        int y = cy + (int)(len * std::sin(angle));

        Color3ub color = (i % 3 == 0) ? red : (i % 3 == 1) ? green : blue;
        DrawLine(primitivesBitmap, cx, cy, x, y, color);
    }

    SaveTga("draw_lines_star.tga", &primitivesBitmap);
    std::cout << "  Drew star pattern with lines" << std::endl;
}

/**
 * Test circle drawing
 */
void TestCircleDrawing()
{
    std::cout << "Testing circle drawing..." << std::endl;

    BitmapRGB8 circleBitmap;
    circleBitmap.Create(512, 512);

    auto* data = circleBitmap.GetData();

    // Clear with light gray background
    Color3ub bgColor(200, 200, 200);
    for (int i = 0; i < 512 * 512; i++)
    {
        data[i] = bgColor;
    }

    // Draw multiple circles
    Color3ub red(255, 0, 0);
    Color3ub green(0, 255, 0);
    Color3ub blue(0, 0, 255);
    Color3ub yellow(255, 255, 0);

    // Center point
    int cx = 256;
    int cy = 256;

    // Draw filled circles with different radii
    DrawFilledCircle(circleBitmap, cx, cy, 100, red);
    DrawFilledCircle(circleBitmap, cx, cy, 80, green);
    DrawFilledCircle(circleBitmap, cx, cy, 60, blue);
    DrawFilledCircle(circleBitmap, cx, cy, 40, yellow);

    SaveTga("draw_circles_filled.tga", &circleBitmap);
    std::cout << "  Drew filled circles" << std::endl;

    // Create circle outline test
    BitmapRGB8 circleOutlineBitmap;
    circleOutlineBitmap.Create(512, 512);
    data = circleOutlineBitmap.GetData();

    // Clear
    Color3ub white(255, 255, 255);
    for (int i = 0; i < 512 * 512; i++)
    {
        data[i] = white;
    }

    // Draw circle outlines
    DrawCircleOutline(circleOutlineBitmap, cx, cy, 150, red);
    DrawCircleOutline(circleOutlineBitmap, cx, cy, 120, green);
    DrawCircleOutline(circleOutlineBitmap, cx, cy, 90, blue);
    DrawCircleOutline(circleOutlineBitmap, cx, cy, 60, yellow);

    SaveTga("draw_circles_outline.tga", &circleOutlineBitmap);
    std::cout << "  Drew circle outlines" << std::endl;
}

/**
 * Test rectangle drawing
 */
void TestRectangleDrawing()
{
    std::cout << "Testing rectangle drawing..." << std::endl;

    BitmapRGB8 rectBitmap;
    rectBitmap.Create(512, 512);

    auto* data = rectBitmap.GetData();

    // Clear with white background
    Color3ub white(255, 255, 255);
    for (int i = 0; i < 512 * 512; i++)
    {
        data[i] = white;
    }

    // Draw grid of colored rectangles
    int gridSize = 4;
    int cellWidth = 512 / gridSize;
    int cellHeight = 512 / gridSize;

    Color3ub colors[] = {
        {255, 0, 0},     // Red
        {0, 255, 0},     // Green
        {0, 0, 255},     // Blue
        {255, 255, 0},   // Yellow
        {255, 0, 255},   // Magenta
        {0, 255, 255},   // Cyan
        {255, 128, 0},   // Orange
        {128, 0, 255},   // Purple
        {255, 192, 203}, // Pink
        {128, 128, 0},   // Olive
        {0, 128, 128},   // Teal
        {192, 192, 192}, // Silver
        {128, 0, 0},     // Maroon
        {0, 128, 0},     // Dark Green
        {0, 0, 128},     // Navy
        {128, 128, 128}  // Gray
    };

    int colorIdx = 0;
    for (int row = 0; row < gridSize; row++)
    {
        for (int col = 0; col < gridSize; col++)
        {
            int x1 = col * cellWidth;
            int y1 = row * cellHeight;
            int x2 = x1 + cellWidth - 1;
            int y2 = y1 + cellHeight - 1;

            DrawFilledRect(rectBitmap, x1, y1, x2, y2, colors[colorIdx % 16]);
            colorIdx++;
        }
    }

    SaveTga("draw_rectangles_grid.tga", &rectBitmap);
    std::cout << "  Drew rectangle grid" << std::endl;
}

/**
 * Test complex pattern
 */
void TestComplexPattern()
{
    std::cout << "Testing complex geometric pattern..." << std::endl;

    BitmapRGB8 complexBitmap;
    complexBitmap.Create(512, 512);

    auto* data = complexBitmap.GetData();

    // Spiral pattern
    Color3ub bgColor(20, 20, 40);
    for (int i = 0; i < 512 * 512; i++)
    {
        data[i] = bgColor;
    }

    int cx = 256;
    int cy = 256;

    // Draw rotating lines (sun-like pattern)
    Color3ub lineColor(255, 200, 0);
    for (int i = 0; i < 36; i++)
    {
        float angle = (i / 36.0f) * 2 * 3.14159f;
        int x = cx + (int)(200 * std::cos(angle));
        int y = cy + (int)(200 * std::sin(angle));
        DrawLine(complexBitmap, cx, cy, x, y, lineColor);
    }

    // Draw concentric circles
    Color3ub circleColor(100, 200, 255);
    for (int r = 50; r < 200; r += 30)
    {
        DrawCircleOutline(complexBitmap, cx, cy, r, circleColor);
    }

    SaveTga("draw_complex_pattern.tga", &complexBitmap);
    std::cout << "  Drew complex pattern" << std::endl;
}

int main(int argc, char** argv)
{
    std::cout << "=== CM2D Draw Geometry Test ===" << std::endl;
    std::cout << std::endl;

    TestDrawingPrimitives();
    TestCircleDrawing();
    TestRectangleDrawing();
    TestComplexPattern();

    std::cout << std::endl;
    std::cout << "All tests completed!" << std::endl;

    return 0;
}
