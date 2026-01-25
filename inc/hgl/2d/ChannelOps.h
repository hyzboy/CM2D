#pragma once

/**
 * Channel Operations Module
 *
 * Provides comprehensive functionality for splitting and merging bitmap channels.
 * This header includes both ChannelSplit.h and ChannelMerge.h for convenience.
 *
 * Split operations (ChannelSplit.h):
 * - SplitRGBA(): Split RGBA into 4 separate channels (R, G, B, A)
 * - SplitRGB(): Split RGB into 3 separate channels (R, G, B)
 * - SplitRG(): Split RG into 2 separate channels (R, G)
 * - SplitRGBA_To_RGB_A(): Split RGBA into RGB + A
 * - ExtractChannel(): Extract single channel by index
 * - ExtractR/G/B/A(): Convenient wrappers for extracting specific channels
 *
 * Merge operations (ChannelMerge.h):
 * - MergeRGBA(): Merge 4 channels into RGBA
 * - MergeRGB(): Merge 3 channels into RGB
 * - MergeRG(): Merge 2 channels into RG
 * - MergeRGB_A_To_RGBA(): Merge RGB bitmap + Alpha channel into RGBA
 * - MergeRG_B_To_RGB(): Merge RG bitmap + B channel into RGB
 *
 * Example usage:
 * ```cpp
 * #include <hgl/2d/ChannelOps.h>
 *
 * // Split RGBA into individual channels
 * BitmapRGBA8 source;
 * source.Create(100, 100);
 * auto [r, g, b, a] = hgl::bitmap::channel::SplitRGBA(source);
 *
 * // Process individual channels...
 *
 * // Merge back into RGBA
 * auto result = hgl::bitmap::channel::MergeRGBA(*r, *g, *b, *a);
 *
 * // Clean up
 * delete r; delete g; delete b; delete a;
 * delete result;
 *
 * // Or split into RGB + A for processing
 * auto [rgb, alpha] = hgl::bitmap::channel::SplitRGBA_To_RGB_A(source);
 * // ... process ...
 * auto merged = hgl::bitmap::channel::MergeRGB_A_To_RGBA(*rgb, *alpha);
 * delete rgb; delete alpha; delete merged;
 * ```
 */

#include<hgl/2d/ChannelSplit.h>
#include<hgl/2d/ChannelMerge.h>
