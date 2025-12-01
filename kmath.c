#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "kmath.h"

float AngleLerp(float a, float b, float t) {
	t *= 100.0f;
	float d = b - a;

	while(d >  180) d -= 360;
	while(d < -180) d += 360;

	return a + (d * t);
}

float ILerp(float a, float b, float t, float delta_time) {
	return 1 - Lerp(a, b, pow(t, delta_time));
}

Vector2 GetDirection(Vector2 destination, Vector2 origin) {
	return Vector2Subtract(destination, origin);
}

Vector2 GetDirectionNormalized(Vector2 destination, Vector2 origin) {
	return Vector2Normalize(Vector2Subtract(destination, origin));
}

Vector2 GetForward(float angle) {
	return (Vector2) { cosf(angle), sinf(angle) };
}
