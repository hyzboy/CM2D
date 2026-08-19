/**
 * Tile / MipMap Verification Test
 * These APIs were completely broken before the Bitmap Rule-of-Five fix:
 * push_back(std::move(tile)) degraded to a shallow copy, so every tile
 * pointer was dangling after the loop-local tile was destroyed.
 * (SplitIntoTiles aborted in ALL build configurations.)
 *
 * Covers: SplitIntoTiles / AssembleTiles roundtrip, partial-tile skip,
 * MakeSeamless edge equality, MipMapChain level generation.
 */

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/Tile.h>
#include<hgl/2d/MipMap.h>
#include<hgl/color/Color3ub.h>
#include<iostream>

using namespace hgl;
using namespace hgl::bitmap;
using namespace hgl::bitmap::mipmap;

static int failures = 0;

#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { std::cerr << "FAIL: " << msg << std::endl; ++failures; } \
        else { std::cout << "  ok: " << msg << std::endl; } \
    } while (0)

static void FillPattern(BitmapRGB8& bmp)
{
    for (int y = 0; y < bmp.GetHeight(); ++y)
    {
        for (int x = 0; x < bmp.GetWidth(); ++x)
        {
            *bmp.GetData(x, y) = Color3ub((x * 3) & 0xFF, (y * 5) & 0xFF, ((x + y) * 7) & 0xFF);
        }
    }
}

static bool BitmapEqual(const BitmapRGB8& a, const BitmapRGB8& b)
{
    if (a.GetWidth() != b.GetWidth() || a.GetHeight() != b.GetHeight())
        return false;

    for (int y = 0; y < a.GetHeight(); ++y)
    {
        for (int x = 0; x < a.GetWidth(); ++x)
        {
            if (*a.GetData(x, y) != *b.GetData(x, y))
                return false;
        }
    }
    return true;
}

int main()
{
    // ==================== SplitIntoTiles ====================
    std::cout << "SplitIntoTiles..." << std::endl;

    BitmapRGB8 src;
    src.Create(64, 64);
    FillPattern(src);

    auto tiles = tile::SplitIntoTiles(src, 16, 16);
    CHECK(tiles.GetTileCount() == 16, "split: 64x64 into 16x16 tiles -> 16 tiles");
    CHECK(tiles.GetTile(0).GetWidth() == 16 && tiles.GetTile(0).GetHeight() == 16, "split: tile size");
    CHECK(tiles.GetTile(15).GetWidth() == 16, "split: last tile exists");
    CHECK(tiles.GetTile(0).GetData() != nullptr, "split: tile data valid (no dangling pointer)");
    CHECK(*tiles.GetTile(0).GetData(0, 0) == *src.GetData(0, 0), "split: tile0 pixel matches source");

    // ==================== AssembleTiles roundtrip ====================
    std::cout << "AssembleTiles roundtrip..." << std::endl;

    auto assembled = tile::AssembleTiles(tiles, 64, 64);
    CHECK(assembled.GetWidth() == 64 && assembled.GetHeight() == 64, "assemble: size restored");
    CHECK(BitmapEqual(assembled, src), "assemble: roundtrip equals source");

    // ==================== partial tiles skip ====================
    std::cout << "Partial tiles..." << std::endl;

    BitmapRGB8 src60;
    src60.Create(60, 60);
    FillPattern(src60);

    auto tiles_full = tile::SplitIntoTiles(src60, 16, 16, false);
    CHECK(tiles_full.GetTileCount() == 9, "partial: allow_partial=false -> 3x3 full tiles only");
    CHECK(tiles_full.GetTile(8).GetWidth() == 16 && tiles_full.GetTile(8).GetHeight() == 16, "partial: all tiles full size");

    auto tiles_partial = tile::SplitIntoTiles(src60, 16, 16, true);
    CHECK(tiles_partial.GetTileCount() == 16, "partial: allow_partial=true -> 4x4 tiles");
    CHECK(tiles_partial.GetTile(15).GetWidth() == 12 && tiles_partial.GetTile(15).GetHeight() == 12, "partial: corner tile is 12x12");

    auto assembled60 = tile::AssembleTiles(tiles_partial, 60, 60);
    CHECK(BitmapEqual(assembled60, src60), "partial: roundtrip with partial tiles equals source");

    // ==================== MakeSeamless ====================
    std::cout << "MakeSeamless..." << std::endl;

    auto seamless = tile::MakeSeamless(src, 8);
    CHECK(seamless.GetWidth() == 64 && seamless.GetHeight() == 64, "seamless: size unchanged");

    bool lr_ok = true, tb_ok = true;
    for (int y = 0; y < 64; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (*seamless.GetData(x, y) != *seamless.GetData(64 - 8 + x, y))
                lr_ok = false;
            if (*seamless.GetData(y, x) != *seamless.GetData(y, 64 - 8 + x))
                tb_ok = false;
        }
    }
    CHECK(lr_ok, "seamless: left band equals right band");
    CHECK(tb_ok, "seamless: top band equals bottom band");

    // ==================== MipMapChain ====================
    std::cout << "MipMapChain..." << std::endl;

    MipMapChain<Color3ub, 3> chain;
    chain.Generate(src);                    // 64x64 -> 1x1
    CHECK(chain.GetLevelCount() == 7, "mipmap: 64->1 is 7 levels");
    CHECK(chain.GetLevel(0).GetWidth() == 64, "mipmap: level0 width");
    CHECK(chain.GetLevel(1).GetWidth() == 32 && chain.GetLevel(1).GetHeight() == 32, "mipmap: level1 is 32x32");
    CHECK(chain.GetLevel(6).GetWidth() == 1 && chain.GetLevel(6).GetHeight() == 1, "mipmap: level6 is 1x1");
    CHECK(BitmapEqual(chain.GetLevel(0), src), "mipmap: level0 equals source");

    auto chain2 = mipmap::GenerateMipMaps(src, mipmap::MipMapConfig{ resize::FilterType::Bilinear, 3, 16 });
    CHECK(chain2.GetLevelCount() == 3, "mipmap: max_levels=3 honored");
    CHECK(chain2.GetLevel(2).GetWidth() == 16, "mipmap: min_size=16 honored");

    if (failures == 0)
    {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    }

    std::cerr << failures << " test(s) failed" << std::endl;
    return 1;
}
