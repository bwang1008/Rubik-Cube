#ifndef CONFIG_H
#define CONFIG_H

/*
 * Global variables go here.
 */

/*
 * Extra credit: what would happen if you set kNear to 1e-5? How to solve it?
 */
const float kNear = 0.1f;
const float kFar = 10000.0f; // 1000.0f;
const float kFov = 45.0f;

const float kScrollSpeed = 64.0f;

// RGB of green, red, white, yellow, orange, blue
const int kColors[6][3] = { {46, 139, 87}, {255, 0, 0}, {255, 255, 255}, {255, 255, 0}, {255, 140, 0}, {0, 0, 255} };
const bool kOptimize = true;
const int cubeWidth = 19;
#endif
