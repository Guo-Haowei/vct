#pragma once
#include "math/GeoMath.h"
#include <vector>

namespace internal {

static void genBoxWireframe(std::vector<vec3>& outPositions, std::vector<unsigned int>& outIndices)
{
    /**
     *        E__________________ H
     *       /|                 /|
     *      / |                / |
     *     /  |               /  |
     *   A/___|______________/D  |
     *    |   |              |   |
     *    |   |              |   |
     *    |   |              |   |
     *    |  F|______________|___|G
     *    |  /               |  /
     *    | /                | /
     *   B|/_________________|C
     * 
     */
    enum VertexIndex { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7 };

    float scale = 0.5f;
    outPositions.clear();
    outPositions = {
        { -scale, +scale, +scale }, // A
        { -scale, -scale, +scale }, // B
        { +scale, -scale, +scale }, // C
        { +scale, +scale, +scale }, // D
        { -scale, +scale, -scale }, // E
        { -scale, -scale, -scale }, // F
        { +scale, -scale, -scale }, // G
        { +scale, +scale, -scale }  // H
    };

    outIndices.clear();
    outIndices = {
        A, B, B, C, C, D, D, A,
        E, F, F, G, G, H, H, E,
        A, E, B, F, D, H, C, G
    };
}

static void genBoxNoNormals(std::vector<vec3>& outPositions, std::vector<unsigned int>& outIndices)
{
    /**
     *        E__________________ H
     *       /|                 /|
     *      / |                / |
     *     /  |               /  |
     *   A/___|______________/D  |
     *    |   |              |   |
     *    |   |              |   |
     *    |   |              |   |
     *    |  F|______________|___|G
     *    |  /               |  /
     *    | /                | /
     *   B|/_________________|C
     * 
     */
    enum VertexIndex { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7 };

    float scale = 0.5f;
    outPositions.clear();
    outPositions = {
        { -scale, +scale, +scale }, // A
        { -scale, -scale, +scale }, // B
        { +scale, -scale, +scale }, // C
        { +scale, +scale, +scale }, // D
        { -scale, +scale, -scale }, // E
        { -scale, -scale, -scale }, // F
        { +scale, -scale, -scale }, // G
        { +scale, +scale, -scale }  // H
    };

    outIndices.clear();
    outIndices = {
        A, B, D, // ABD
        D, B, C, // DBC
        E, H, F, // EHF
        H, G, F, // HGF

        D, C, G, // DCG
        D, G, H, // DGH
        A, F, B, // AFB
        A, E, F, // AEF

        A, D, H, // ADH
        A, H, E, // AHE
        B, F, G, // BFG
        B, G, C, // BGC
    };
}


} // namespace::internal
