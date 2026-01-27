#include<hgl/2d/NoiseMap.h>
#include<cmath>
#include<algorithm>

namespace hgl::bitmap
{
    // Gradient vectors for simplex noise
    const SimplexNoise::Grad SimplexNoise::gradients[8] = {
        {1.0f, 1.0f}, {-1.0f, 1.0f}, {1.0f, -1.0f}, {-1.0f, -1.0f},
        {1.0f, 0.0f}, {-1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, -1.0f}
    };

    SimplexNoise::SimplexNoise(uint32 seed) : NoiseGenerator(seed)
    {
        // Initialize permutation table (same as Perlin)
        permutation.resize(512);

        std::vector<int> p(256);
        for (int i = 0; i < 256; i++)
            p[i] = i;

        // Shuffle using seed
        uint32 s = seed;
        for (int i = 255; i > 0; i--)
        {
            s = s * 1103515245 + 12345;
            int j = (s / 65536) % (i + 1);
            std::swap(p[i], p[j]);
        }

        // Duplicate for overflow handling
        for (int i = 0; i < 256; i++)
        {
            permutation[i] = p[i];
            permutation[256 + i] = p[i];
        }
    }

    int SimplexNoise::FastFloor(float x) const
    {
        return x > 0 ? static_cast<int>(x) : static_cast<int>(x) - 1;
    }

    float SimplexNoise::Dot(const Grad& g, float x, float y) const
    {
        return g.x * x + g.y * y;
    }

    float SimplexNoise::Generate(float x, float y) const
    {
        // Skewing factors for 2D simplex grid (precomputed for performance)
        const float F2 = 0.366025403f; // 0.5 * (sqrt(3) - 1)
        const float G2 = 0.211324865f; // (3 - sqrt(3)) / 6

        // Skew input space to determine which simplex cell we're in
        float s = (x + y) * F2;
        int i = FastFloor(x + s);
        int j = FastFloor(y + s);

        float t = (i + j) * G2;
        float X0 = i - t;
        float Y0 = j - t;
        float x0 = x - X0;
        float y0 = y - Y0;

        // Determine which simplex we're in
        int i1, j1;
        if (x0 > y0) { i1 = 1; j1 = 0; }
        else { i1 = 0; j1 = 1; }

        // Offsets for corners
        float x1 = x0 - i1 + G2;
        float y1 = y0 - j1 + G2;
        float x2 = x0 - 1.0f + 2.0f * G2;
        float y2 = y0 - 1.0f + 2.0f * G2;

        // Work out hashed gradient indices
        int ii = i & 255;
        int jj = j & 255;
        int gi0 = permutation[ii + permutation[jj]] % 8;
        int gi1 = permutation[ii + i1 + permutation[jj + j1]] % 8;
        int gi2 = permutation[ii + 1 + permutation[jj + 1]] % 8;

        // Calculate contribution from three corners
        float n0 = 0.0f, n1 = 0.0f, n2 = 0.0f;

        float t0 = 0.5f - x0 * x0 - y0 * y0;
        if (t0 >= 0.0f) {
            t0 *= t0;
            n0 = t0 * t0 * Dot(gradients[gi0], x0, y0);
        }

        float t1 = 0.5f - x1 * x1 - y1 * y1;
        if (t1 >= 0.0f) {
            t1 *= t1;
            n1 = t1 * t1 * Dot(gradients[gi1], x1, y1);
        }

        float t2 = 0.5f - x2 * x2 - y2 * y2;
        if (t2 >= 0.0f) {
            t2 *= t2;
            n2 = t2 * t2 * Dot(gradients[gi2], x2, y2);
        }

        // Add contributions and scale to [-1, 1]
        // Scaling factor for simplex noise normalization
        // This value was empirically determined to map simplex noise contributions
        // (which sum up from 3 gradient contributions) to approximate [-1, 1] range
        const float SIMPLEX_SCALE = 70.0f;
        return SIMPLEX_SCALE * (n0 + n1 + n2);
    }

}//namespace hgl::bitmap
