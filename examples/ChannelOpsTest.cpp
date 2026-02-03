/**
 * Channel Operations Test Example
 * Demonstrates channel splitting, merging, and manipulation
 */

#include<hgl/2d/Bitmap.h>
#include<hgl/2d/ChannelSplit.h>
#include<hgl/2d/ChannelMerge.h>
#include<hgl/2d/ChannelOps.h>
#include<hgl/2d/BitmapSave.h>
#include<hgl/2d/TGA.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>
#include<iostream>
#include<hgl/utf.h>
#include<tuple>
#include<vector>

using namespace hgl;
using namespace hgl::bitmap;
namespace channel = hgl::bitmap::channel;

/**
 * Create a test image with color gradients
 */
BitmapRGBA8* CreateTestImageWithAlpha(int width, int height)
{
    auto* bitmap = new BitmapRGBA8();
    bitmap->Create(width, height);

    auto* data = bitmap->GetData();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Color4ub pixel;
            pixel.r = static_cast<uint8>(255 * x / width);        // Red gradient
            pixel.g = static_cast<uint8>(255 * y / height);       // Green gradient
            pixel.b = static_cast<uint8>(255 * (1 - x / (float)width) * (1 - y / (float)height)); // Blue gradient
            pixel.a = static_cast<uint8>(255 * x / width);        // Alpha gradient

            data[y * width + x] = pixel;
        }
    }

    return bitmap;
}

/**
 * Test channel splitting for RGBA
 */
void TestChannelSplitRGBA()
{
    std::cout << "Testing RGBA channel split..." << std::endl;

    auto* original = CreateTestImageWithAlpha(256, 256);

    auto split_rgba = channel::SplitRGBA(*original);
    auto* redChannel = std::get<0>(split_rgba);
    auto* greenChannel = std::get<1>(split_rgba);
    auto* blueChannel = std::get<2>(split_rgba);
    auto* alphaChannel = std::get<3>(split_rgba);

    SaveTga("channel_red.tga", redChannel);
    SaveTga("channel_green.tga", greenChannel);
    SaveTga("channel_blue.tga", blueChannel);
    SaveTga("channel_alpha.tga", alphaChannel);

    std::cout << "  Split RGBA into R, G, B, A channels" << std::endl;

    delete redChannel;
    delete greenChannel;
    delete blueChannel;
    delete alphaChannel;
    delete original;
}

/**
 * Test channel merging
 */
void TestChannelMerge()
{
    std::cout << "Testing channel merge..." << std::endl;

    // Create test channels
    BitmapGrey8 redChan;
    BitmapGrey8 greenChan;
    BitmapGrey8 blueChan;
    BitmapGrey8 alphaChan;

    redChan.Create(256, 256);
    greenChan.Create(256, 256);
    blueChan.Create(256, 256);
    alphaChan.Create(256, 256);

    // Fill red channel with horizontal gradient
    {
        auto* data = redChan.GetData();
        for (int y = 0; y < 256; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                data[y * 256 + x] = static_cast<uint8>(255 * x / 256);
            }
        }
    }

    // Fill green channel with vertical gradient
    {
        auto* data = greenChan.GetData();
        for (int y = 0; y < 256; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                data[y * 256 + x] = static_cast<uint8>(255 * y / 256);
            }
        }
    }

    // Fill blue channel with diagonal gradient
    {
        auto* data = blueChan.GetData();
        for (int y = 0; y < 256; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                data[y * 256 + x] = static_cast<uint8>(255 * (x + y) / 512);
            }
        }
    }

    // Fill alpha channel with full opacity
    {
        auto* data = alphaChan.GetData();
        for (int i = 0; i < 256 * 256; i++)
        {
            data[i] = 255;
        }
    }

    // Merge channels
    BitmapRGBA8* mergedImage = channel::MergeRGBA(redChan, greenChan, blueChan, alphaChan);

    SaveTga("channel_merged_rgba.tga", mergedImage);
    std::cout << "  Merged 4 channels into RGBA image" << std::endl;

    delete mergedImage;
}

/**
 * Test channel swapping
 */
void TestChannelSwap()
{
    std::cout << "Testing channel swap operations..." << std::endl;

    auto* original = CreateTestImageWithAlpha(256, 256);

    // Get channels
    auto split_rgba = channel::SplitRGBA(*original);
    auto* redChan = std::get<0>(split_rgba);
    auto* greenChan = std::get<1>(split_rgba);
    auto* blueChan = std::get<2>(split_rgba);
    auto* alphaChan = std::get<3>(split_rgba);

    // Swap R and G channels
    BitmapRGBA8* swappedRG = channel::MergeRGBA(*greenChan, *redChan, *blueChan, *alphaChan);
    SaveTga("channel_swap_rg.tga", swappedRG);
    std::cout << "  Swapped R and G channels" << std::endl;

    // Swap R and B channels
    BitmapRGBA8* swappedRB = channel::MergeRGBA(*blueChan, *greenChan, *redChan, *alphaChan);
    SaveTga("channel_swap_rb.tga", swappedRB);
    std::cout << "  Swapped R and B channels" << std::endl;

    // Grayscale: all channels to red (or any channel)
    BitmapRGBA8* grayscaleImage = channel::MergeRGBA(*redChan, *redChan, *redChan, *alphaChan);
    SaveTga("channel_grayscale_from_red.tga", grayscaleImage);
    std::cout << "  Created grayscale from red channel" << std::endl;

    delete swappedRG;
    delete swappedRB;
    delete grayscaleImage;
    delete redChan;
    delete greenChan;
    delete blueChan;
    delete alphaChan;
    delete original;
}

