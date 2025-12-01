#include <stdio.h>
#include "raylib.h"
#include "sprites.h"

#define player_size (Vector2){144, 144}

void LoadSpritesAll(SpriteLoader *sl) {
	puts("Loading sprites...");

	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * PLAYER IDLE *

	LoadSpritesheet("resources/graphics/player/swim_idle.png", player_size, sl, SHEET_PLAYER);
	AddSpriteAnim(&sl->spr_pool[SHEET_PLAYER], 0, sl->spr_pool[SHEET_PLAYER].frame_count, 0.166f, sl, ANIM_PLAYER_SWIM_IDLE);

	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * PLAYER SWIM UP *

	LoadSpritesheet("resources/graphics/player/swim_up.png", player_size, sl, SHEET_PLAYER_SWIM_UP);
	AddSpriteAnim(&sl->spr_pool[SHEET_PLAYER_SWIM_UP], 0, sl->spr_pool[SHEET_PLAYER_SWIM_UP].frame_count, 0.333f, sl, ANIM_PLAYER_SWIM_UP);
		
	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * ASTEROIDS *

	LoadSpritesheet("resources/graphics/asteroid01.png", (Vector2){256, 256}, sl, SHEET_ASTEROIDS);

	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * FISH 0 *

	LoadSpritesheet("resources/graphics/fish/fish00.png", (Vector2){144, 81}, sl, SHEET_FISH_00);
	AddSpriteAnim(&sl->spr_pool[SHEET_FISH_00], 0, sl->spr_pool[SHEET_FISH_00].frame_count, 0.166f, sl, ANIM_FISH_SWIM_00);

	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * HARPOON *
	
	LoadSpritesheet("resources/graphics/player/harpoon.png", (Vector2){51, 38}, sl, SHEET_HARPOON);
	
	// -----------------------------------------------------------------------------------------------------------------------------------------
	// * PLAYER RECOIL *

	LoadSpritesheet("resources/graphics/player/recoil_lft.png", player_size, sl, SHEET_PLAYER_RECOIL_LFT);
	LoadSpritesheet("resources/graphics/player/recoil_rgt.png", player_size, sl, SHEET_PLAYER_RECOIL_RGT);

	AddSpriteAnim(
		&sl->spr_pool[SHEET_PLAYER_RECOIL_LFT], 0, sl->spr_pool[SHEET_PLAYER_RECOIL_LFT].frame_count, 0.233f, sl, ANIM_PLAYER_RECOIL_LFT);

	AddSpriteAnim(
		&sl->spr_pool[SHEET_PLAYER_RECOIL_RGT], 0, sl->spr_pool[SHEET_PLAYER_RECOIL_RGT].frame_count, 0.233f, sl, SHEET_PLAYER_RECOIL_RGT);

	// -----------------------------------------------------------------------------------------------------------------------------------------
	
}

