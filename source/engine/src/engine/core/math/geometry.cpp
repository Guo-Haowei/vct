#include "geometry.h"

namespace vct {

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

// clang-format off
enum { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7 };
// clang-format on

MeshComponent make_box_mesh(float size) {
    MeshComponent mesh;
    mesh.positions = {
        vec3(-size, +size, +size),  // A
        vec3(-size, -size, +size),  // B
        vec3(+size, -size, +size),  // C
        vec3(+size, +size, +size),  // D
        vec3(-size, +size, -size),  // E
        vec3(-size, -size, -size),  // F
        vec3(+size, -size, -size),  // G
        vec3(+size, +size, -size),  // H
    };

    mesh.indices = {
        A, B, D,  // ABD
        D, B, C,  // DBC
        E, H, F,  // EHF
        H, G, F,  // HGF
        D, C, G,  // DCG
        D, G, H,  // DGH
        A, F, B,  // AFB
        A, E, F,  // AEF
        A, D, H,  // ADH
        A, H, E,  // AHE
        B, F, G,  // BFG
        B, G, C,  // BGC
    };

    return mesh;
}

// load scene
MeshComponent make_box_wireframe_mesh(float size) {
    MeshComponent mesh;
    mesh.positions = {
        vec3(-size, +size, +size),  // A
        vec3(-size, -size, +size),  // B
        vec3(+size, -size, +size),  // C
        vec3(+size, +size, +size),  // D
        vec3(-size, +size, -size),  // E
        vec3(-size, -size, -size),  // F
        vec3(+size, -size, -size),  // G
        vec3(+size, +size, -size),  // H
    };

    mesh.indices = { A, B, B, C, C, D, D, A, E, F, F, G, G, H, H, E, A, E, B, F, D, H, C, G };

    return mesh;
}

// clang-format off
void boxWithNormal(std::vector<vec3>& outPoints, std::vector<vec3>& outNomrals, std::vector<unsigned int>& outIndices, float size)
{
    outPoints.clear();
    outPoints = {
        { -size, +size, +size }, // A
        { -size, -size, +size }, // B
        { +size, -size, +size }, // C
        { +size, +size, +size }, // D

        { -size, +size, -size }, // E
        { -size, -size, -size }, // F
        { +size, -size, -size }, // G
        { +size, +size, -size }, // H

        { -size, +size, -size }, // E 8
        { -size, -size, -size }, // F 9
        { -size, -size, +size }, // B 10
        { -size, +size, +size }, // A 11

        { +size, +size, -size }, // H 12
        { +size, -size, -size }, // G 13
        { +size, -size, +size }, // C 14
        { +size, +size, +size }, // D 15

        { -size, +size, -size }, // E 16
        { -size, +size, +size }, // A 17
        { +size, +size, +size }, // D 18
        { +size, +size, -size }, // H 19

        { -size, -size, -size }, // F 20
        { -size, -size, +size }, // B 21
        { +size, -size, +size }, // C 22
        { +size, -size, -size }, // G 23
    };

    constexpr vec3 UnitX(1, 0, 0);
    constexpr vec3 UnitY(0, 1, 0);
    constexpr vec3 UnitZ(0, 0, 1);

    outNomrals.clear();
    outNomrals = {
        +UnitZ, +UnitZ, +UnitZ, +UnitZ,
        -UnitZ, -UnitZ, -UnitZ, -UnitZ,
        -UnitX, -UnitX, -UnitX, -UnitX,
        +UnitX, +UnitX, +UnitX, +UnitX,
        +UnitY, +UnitY, +UnitY, +UnitY,
        -UnitY, -UnitY, -UnitY, -UnitY,
    };

    outIndices.clear();
    outIndices = {
        0, 1, 3, 3, 1, 2,
        4, 7, 5, 7, 6, 5,
        8, 9, 11, 9, 10, 11,
        15, 14, 13, 15, 13, 12,
        16, 17, 18, 16, 18, 19,
        21, 20, 22, 20, 23, 22,
    };
}

}  // namespace vct
