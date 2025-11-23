#include "raylib.h"

#ifndef KMATH_H_
#define KMATH_H_

#define PI2 (PI*2)

float AngleLerp(float a, float b, float t);
float ILerp(float a, float b, float t, float delta_time);

Vector2 GetDirection(Vector2 destination, Vector2 origin);
Vector2 GetDirectionNormalized(Vector2 destination, Vector2 origin);

#endif // KMATH_H_
