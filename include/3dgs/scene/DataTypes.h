#pragma once

#include "pch.h"

namespace iiixrlab::scene
{
    struct GaussianInfo
    {
        // Total number of points (gaussians) in this splat.
        uint32_t NumPoints = 0;
        // Degree of spherical harmonics for this splat.
        uint32_t ShDegree = 0;
        // Whether the gaussians should be rendered in antialiased mode (mip splatting)
        bool isAntialiased = false;  
        // See block comment above for details
        std::vector<float> Positions;
        std::vector<float> Scales;
        std::vector<float> Rotations;
        std::vector<float> Alphas;
        std::vector<float> Colors;
    
        // Spherical harmonics coefficients. The number of coefficients per point depends on shDegree:
        //   0 -> 0
        //   1 -> 9   (3 coeffs x 3 channels)
        //   2 -> 24  (8 coeffs x 3 channels)
        //   3 -> 45  (15 coeffs x 3 channels)
        // The color channel is the inner (fastest varying) axis, and the coefficient is the outer
        // (slower varying) axis, i.e. for degree 1, the order of the 9 values is:
        //   sh1n1_r, sh1n1_g, sh1n1_b, sh10_r, sh10_g, sh10_b, sh1p1_r, sh1p1_g, sh1p1_b
        std::vector<float> SphericalHarmonics;
    };
} // namespace iiixrlab::scene
