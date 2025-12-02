#include "raylib.h"
#include "input.h"

void ProcessInput(InputState *input, float delta_time) {
	// Poll input
	if(IsGamepadAvailable(0))
		PollInputGamepad(input);
	else
		PollInputKeyboard(input);

	// TODO: 
	// Manage timers
}

void PollInputKeyboard(InputState *input) {
	input->move_x = 0;
	input->move_y = 0;

	if(IsKeyDown(KEY_A)) input->move_x -= 1;
	if(IsKeyDown(KEY_D)) input->move_x += 1;

	if(IsKeyDown(KEY_W)) input->move_y -= 1;
	if(IsKeyDown(KEY_S)) input->move_y += 1;

	input->jetpack 	= IsKeyDown(KEY_SPACE);
	input->aim 		= IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
	input->retract 	= IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	input->shoot 	= IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void PollInputGamepad(InputState *input) {
	// TODO:
	// Implement gamepad input polling
	input->move_x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
	input->move_y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

	input->jetpack  = IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
	input->aim 	    = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_2);
	input->retract  = IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2);
	input->shoot 	= IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2);
}

