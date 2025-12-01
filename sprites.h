#ifndef SPRITES_H_
#define SPRITES_H_

#include <stdint.h>
#include "raylib.h"

#define SPR_TEX_VALID	0x01
#define SPR_ALLOCATED	0x02
#define SPR_PERSIST  	0x04
#define SPR_FLIP_X	   	0x08
#define SPR_FLIP_Y	   	0x10

typedef struct {
	uint8_t flags;
	uint16_t frame_count;		// Number of frames
	uint16_t cols, rows;		// Number of columns and rows

	uint16_t frame_w;			// Frame width
	uint16_t frame_h;			// Frame height
	
	Texture2D texture;			// Source image
} Spritesheet;

Spritesheet SpritesheetCreate(char *texture_path, Vector2 frame_dimensions);
void SpritesheetClose(Spritesheet *spritesheet);

void DrawSprite(Spritesheet *spritesheet, uint8_t frame_index, Vector2 position, uint8_t flags);
void DrawSpritePro(Spritesheet *spritesheet, uint8_t frame_index, Vector2 position, float rotation, float scale, uint8_t flags);

uint8_t FrameIndex(Spritesheet *spritesheet, uint8_t c, uint8_t r);
Rectangle GetFrameRec(uint8_t idx, Spritesheet *spritesheet);

typedef struct {
	uint16_t frame_count;		// Total number of frames 
	uint16_t start_frame;		// Which frame is "zero" or first frame
	uint16_t cur_frame;			// Which frame should be displayed  

	uint16_t cycles;			// Number of times animation has been completed  

	float speed;				// Animation speed
	float timer;				// Timer for switching frames

	Spritesheet *spritesheet; 	// Pointer to spritesheet instance
} SpriteAnimation;

SpriteAnimation AnimCreate(Spritesheet *spritesheet, uint8_t start_frame, uint8_t frame_count, float speed);
void AnimPlay(SpriteAnimation *anim, float delta_time);
void AnimDraw(SpriteAnimation *anim, Vector2 position, uint8_t flags);
void AnimDrawPro(SpriteAnimation *anim, Vector2 position, float rotation, float scale, uint8_t flags);
void AnimReset(SpriteAnimation *anim);

#define SPR_POOL_CAPACITY	64

enum SPRITESHEETS {
	SHEET_PLAYER,
	SHEET_PLAYER_RECOIL_LFT,
	SHEET_PLAYER_RECOIL_RGT,
	SHEET_PLAYER_SWIM_UP,
	SHEET_HARPOON,

	SHEET_ASTEROIDS,
	SHEET_ITEMS,

	SHEET_FISH_00,
	SHEET_FISH_01,
	SHEET_FISH_02,

	SHEET_STAR_00,
	SHEET_STAR_01,
	SHEET_STAR_02,
	SHEET_STAR_03,
};

enum ANIMATIONS {
	ANIM_PLAYER_SWIM_IDLE,
	ANIM_PLAYER_RECOIL_LFT,
	ANIM_PLAYER_RECOIL_RGT,
	ANIM_PLAYER_SWIM_UP,

	ANIM_FISH_SWIM_00,
	ANIM_FISH_SWIM_01,
	ANIM_FISH_SWIM_02,

	ANIM_STAR_00,
	ANIM_STAR_01,
	ANIM_STAR_02,
	ANIM_STAR_03,
};

typedef struct {
	uint8_t spr_count;
	uint8_t anim_count;

	Spritesheet spr_pool[SPR_POOL_CAPACITY];
	SpriteAnimation anims[SPR_POOL_CAPACITY];
} SpriteLoader;

void LoadSpritesheet(char *tex_path, Vector2 frame_dimensions, SpriteLoader *sl, uint8_t id);
void AddSpriteAnim(Spritesheet *spritesheet, uint8_t start_frame, uint8_t frame_count, float speed, SpriteLoader *sl, uint8_t id);
void SpriteLoaderClose(SpriteLoader *sl);

void LoadSpritesAll(SpriteLoader *sl);

#endif // !SPRITES_H_ 
