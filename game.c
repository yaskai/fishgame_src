#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "game.h"
#include "audioplayer.h"
#include "config.h"
#include "sprites.h"
#include "map.h"

Texture2D controls;

// Buffer texture game draws to, used for scaling graphics to desired resolution 
RenderTexture2D render_target;

// Game state update and draw function type defines
typedef void(*UpdateFunc)(Game *game, float dt);
typedef void(*DrawFunc)(Game *game, uint8_t flags);

// Game state update and draw function arrays, state acts as index 
// (ie. state = main, game_update_funcs[main] called in GameUpdate)
UpdateFunc game_update_funcs[] = { TitleUpdate, MainUpdate, OverScreenUpdate };
DrawFunc game_draw_funcs[] = { TitleDraw, MainDraw, OverScreenDraw };

// Initialize data, allocate memory, etc.
void GameInit(Game *game) {
	// Initialize config struct and read options from file
	game->conf = (Config){0};
	ConfigRead(&game->conf, "options.conf");

	// Initialize camera
	game->cam = (Camera2D) {
		.target = {0, 0},
		.offset = Vector2Scale((Vector2){VIRTUAL_WIDTH, VIRTUAL_HEIGHT}, 0.5f),
		.rotation = 0.0f,
		.zoom = 1.0f
	};

	// Initialize input 
	game->input_state = (InputState){0};

	// Initialize entity handler
	EntHandlerInit(&game->ent_handler, &game->sprite_loader, &game->cam);
	game->ent_handler.ap = &game->audio_player;
	game->ent_handler.debug_flags = game->conf.debug_flags;
}

// Initialize necessary data for rendering the game 
void GameRenderInit(Game *game) {
	// Load empty texture, used as buffer for scaling
	render_target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
	SetTextureFilter(render_target.texture, TEXTURE_FILTER_POINT);

	// Set source and destination rectangle values for window scaling
	game->render_src_rec  = (Rectangle) { 0, 0, VIRTUAL_WIDTH, -VIRTUAL_HEIGHT };
	game->render_dest_rec = (Rectangle) { 0, 0, game->conf.window_width, game->conf.window_height };

	BgInit(&game->bg);
}

// Initialize sprite loader struct, load assets
void GameContentInit(Game *game) {
	game->sprite_loader = (SpriteLoader){0};
	LoadSpritesAll(&game->sprite_loader);

	//game->audio_player = (AudioPlayer){0};
	AudioPlayerInit(&game->audio_player);

	controls = LoadTexture("resources/fishgamecontrols.jpg");
	SetTextureFilter(controls, TEXTURE_FILTER_TRILINEAR);
}

void GameUpdate(Game *game) {
	// Get delta time once only, pass to other update functions
	float delta_time = GetFrameTime();

	// Send quit request on hitting escape
	if(IsKeyPressed(KEY_ESCAPE))
		game->flags |= GAME_QUIT_REQUEST;

	// Default to gamepad controls if available and input method unspecified
	if((game->flags & INPUT_SPECIFIED) == 0) 
		if(IsGamepadAvailable(0)) game->input_method = GAMEPAD; 

	// Poll input
	ProcessInput(&game->input_state, delta_time);
	
	// Call state appropriate update function
	game_update_funcs[game->state](game, delta_time);
}

// Render game to buffer texture
void GameDrawToBuffer(Game *game, uint8_t flags) {
	if(game->state == GAME_MAIN) 
		BgDraw(&game->bg);

	BeginTextureMode(render_target);
	ClearBackground((Color){0});

	// Call state appropriate draw function
	game_draw_funcs[game->state](game, flags);

	EndTextureMode();
}

// Render buffer onto window
void GameDrawToWindow(Game *game) {
	BeginDrawing();
	ClearBackground(BLACK);

	DrawTexturePro(game->bg.render_texture.texture, game->render_src_rec, game->render_dest_rec, Vector2Zero(), 0, WHITE);
	
	// Draw scaled render_target texture to window 
	DrawTexturePro(render_target.texture, game->render_src_rec, game->render_dest_rec, Vector2Zero(), 0, WHITE);
	EndDrawing();
}

// Free allocated memory for buffer texture and assets 
void GameClose(Game *game) {
	EntHandlerClose(&game->ent_handler);
	UnloadRenderTexture(render_target);
	SpriteLoaderClose(&game->sprite_loader);
	AudioPlayerClose(&game->audio_player);
}

