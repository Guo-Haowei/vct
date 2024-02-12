#include "cbuffer.glsl.h"

layout(location = 0) in vec2 pass_uv;
layout(location = 0) out vec4 out_color;

#include "common.glsl"

// implementation https://blog.simonrodriguez.fr/articles/2016/07/implementing_fxaa.html

float rgb_to_luma(vec3 rgb) {
    return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

#define EDGE_THRESHOLD_MIN 0.0312
#define EDGE_THRESHOLD_MAX 0.125
#define SUBPIXEL_QUALITY   0.75
#define ITERATIONS         12

float QUALITY[] = {
    1.0, 1.0, 1.0, 1.0, 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0
};

void main() {
    const vec2 uv = pass_uv;
    const vec3 pixel = texture(c_fxaa_input_image, uv).rgb;
    if (c_enable_fxaa == 0) {
        out_color = vec4(pixel, 1.0);
        return;
    }

    vec3 color_center = texture(c_fxaa_input_image, uv).rgb;
    vec2 texel_size = 1.0 / vec2(c_screen_width, c_screen_height);

    // Luma at the current fragment
    float luma_center = rgb_to_luma(color_center);

    // Luma at the four direct neighbours of the current fragment.
    float luma_S = rgb_to_luma(texture(c_fxaa_input_image, uv + texel_size * vec2(0, -1)).rgb);
    float luma_N = rgb_to_luma(texture(c_fxaa_input_image, uv + texel_size * vec2(0, 1)).rgb);
    float luma_W = rgb_to_luma(texture(c_fxaa_input_image, uv + texel_size * vec2(-1, 0)).rgb);
    float luma_E = rgb_to_luma(texture(c_fxaa_input_image, uv + texel_size * vec2(1, 0)).rgb);

    // Find the maximum and minimum luma around the current fragment.
    float luma_min = min(luma_center, min(min(luma_S, luma_N), min(luma_W, luma_E)));
    float luma_max = max(luma_center, max(max(luma_S, luma_N), max(luma_W, luma_E)));

    // Compute the delta.
    float luma_range = luma_max - luma_min;

    // If the luma variation is lower that a threshold (or if we are in a really dark area), we are not on an edge, don't perform any AA.
    if (luma_range < max(EDGE_THRESHOLD_MIN, luma_max * EDGE_THRESHOLD_MAX)) {
        out_color = vec4(color_center, 1.0);
        return;
    }

    // Query the 4 remaining corners lumas.
    float luma_SW = rgb_to_luma(texture(c_fxaa_input_image, uv + texel_size * vec2(-1, -1)).rgb);
    float luma_SE = rgb_to_luma(texture(c_fxaa_input_image, uv + texel_size * vec2(1, -1)).rgb);
    float luma_NE = rgb_to_luma(texture(c_fxaa_input_image, uv + texel_size * vec2(1, 1)).rgb);
    float luma_NW = rgb_to_luma(texture(c_fxaa_input_image, uv + texel_size * vec2(-1, 1)).rgb);

    // @TODO: investigate why the visual effect isn't great
    // Combine the four edges lumas (using intermediary variables for future computations with the same values).
    float lumaDownUp = luma_S + luma_N;
    float lumaLeftRight = luma_W + luma_E;

    // Same for corners
    float lumaLeftCorners = luma_SW + luma_NW;
    float lumaDownCorners = luma_SW + luma_SE;
    float lumaRightCorners = luma_SE + luma_NE;
    float lumaUpCorners = luma_NE + luma_NW;

    // Compute an estimation of the gradient along the horizontal and vertical axis.
    float edgeHorizontal = abs(-2.0 * luma_W + lumaLeftCorners) + abs(-2.0 * luma_center + lumaDownUp) * 2.0 + abs(-2.0 * luma_E + lumaRightCorners);
    float edgeVertical = abs(-2.0 * luma_N + lumaUpCorners) + abs(-2.0 * luma_center + lumaLeftRight) * 2.0 + abs(-2.0 * luma_S + lumaDownCorners);

    // Is the local edge horizontal or vertical ?
    bool isHorizontal = (edgeHorizontal >= edgeVertical);

    // Select the two neighboring texels lumas in the opposite direction to the local edge.
    float luma1 = isHorizontal ? luma_S : luma_W;
    float luma2 = isHorizontal ? luma_N : luma_E;
    // Compute gradients in this direction.
    float gradient1 = luma1 - luma_center;
    float gradient2 = luma2 - luma_center;

    // Which direction is the steepest ?
    bool is1Steepest = abs(gradient1) >= abs(gradient2);

    // Gradient in the corresponding direction, normalized.
    float gradientScaled = 0.25 * max(abs(gradient1), abs(gradient2));

    // Choose the step size (one pixel) according to the edge direction.
    float stepLength = isHorizontal ? texel_size.y : texel_size.x;

    // Average luma in the correct direction.
    float lumaLocalAverage = 0.0;

    if (is1Steepest) {
        // Switch the direction
        stepLength = -stepLength;
        lumaLocalAverage = 0.5 * (luma1 + luma_center);
    } else {
        lumaLocalAverage = 0.5 * (luma2 + luma_center);
    }

    // Shift UV in the correct direction by half a pixel.
    vec2 currentUv = uv;
    if (isHorizontal) {
        currentUv.y += stepLength * 0.5;
    } else {
        currentUv.x += stepLength * 0.5;
    }

    // Compute offset (for each iteration step) in the right direction.
    vec2 offset = isHorizontal ? vec2(texel_size.x, 0.0) : vec2(0.0, texel_size.y);
    // Compute UVs to explore on each side of the edge, orthogonally. The QUALITY allows us to step faster.
    vec2 uv1 = currentUv - offset;
    vec2 uv2 = currentUv + offset;

    // Read the lumas at both current extremities of the exploration segment, and compute the delta wrt to the local average luma.
    float lumaEnd1 = rgb_to_luma(texture(c_fxaa_input_image, uv1).rgb);
    float lumaEnd2 = rgb_to_luma(texture(c_fxaa_input_image, uv2).rgb);
    lumaEnd1 -= lumaLocalAverage;
    lumaEnd2 -= lumaLocalAverage;

    // If the luma deltas at the current extremities are larger than the local gradient, we have reached the side of the edge.
    bool reached1 = abs(lumaEnd1) >= gradientScaled;
    bool reached2 = abs(lumaEnd2) >= gradientScaled;
    bool reachedBoth = reached1 && reached2;

    // If the side is not reached, we continue to explore in this direction.
    if (!reached1) {
        uv1 -= offset;
    }
    if (!reached2) {
        uv2 += offset;
    }

    // If both sides have not been reached, continue to explore.
    if (!reachedBoth) {
        for (int i = 2; i < ITERATIONS; i++) {
            // If needed, read luma in 1st direction, compute delta.
            if (!reached1) {
                lumaEnd1 = rgb_to_luma(texture(c_fxaa_input_image, uv1).rgb);
                lumaEnd1 = lumaEnd1 - lumaLocalAverage;
            }
            // If needed, read luma in opposite direction, compute delta.
            if (!reached2) {
                lumaEnd2 = rgb_to_luma(texture(c_fxaa_input_image, uv2).rgb);
                lumaEnd2 = lumaEnd2 - lumaLocalAverage;
            }
            // If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge.
            reached1 = abs(lumaEnd1) >= gradientScaled;
            reached2 = abs(lumaEnd2) >= gradientScaled;
            reachedBoth = reached1 && reached2;

            // If the side is not reached, we continue to explore in this direction, with a variable quality.
            if (!reached1) {
                uv1 -= offset * QUALITY[i];
            }
            if (!reached2) {
                uv2 += offset * QUALITY[i];
            }

            // If both sides have been reached, stop the exploration.
            if (reachedBoth) {
                break;
            }
        }
    }

    // Compute the distances to each extremity of the edge.
    float distance1 = isHorizontal ? (uv.x - uv1.x) : (uv.y - uv1.y);
    float distance2 = isHorizontal ? (uv2.x - uv.x) : (uv2.y - uv.y);

    // In which direction is the extremity of the edge closer ?
    bool isDirection1 = distance1 < distance2;
    float distanceFinal = min(distance1, distance2);

    // Length of the edge.
    float edgeThickness = (distance1 + distance2);

    // UV offset: read in the direction of the closest side of the edge.
    float pixelOffset = -distanceFinal / edgeThickness + 0.5;

    // Is the luma at center smaller than the local average ?
    bool isLumaCenterSmaller = luma_center < lumaLocalAverage;

    // If the luma at center is smaller than at its neighbour, the delta luma at each end should be positive (same variation).
    // (in the direction of the closer side of the edge.)
    bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;

    // If the luma variation is incorrect, do not offset.
    float finalOffset = correctVariation ? pixelOffset : 0.0;

    // Sub-pixel shifting
    // Full weighted average of the luma over the 3x3 neighborhood.
    float lumaAverage = (1.0 / 12.0) * (2.0 * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);
    // Ratio of the delta between the global average and the center luma, over the luma range in the 3x3 neighborhood.
    float subPixelOffset1 = clamp(abs(lumaAverage - luma_center) / luma_range, 0.0, 1.0);
    float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
    // Compute a sub-pixel offset based on this delta.
    float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;

    // Pick the biggest of the two offsets.
    finalOffset = max(finalOffset, subPixelOffsetFinal);

    // Compute the final UV coordinates.
    vec2 finalUv = uv;
    if (isHorizontal) {
        finalUv.y += finalOffset * stepLength;
    } else {
        finalUv.x += finalOffset * stepLength;
    }

    // Read the color at the new UV coordinates, and use it.
    vec3 finalColor = texture(c_fxaa_input_image, finalUv).rgb;
    out_color = vec4(finalColor, 1.0);
}
