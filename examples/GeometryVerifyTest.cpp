/**
 * DrawGeometry Verification Test
 * First real coverage of the DrawGeometry primitives:
 * - DrawBar region-exactness (was writing past the bar's right edge)
 * - DrawWireCircle double-blend regression (was drawing 8 points twice)
 * - DrawSolidCircle fill bounds
 * - DrawMonoBitmap edge cases (w==width was never drawn)
 * - DrawHLine/DrawVLine/DrawLine clipping
 * - DrawArc / DrawQuadraticBezier smoke
 */

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/DrawGeometry.h>
#include<hgl/2d/Blend.h>
#include<hgl/color/Color3ub.h>
#include<iostream>

using namespace hgl;
using namespace hgl::bitmap;

static int failures = 0;

#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { std::cerr << "FAIL: " << msg << std::endl; ++failures; } \
        else { std::cout << "  ok: " << msg << std::endl; } \
    } while (0)

static Color3ub Pixel(const BitmapRGB8& bmp, int x, int y)
{
    return *bmp.GetData(x, y);
}

int main()
{
    const Color3ub bg(255, 255, 255);
    const Color3ub fg(255, 0, 0);

    BitmapRGB8 bmp;
    bmp.Create(64, 64);

    DrawGeometryRGB8 dg(&bmp);
    dg.SetDrawColor(fg);

    // ==================== DrawBar region exactness ====================
    std::cout << "DrawBar..." << std::endl;
    bmp.ClearColor(bg);
    dg.SetAlpha(1.0f);
    dg.DrawBar(10, 5, 20, 10);      // region [10..29] x [5..14]

    CHECK(Pixel(bmp, 10, 5) == fg, "bar: top-left corner");
    CHECK(Pixel(bmp, 29, 14) == fg, "bar: bottom-right corner");
    CHECK(Pixel(bmp, 20, 10) == fg, "bar: center");
    CHECK(Pixel(bmp, 30, 5) != fg, "bar: right edge +1 not painted (was overrunning)");
    CHECK(Pixel(bmp, 9, 5) == bg, "bar: left edge -1 not painted");
    CHECK(Pixel(bmp, 10, 15) == bg, "bar: bottom edge +1 not painted");
    CHECK(Pixel(bmp, 10, 4) == bg, "bar: top edge -1 not painted");

    // off-origin bar (l>0) previously wrote into the next row
    bmp.ClearColor(bg);
    dg.DrawBar(30, 30, 8, 8);       // region [30..37] x [30..37]
    CHECK(Pixel(bmp, 37, 37) == fg, "bar2: bottom-right corner");
    CHECK(Pixel(bmp, 38, 30) == bg, "bar2: right edge +1 not painted");
    CHECK(Pixel(bmp, 30, 38) == bg, "bar2: bottom edge +1 not painted");

    // ==================== DrawWireCircle ====================
    std::cout << "DrawWireCircle..." << std::endl;
    bmp.ClearColor(bg);
    dg.SetAlpha(0.5f);
    dg.DrawWireCircle(32, 32, 16);

    // Compute expected colors via the same blend machinery
    BlendColor<Color3ub> bl;                        // Normal mode
    const Color3ub once  = bl(bg, fg, 0.5f);        // blended once
    const Color3ub twice = bl(once, fg, 0.5f);      // blended twice

    CHECK(once != twice, "wire circle: once/twice colors differ");

    int twice_count = 0, other_count = 0, ring_count = 0;
    for (int y = 0; y < 64; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            const Color3ub c = Pixel(bmp, x, y);
            if (c == once)
                ++ring_count;
            else if (c == twice)
                ++twice_count;
            else if (c != bg)
                ++other_count;
        }
    }

    // x=0 first iteration draws the 4 axis points twice (inherent symmetry);
    // the trailing 8-PutPixel block (removed) used to double-blend the last
    // octant points too, so twice_count was 12 before the fix.
    CHECK(ring_count > 0, "wire circle: ring pixels present");
    CHECK(twice_count == 4, "wire circle: exactly 4 axis points double-blended (was 12 before fix)");
    CHECK(other_count == 0, "wire circle: no unexpected blend colors");

    // ==================== DrawSolidCircle ====================
    std::cout << "DrawSolidCircle..." << std::endl;
    bmp.ClearColor(bg);
    dg.SetAlpha(1.0f);
    dg.DrawSolidCircle(32, 32, 8);

    CHECK(Pixel(bmp, 32, 32) == fg, "solid circle: center");
    CHECK(Pixel(bmp, 32, 40) == fg, "solid circle: dy==r boundary");
    CHECK(Pixel(bmp, 24, 32) == fg, "solid circle: dx==-r boundary");
    CHECK(Pixel(bmp, 32, 41) == bg, "solid circle: outside dy==r+1");
    CHECK(Pixel(bmp, 23, 32) == bg, "solid circle: outside dx==r+1");

    // ==================== DrawMonoBitmap ====================
    std::cout << "DrawMonoBitmap..." << std::endl;
    bmp.ClearColor(bg);
    uint8 mono[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    dg.DrawMonoBitmap(10, 10, mono, 8, 8);
    CHECK(Pixel(bmp, 10, 10) == fg, "mono: top-left set");
    CHECK(Pixel(bmp, 17, 17) == fg, "mono: bottom-right set");
    CHECK(Pixel(bmp, 18, 10) == bg, "mono: right of bitmap clear");
    CHECK(Pixel(bmp, 10, 18) == bg, "mono: below bitmap clear");

    // w == bitmap width: was never drawn (early-return bug)
    bmp.ClearColor(bg);
    uint8 mono_row[8] = { 0xFF, 0, 0, 0, 0, 0, 0, 0 };   // 64px wide, first 8 px set
    dg.DrawMonoBitmap(0, 0, mono_row, 64, 1);
    CHECK(Pixel(bmp, 0, 0) == fg, "mono: w==width now draws (was early-return)");
    CHECK(Pixel(bmp, 7, 0) == fg, "mono: w==width 8th pixel set");
    CHECK(Pixel(bmp, 8, 0) == bg, "mono: w==width 9th pixel clear");

    // out-of-bounds placement: silent no-op, no crash
    bmp.ClearColor(bg);
    dg.DrawMonoBitmap(60, 0, mono, 8, 8);   // 60+8 > 64
    CHECK(Pixel(bmp, 60, 0) == bg, "mono: out-of-bounds placement is a no-op");

    // ==================== DrawHLine / DrawVLine clipping ====================
    std::cout << "DrawHLine/DrawVLine..." << std::endl;
    bmp.ClearColor(bg);
    dg.DrawHLine(-5, 30, 10);       // clamps to x in [0..4]
    CHECK(Pixel(bmp, 0, 30) == fg, "hline: negative start clamped to 0");
    CHECK(Pixel(bmp, 4, 30) == fg, "hline: last clamped pixel");
    CHECK(Pixel(bmp, 5, 30) == bg, "hline: beyond length clear");

    bmp.ClearColor(bg);
    dg.DrawVLine(30, -5, 10);       // clamps to y in [0..4]
    CHECK(Pixel(bmp, 30, 0) == fg, "vline: negative start clamped to 0");
    CHECK(Pixel(bmp, 30, 4) == fg, "vline: last clamped pixel");
    CHECK(Pixel(bmp, 30, 5) == bg, "vline: beyond length clear");

    // ==================== DrawLine ====================
    std::cout << "DrawLine..." << std::endl;
    bmp.ClearColor(bg);
    dg.DrawLine(0, 0, 63, 63);
    CHECK(Pixel(bmp, 32, 32) == fg, "line: diagonal midpoint");
    CHECK(Pixel(bmp, 63, 63) == fg, "line: endpoint");

    // ==================== DrawArc / Bezier smoke ====================
    std::cout << "DrawArc/Bezier..." << std::endl;
    bmp.ClearColor(bg);
    dg.DrawArc(32, 32, 16, 0.0f, 1.5708f);      // first quadrant
    CHECK(Pixel(bmp, 48, 32) == fg, "arc: start point at angle 0");

    bmp.ClearColor(bg);
    dg.DrawQuadraticBezier(math::Vector2i(0, 0), math::Vector2i(31, 0), math::Vector2i(63, 32));
    CHECK(Pixel(bmp, 63, 32) == fg, "bezier: endpoint");

    if (failures == 0)
    {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    }

    std::cerr << failures << " test(s) failed" << std::endl;
    return 1;
}