// Update title screen UI elements, start gameplay on user input
void TitleUpdate(Game *game, float delta_time) {
	PlayTrack(&game->audio_player, 1);

	if(IsKeyPressed(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
		MainStart(game);
}

// Draw title screen graphics
void TitleDraw(Game *game, uint8_t flags) {
	Vector2 screen_center = Vector2Scale((Vector2){VIRTUAL_WIDTH, VIRTUAL_HEIGHT}, 0.5f);
	char *prompt_text = (game->input_method == KEYBOARD) ? "press space to play" : "press A to play";

	//DrawText("Fish game Demo", screen_center.x - 380, screen_center.y - 100, 100, RAYWHITE);
	DrawText(prompt_text, screen_center.x - 160, screen_center.y + 100, 32, RAYWHITE);

	DrawTextureRec(controls, (Rectangle){0, 0, controls.width, controls.height}, (Vector2){0, 0}, WHITE);
	DrawTexturePro(
		controls,
		(Rectangle){0, 0, controls.width, controls.height},
		(Rectangle){0, 0, 1920 * 0.5f, 1080 * 0.5f},
		(Vector2){0, 0},
		0, 
		WHITE);
}

// Main gameplay loop logic
void MainUpdate(Game *game, float delta_time) {
	//game->ent_handler.game_timer -= delta_time;
	if(game->ent_handler.game_timer <= 0)
		game->state = GAME_END;

	EntHandlerUpdate(&game->ent_handler, delta_time);
}

// Render objects to buffer texture
void MainDraw(Game *game, uint8_t flags) {
	// No camera transformations:

	// Draw background
	//BgDraw(&game->bg);
	//DrawTexturePro(game->bg.render_texture.texture, game->render_src_rec, game->render_dest_rec, Vector2Zero(), 0, WHITE);

	// With camera transformations:
	BeginMode2D(game->cam);
	EntHandlerDraw(&game->ent_handler, (SHOW_DEBUG));
	EndMode2D();
	
	// No camera transformations:
	int total  = (int)game->ent_handler.game_timer;
	int minute = total / 60;
	int second = total % 60;
	DrawText(TextFormat("%02d:%02d", (int)minute, (int)second), 32, 32, 64, RAYWHITE);
	DrawText(TextFormat("caught: %d", game->ent_handler.fish_collected), 32, 80, 64, RAYWHITE);
}

void OverScreenUpdate(Game *game, float delta_time) {
	if(IsKeyPressed(KEY_SPACE)) {
		MainStart(game);
	}
}

void OverScreenDraw(Game *game, uint8_t flags) {
	Vector2 screen_center = Vector2Scale((Vector2){VIRTUAL_WIDTH, VIRTUAL_HEIGHT}, 0.5f);
	char *prompt_text = (game->input_method == KEYBOARD) ? "press space to play again" : "press A to play";

	DrawText(TextFormat("Caught %d fish!", game->ent_handler.fish_collected), screen_center.x - 180, screen_center.y - 100, 50, RAYWHITE);
	DrawText(prompt_text, screen_center.x - 160, screen_center.y + 100, 32, RAYWHITE);
}

void OptionsScreenUpdate(Game *game, float delta_time) {
}

void OptionsScreenDraw(Game *game, uint8_t flags) {
}

// Start gameplay
void MainStart(Game *game) {
	StopTrack(&game->audio_player, 0);
	//PlayEffect(&game->audio_player, 0);

	EntHandlerClear(&game->ent_handler);

	char level_path[64];

	if(TARGET_PLATFORM == PLATFORM_WIN64)
		strcpy(level_path, "resources\\\\levels\\\\");
	else
		strcpy(level_path, "resources/levels/");

	strcat(level_path, game->conf.level_path);

	MapLoad(&game->ent_handler, level_path);

	//printf(">[%s]<\n", game->conf.level_path);
	//MapLoad(&game->ent_handler, game->conf.level_path);

	PlayerSetHandler(&game->ent_handler);
	RopeSetHandler(&game->ent_handler);
	
	Entity *player = &game->ent_handler.ents[game->ent_handler.player_id];
	PlayerData *player_data = player->data;
	player_data->input = &game->input_state;

	game->cam.target = EntCenter(player);

	game->state = GAME_MAIN;
	game->ent_handler.game_timer = 180;
	game->ent_handler.fish_collected = 0;
}

