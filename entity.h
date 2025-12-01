#include <stdint.h>
#include "raylib.h"
#include "sprites.h"
#include "input.h"
#include "rope.h"

#ifndef ENTITY_H_
#define ENTITY_H_

#define ENT_ACTIVE			0x01
#define ENT_ORBIT			0x02
#define ENT_IS_BODY			0x04
#define ENT_GROUNDED		0x08
#define ENT_CAST_ORBIT		0x10
#define ENT_SWITCH_ORBIT	0x20

#define ENT_TYPE_COUNT	5

enum ENT_TYPE {
	ENT_PLAYER,		
	ENT_FISH,
	ENT_NPC,
	ENT_ASTEROID,
	ENT_ITEM
};

// *** BASE ENTITY STRUCT ***	
//
typedef struct Entity {
	uint8_t flags;			// Bit flags: active, anchored, alive, etc...
	uint8_t type;			// Entity type
	uint8_t sprite_id;		// Spritesheet index

	uint16_t id;

	float radius;
	float angle;
	float scale;
	float sprite_angle;		// Angle used for sprite rotation in degrees

	float angle_vel;

	Vector2 position, prev_pos, start_pos;
	Vector2 velocity;
	Vector2 center_offset;
	
	Vector2 pull_point_prev;
	Vector2 pull_point_next;
	Vector2 pull_point;
	
	// Primary entity function pointers:
	// functions specified on EntInit() 
	void (*update)(struct Entity *self, float dt);
	void (*draw)(struct Entity *self, SpriteLoader *sl);

	// Pointer to entity type specific data
	void *data;
} Entity;

// *** SHARED ENTITY FUNCTIONS ***
//
// Initialize entity, set function pointers, data, etc.
void EntInit(Entity *ent, uint8_t type);

// Add current velocity of entity to entity's position
void EntUpdatePosition(Entity *ent, float dt);

// Apply entity's angular velocity
void EntSpin(Entity *ent, float dt);

// Get entity's center point
Vector2 EntCenter(Entity *ent);

// *** HARPOON ***
typedef struct {
	uint8_t flags;
	uint8_t state;

	int16_t hit_id;

	float angle;
	float hit_angle;

	Vector2 position;
	Vector2 velocity;
	Vector2 hit_pos;
	Vector2 offset;
} Harpoon;

// *** PLAYER ***
//
typedef struct {
	uint8_t ex_flags;			// Extra flags
	uint8_t state;				// State used for animations some inputs

	short sprite_dir;			// Sprite direction
	short active_anim;		    // Index of current animation, -1 for none

	float rotate_timer;
	float rotate_accel;

	float rope_timer;

	float jetpack_timer; 
	float jetpack_accel;
	float jetpack_cooldown;

	float fling_timer;

	float fuel;
	float oxygen;

	Vector2 fling_vel;

	Vector2 cursor_pos;

	float *time_mod;

	Camera2D *camera;			// Pointer to camera instance
	InputState *input;			// Pointer to input state instance
	
	SpriteAnimation *run_anim;

	Rope *rope;

	Harpoon harpoon;
} PlayerData;

enum PLAYER_STATES {
	PLR_IDLE,
	PLR_SWIM,
	PLR_JUMP,
	PLR_FALL,
	PLR_AIM,
	PLR_SHOOT,
	PLR_RECOIL,
	PLR_DEAD
};

#define PLR_MAX_RUN_VEL		100.0f;
#define PLR_MAX_FALL_VEL	200.0f;

#define PLR_JUMP_GRAV	   1000.0f		
#define PLR_FALL_GRAV	    900.0f
#define PLR_CUT_GRAV	   1850.0f

#define HARPOON_ACTIVE		0x01
#define PLR_FLING 			0x02

enum HARPOON_STATES {
	HARPOON_NONE,
	HARPOON_AIM,
	HARPOON_EXTEND,
	HARPOON_RETRACT,
	HARPOON_STUCK,
	HARPOON_PULL,
	HARPOON_REEL
};

void *PlayerFetchData(Entity *player);

void PlayerInit(Entity *player, SpriteLoader *sl, Camera2D *camera);
void PlayerSpawn(Entity *player, Vector2 position);
void PlayerUpdate(Entity *player, float dt);
void PlayerDraw(Entity *player, SpriteLoader *sl);

void PlayerInput(Entity *player, float dt);

void PlayerPhysics(Entity *player, float dt);
void PlayerCameraControls(Entity *player, float dt);

void PlayerApplyJetpack(Entity *player, PlayerData *p, float dt, uint8_t on);
void PlayerHandleBodyCollision(Entity *player, PlayerData *p, Entity *ent, float dt);
void PlayerHandleFishCollision(Entity *player, PlayerData *p, Entity *ent, float dt);

void HarpoonUpdate(Entity *player, PlayerData *p, Harpoon *h, float dt);
void HarpoonDraw(Entity *player, PlayerData *p, Harpoon *h, SpriteLoader *sl);

void HarpoonInput(Entity *player, PlayerData *p, Harpoon *h, float dt);
void HarpoonCollision(Entity *player, PlayerData *p, Harpoon *h, float dt);

void HarpoonAim(Entity *player, PlayerData *p, Harpoon *h, float dt);
void HarpoonShoot(Entity *player, PlayerData *p, Harpoon *h);
void HarpoonExtend(Entity *player, PlayerData *p, Harpoon *h, float dt);
void HarpoonRetract(Entity *player, PlayerData *p, Harpoon *h, float dt);
void HarpoonStuck(Entity *player, PlayerData *p, Harpoon *h, float dt);
void HarpoonPull(Entity *player, PlayerData *p, Harpoon *h, float dt);
void HarpoonReel(Entity *player, PlayerData *p, Harpoon *h, float dt);

void PlayerFlingStart(Entity *player, PlayerData *p, Harpoon *h);

// *** ASTEROID ***
//
typedef struct {
	uint8_t ex_flags;
	uint8_t state;
	uint8_t frame;
} AsteroidData;

void AsteroidUpdate(Entity *asteroid, float dt);
void AsteroidDraw(Entity *asteroid, SpriteLoader *sl);

enum FISH_STATES {
	FISH_IDLE,
	FISH_SWIM,
	FISH_CAUGHT
};

// *** FISH ***
//
typedef struct {
	uint8_t subtype;
	uint8_t state; 
	uint8_t ex_flags;
	uint8_t size; 
	uint8_t rarity;

	float timer;

	Vector2 dir;
} FishData;

void FishUpdate(Entity *fish, float dt);
void FishDraw(Entity *fish, SpriteLoader *sl);

// *** NPC ***
//
typedef struct {
	uint8_t ex_flags;
	uint8_t state;
} NpcData;

void NpcUpdate(Entity *npc, float dt);
void NpcDraw(Entity *npc, SpriteLoader *sl);

typedef struct {
	uint8_t subtype;
	uint8_t ex_flags;
} ItemData;

#endif // !ENTITY_H_