/**
 * Test channel manipulation (invert, scale, etc)
 */
void TestChannelManipulation()
{
    std::cout << "Testing channel manipulation..." << std::endl;

    auto* original = CreateTestImageWithAlpha(256, 256);

    // Get channels
    auto split_rgba = channel::SplitRGBA(*original);
    auto* redChan = std::get<0>(split_rgba);
    auto* greenChan = std::get<1>(split_rgba);
    auto* blueChan = std::get<2>(split_rgba);
    auto* alphaChan = std::get<3>(split_rgba);

    // Invert red channel
    {
        auto* data = redChan->GetData();
        for (int i = 0; i < 256 * 256; i++)
        {
            data[i] = 255 - data[i];
        }
    }

    BitmapRGBA8* invertedRed = channel::MergeRGBA(*redChan, *greenChan, *blueChan, *alphaChan);
    SaveTga("channel_inverted_red.tga", invertedRed);
    std::cout << "  Inverted red channel" << std::endl;

    // Scale blue channel (make it darker)
    {
        auto* data = blueChan->GetData();
        for (int i = 0; i < 256 * 256; i++)
        {
            data[i] = static_cast<uint8>(data[i] * 0.5f);
        }
    }

    BitmapRGBA8* scaledBlue = channel::MergeRGBA(*redChan, *greenChan, *blueChan, *alphaChan);
    SaveTga("channel_scaled_blue.tga", scaledBlue);
    std::cout << "  Scaled blue channel to 50%" << std::endl;

    delete invertedRed;
    delete scaledBlue;
    delete redChan;
    delete greenChan;
    delete blueChan;
    delete alphaChan;
    delete original;
}

/**
 * Test RGB channel operations
 */
void TestRGBChannels()
{
    std::cout << "Testing RGB channel split..." << std::endl;

    BitmapRGB8 rgbBitmap;
    rgbBitmap.Create(256, 256);

    auto* data = rgbBitmap.GetData();

    // Create gradient
    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            Color3ub pixel;
            pixel.r = static_cast<uint8>(255 * x / 256);
            pixel.g = static_cast<uint8>(255 * y / 256);
            pixel.b = static_cast<uint8>(255 * (1 - x / (float)256));

            data[y * 256 + x] = pixel;
        }
    }

    // Split RGB
    auto split_rgb = channel::SplitRGB(rgbBitmap);
    auto* r = std::get<0>(split_rgb);
    auto* g = std::get<1>(split_rgb);
    auto* b = std::get<2>(split_rgb);

    SaveTga("channel_rgb_red.tga", r);
    SaveTga("channel_rgb_green.tga", g);
    SaveTga("channel_rgb_blue.tga", b);

    std::cout << "  Split RGB into separate channels" << std::endl;

    // Merge back
    BitmapRGB8* mergedRGB = channel::MergeRGB(*r, *g, *b);
    SaveTga("channel_rgb_merged.tga", mergedRGB);
    std::cout << "  Merged RGB channels back" << std::endl;

    delete r;
    delete g;
    delete b;
    delete mergedRGB;
}

/**
 * Test channel averaging (create composite)
 */
void TestChannelComposite()
{
    std::cout << "Testing channel composite operations..." << std::endl;

    auto* original = CreateTestImageWithAlpha(256, 256);

    // Get channels
    auto split_rgba = channel::SplitRGBA(*original);
    auto* redChan = std::get<0>(split_rgba);
    auto* greenChan = std::get<1>(split_rgba);
    auto* blueChan = std::get<2>(split_rgba);
    auto* alphaChan = std::get<3>(split_rgba);

    // Create luminance (weighted average of RGB)
    BitmapGrey8 luminance;
    luminance.Create(256, 256);

    auto* redData = redChan->GetData();
    auto* greenData = greenChan->GetData();
    auto* blueData = blueChan->GetData();
    auto* lumData = luminance.GetData();

    for (int i = 0; i < 256 * 256; i++)
    {
        // Standard luminance formula: 0.299*R + 0.587*G + 0.114*B
        uint8 lum = static_cast<uint8>(
            0.299f * redData[i] +
            0.587f * greenData[i] +
            0.114f * blueData[i]
        );
        lumData[i] = lum;
    }

    SaveTga("channel_luminance.tga", &luminance);
    std::cout << "  Created luminance from RGB channels" << std::endl;

    delete redChan;
    delete greenChan;
    delete blueChan;
    delete alphaChan;
    delete original;
}

int main(int argc, char** argv)
{
    std::cout << "=== CM2D Channel Operations Test ===" << std::endl;
    std::cout << std::endl;

    TestChannelSplitRGBA();
    std::cout << std::endl;

    TestChannelMerge();
    std::cout << std::endl;

    TestChannelSwap();
    std::cout << std::endl;

    TestChannelManipulation();
    std::cout << std::endl;

    TestRGBChannels();
    std::cout << std::endl;

    TestChannelComposite();
    std::cout << std::endl;

    std::cout << "All tests completed!" << std::endl;

    return 0;
}
