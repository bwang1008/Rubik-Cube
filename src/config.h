#ifndef CONFIG_H
#define CONFIG_H

/*
 * Global variables go here.
 */

/*
 * Extra credit: what would happen if you set kNear to 1e-5? How to solve it?
 */
const float kNear = 0.1f;
const float kFar = 1000.0f;
const float kFov = 45.0f;

// Floor info.
const float kFloorEps = 0.5 * (0.025 + 0.0175);
const float kFloorXMin = -100.0f;
const float kFloorXMax = 100.0f;
const float kFloorZMin = -100.0f;
const float kFloorZMax = 100.0f;
const float kFloorY = -0.75617 - kFloorEps;

const float kScrollSpeed = 64.0f;

const float cubieWidth = 1.0f;
const int cubeWidth = 1;

#endif
