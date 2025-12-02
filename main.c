#include <stdio.h>
#include "raylib.h"
#include "game.h"
#include "config.h"

#define ARG_NONE	0x00
#define ARG_DEBUG 	0x01
 
int main() {
	// Initialize audio backend
	InitAudioDevice();
	SetMasterVolume(1);
	
	// Disable logs
	SetTraceLogLevel(LOG_ERROR);

	// Initialize game
	// Set window options, instantiate objects, allocate memory, etc.
	Game game = {0};
	GameInit(&game);

	// Open window, use values from config file
	SetConfigFlags(0);
	InitWindow(game.conf.window_width, game.conf.window_height, "Fish Game Demo");
	SetTargetFPS(game.conf.refresh_rate);
	
	// Load empty texture for use as a buffer
	// Buffer is drawn and scaled to window resolution
	GameRenderInit(&game);

	// Load game assets(spritesheets, sound effects, music, etc)
	GameContentInit(&game);

	SetExitKey(KEY_F10);	
	bool exit = false;

	// Main loop:
	while(!exit) {
		exit = ( WindowShouldClose() || (game.flags & GAME_QUIT_REQUEST) );	

		// Update game logic
		GameUpdate(&game);

		// Render to buffer
		GameDrawToBuffer(&game, (SHOW_DEBUG));

		// Render to screen
		GameDrawToWindow(&game);
	}

	// Cleanup:
	// Free allocated memory, close application
	GameClose(&game);
	CloseAudioDevice();
	CloseWindow();

	return 0;
}

