#include <stdint.h>
#include "raylib.h"

#ifndef INPUT_H_
#define INPUT_H_

typedef struct {
	KeyboardKey inc_key;
	KeyboardKey dec_key;
	
	GamepadAxis gp_axis;
	
	float value;
} DirectionInput;

typedef struct {
	uint8_t use_gamepad;

	bool jump;
	bool aim;
	bool shoot;
	bool interact;
	bool pause;
	bool jetpack;
	bool retract;
	bool cancel;

	float move_x, move_y;
	float look_x, look_y;

	float move_timer;
	float look_timer;

	float jump_timer;
	float shoot_timer;
	float interact_timer;
	float pause_timer;
} InputState;

void ProcessInput(InputState *state, float delta_time);
void PollInputGamepad(InputState *state);
void PollInputKeyboard(InputState *state);

void SetVibrate(InputState *state, float value, float t);

void InputDisplayInfo(InputState *state);

#endif
