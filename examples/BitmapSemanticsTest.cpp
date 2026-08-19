/**
 * Bitmap Value-Semantics Test
 * Verifies copy/move constructors and assignments of Bitmap<T,C>
 * (deep copy independence, moved-from state, self-assignment)
 */

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/Transform.h>
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

int main()
{
    std::cout << "=== Bitmap Value-Semantics Test ===" << std::endl;

    // --- copy constructor: deep copy ---
    std::cout << "Copy constructor..." << std::endl;
    BitmapRGB8 a;
    a.Create(8, 8);
    a.GetData()[3] = Color3ub(1, 2, 3);

    BitmapRGB8 b(a);
    CHECK(b.GetData() != nullptr, "copy ctor: data allocated");
    CHECK(b.GetData() != a.GetData(), "copy ctor: independent buffer");
    CHECK(b.GetWidth() == 8 && b.GetHeight() == 8, "copy ctor: size copied");
    CHECK(b.GetData()[3] == Color3ub(1, 2, 3), "copy ctor: pixel copied");
    b.GetData()[3] = Color3ub(9, 9, 9);
    CHECK(a.GetData()[3] == Color3ub(1, 2, 3), "copy ctor: modifying copy leaves source intact");

    // --- copy assignment ---
    std::cout << "Copy assignment..." << std::endl;
    BitmapRGB8 c;
    c = b;
    CHECK(c.GetData() != nullptr && c.GetData() != b.GetData(), "copy assign: independent buffer");
    CHECK(c.GetData()[3] == Color3ub(9, 9, 9), "copy assign: pixel copied");
    c.GetData()[3] = Color3ub(5, 5, 5);
    CHECK(b.GetData()[3] == Color3ub(9, 9, 9), "copy assign: source unaffected");

    // --- copy assignment onto existing buffer (realloc path) ---
    c.Create(4, 4);             // shrink, forces reallocation
    c = a;                      // copy over existing data
    CHECK(c.GetWidth() == 8 && c.GetHeight() == 8, "copy assign over existing buffer: size updated");
    CHECK(c.GetData()[3] == Color3ub(1, 2, 3), "copy assign over existing buffer: pixel copied");

    // --- move constructor ---
    std::cout << "Move constructor..." << std::endl;
    Color3ub* orig_ptr = c.GetData();
    BitmapRGB8 d(std::move(c));
    CHECK(d.GetData() == orig_ptr, "move ctor: buffer stolen");
    CHECK(c.GetData() == nullptr, "move ctor: source data nulled");
    CHECK(c.GetWidth() == 0 && c.GetHeight() == 0, "move ctor: source size zeroed");
    CHECK(d.GetData()[3] == Color3ub(1, 2, 3), "move ctor: pixels intact");

    // --- move assignment ---
    std::cout << "Move assignment..." << std::endl;
    BitmapRGB8 e;
    e = std::move(d);
    CHECK(e.GetData() == orig_ptr, "move assign: buffer stolen");
    CHECK(d.GetData() == nullptr, "move assign: source data nulled");
    CHECK(e.GetData()[3] == Color3ub(1, 2, 3), "move assign: pixels intact");

    // --- self-assignment ---
    std::cout << "Self-assignment..." << std::endl;
    e = e;
    CHECK(e.GetData() != nullptr && e.GetWidth() == 8, "self assign: no crash, data intact");

    // --- moved-from object is reusable via Create() ---
    std::cout << "Moved-from reuse..." << std::endl;
    c.Create(2, 2);
    CHECK(c.GetData() != nullptr && c.GetWidth() == 2, "moved-from Create: works");

    // --- copy of empty bitmap ---
    std::cout << "Empty bitmap copy..." << std::endl;
    BitmapRGB8 empty1;
    BitmapRGB8 empty2(empty1);
    CHECK(empty2.GetData() == nullptr && empty2.GetWidth() == 0, "copy of empty: stays empty");

    // --- by-value return regression (double-free check) ---
    std::cout << "By-value return regression (Flip/Rotate)..." << std::endl;
    {
        BitmapRGB8 src2;
        src2.Create(16, 16);
        auto f = transform::Flip(src2, transform::FlipDirection::Horizontal);
        CHECK(f.GetData() != nullptr && f.GetWidth() == 16, "Flip by-value return: usable");
        auto r = transform::Rotate(src2, transform::RotateAngle::Rotate90CW);
        CHECK(r.GetData() != nullptr && r.GetWidth() == 16 && r.GetHeight() == 16, "Rotate by-value return: usable");
    } // destructors here must not double-free
    std::cout << "  by-value returns destroyed cleanly" << std::endl;

    if (failures == 0)
    {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    }

    std::cerr << failures << " test(s) failed" << std::endl;
    return 1;
}
