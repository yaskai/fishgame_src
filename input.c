#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "raylib.h"
#include "raymath.h"
#include "input.h"

int gamepad = 0;
bool gamepad_found = 0;

void ProcessInput(InputState *input, float delta_time) {
	// Find gamepad
	if(!gamepad_found) {
		for(short i = 0; i < 10; i++) {
			if(IsGamepadAvailable(i)) {
				if((strcmp(GetGamepadName(i), "Xbox") >= 4)) {
					gamepad = i;
					printf("Gamepad set to: %s, id = %d\n", GetGamepadName(i), i);
					gamepad_found = true;
					break;
				}
			}
		}
	}

	// Poll input
	if(IsGamepadAvailable(gamepad)) {
		PollInputGamepad(input);
		return;
	}

	PollInputKeyboard(input);

	// TODO: 
	// Manage timers
}

void PollInputKeyboard(InputState *input) {
	input->use_gamepad = 0;

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
	input->use_gamepad = true;

	input->move_x = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_X);
	input->move_y = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_LEFT_Y);

	if(input->move_x > -0.1f && input->move_x < 0.1f) {
		input->move_x = 0.0f;
	}

	if(input->move_y > -0.1f && input->move_y < 0.1f) {
		input->move_y = 0.0f;
	}

	input->look_x = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_X);
	input->look_y = GetGamepadAxisMovement(gamepad, GAMEPAD_AXIS_RIGHT_Y);

	if(input->look_x > -0.1f && input->look_x < 0.1f) {
		input->look_x = 0.0f;
	}

	if(input->look_y > -0.1f && input->look_y < 0.1f) {
		input->look_y = 0.0f;
	}

	input->jetpack  = IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
	input->aim 	    = IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_LEFT_TRIGGER_2);
	input->retract  = IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2);
	input->shoot 	= IsGamepadButtonPressed(gamepad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2);

	/*
	if(GetGamepadButtonPressed() != 0)
		printf("%d\n", GetGamepadButtonPressed());
	*/
}

void SetVibrate(InputState *state, float value, float t) {
	SetGamepadVibration(gamepad, value, value, t);
}

