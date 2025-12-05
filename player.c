#include <math.h>
#include <stdint.h>
#include <float.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "entity.h"
#include "sprites.h"
#include "ent_handler.h"
#include "kmath.h"
#include "audioplayer.h"

#define PLAYER_MAX_VEL 		700.0f
#define CAM_ZOOM_DEFAULT 	0.85f
#define CAM_ZOOM_FOCUSED	0.95f
#define SCREENSHAKE_MAX		10

#define RECOIL_AMOUNT		100.04

#define GP_CURSOR_SPEED		500.0f
#define GP_CURSOR_ACCEL		10.0f
#define GP_CURSOR_DEADZONE	0.550f

EntHandler *ptr_player_handler; 
void PlayerSetHandler(EntHandler *handler) { ptr_player_handler = handler; }

void *PlayerFetchData(Entity *player) { return player->data; }

bool jetpack_on = false;
float screenshake = 0;

Vector2 rope_grav_targ = (Vector2){0, 0};

Rope rope = (Rope){0};
uint8_t temp_rope_start = 0;

Vector2 glob_player_center = (Vector2){0, 0};
Vector2 glob_harpoon_pos   = (Vector2){0, 0};

Vector2 debug_ray_start[10] = {};
Vector2 debug_ray_end[10] = {};

SpriteAnimation *idle_anim;
SpriteAnimation *recoil_anims[2];
SpriteAnimation *swim_up_anim;

uint8_t recoil_dir;

float zoom_targ = CAM_ZOOM_DEFAULT;

Vector2 last_look;

float aim_angle = 0;

// Initialize player, set data, pointers, references, etc.
void PlayerInit(Entity *player, SpriteLoader *sl, Camera2D *camera) {
	PlayerData *p = PlayerFetchData(player);
	*p = (PlayerData){0};

	p->active_anim = 0;
	player->angle = 0;
	player->angle = -90 * DEG2RAD;

	player->center_offset = (Vector2) { sl->spr_pool[SHEET_PLAYER].frame_w * 0.5f, sl->spr_pool[SHEET_PLAYER].frame_h * 0.5f };
	
	player->radius = player->center_offset.y * 0.5f;
	player->sprite_id = SHEET_PLAYER;

	// Set camera pointer
	p->camera = camera;

	// Initialize rope
	p->rope = &rope;
	RopeInit(p->rope, EntCenter(player));
	RopeSetHandler(ptr_player_handler);

	// Initialize harpoon
	p->harpoon = (Harpoon){0};

	p->rope->anchors[0] = &glob_player_center;
	p->rope->anchors[1] = &p->harpoon.position;

	// Set animation pointers
	idle_anim = &sl->anims[ANIM_PLAYER_SWIM_IDLE];
	recoil_anims[0] = &sl->anims[ANIM_PLAYER_RECOIL_LFT]; 
	recoil_anims[1] = &sl->anims[ANIM_PLAYER_RECOIL_RGT]; 
	swim_up_anim = &sl->anims[ANIM_PLAYER_SWIM_UP];
}

// Update player
void PlayerUpdate(Entity *player, float dt) {
	// Handle input
	PlayerInput(player, dt);

	PlayerData *p = player->data;

	glob_player_center = EntCenter(player);
	glob_harpoon_pos = p->harpoon.position;

	// Manage physics
	PlayerPhysics(player, dt);

	// Update camera
	PlayerCameraControls(player, dt);

	// Upate player sprite angle
	player->sprite_angle = player->angle * RAD2DEG + 90;

	// Manage player state
	switch(p->state) {

		case PLR_IDLE:
			AnimPlay(idle_anim, dt);
			break;

		case PLR_SWIM:
			AnimPlay(swim_up_anim, dt);
			break;
			
		case PLR_JUMP:
			break;

		case PLR_FALL:
			break;

		case PLR_AIM:
			break;

		case PLR_SHOOT:
			break;

		case PLR_RECOIL:
			AnimPlay(recoil_anims[recoil_dir], dt);
			
			if(recoil_anims[recoil_dir]->cycles > 0) {
				AnimReset(recoil_anims[recoil_dir]);
				p->state = PLR_IDLE;
			} 

			break;

		case PLR_DEAD:
			break;
	}
}

// Render player
void PlayerDraw(Entity *player, SpriteLoader *sl) {
	PlayerData *p = PlayerFetchData(player);
	
	// Draw harpoon sprite and rope
	HarpoonDraw(player, p, &p->harpoon, sl);

	// Draw player sprite
	//DrawSpritePro(&sl->spr_pool[player->sprite_id], 0, player->position, player->sprite_angle, player->scale, 0);
	//AnimDrawPro(idle_anim, player->position, player->sprite_angle, player->scale, 0);

	switch(p->state) {

		case PLR_IDLE:
			AnimDrawPro(idle_anim, player->position, player->sprite_angle, player->scale, 0);
			break;

		case PLR_SWIM:
			AnimDrawPro(swim_up_anim, player->position, player->sprite_angle, player->scale, 0);
			break;
			
		case PLR_JUMP:
			break;

		case PLR_FALL:
			break;

		case PLR_AIM:
			break;

		case PLR_SHOOT:
			break;

		case PLR_RECOIL:
			//AnimDrawPro(recoil_anims[recoil_dir], player->position, player->sprite_angle, player->scale, 0);
			DrawSpritePro(recoil_anims[recoil_dir]->spritesheet, 15, player->position, player->sprite_angle, player->scale, 0);
			break;

		case PLR_DEAD:
			break;
	}
}

// Handle input polled from input.c
void PlayerInput(Entity *player, float dt) {
	PlayerData *p = PlayerFetchData(player);
	InputState *input = p->input;

	// horizontal input for player rotation
	if(input->move_x) {
		p->rotate_timer += dt;
		p->rotate_accel = Clamp(p->rotate_timer * 2, 1, 5);

		player->angle += (input->move_x * p->rotate_accel) * dt;
	} else {
		p->rotate_timer = 0;
		p->rotate_accel = Lerp(p->rotate_accel, 0, dt);
	} 

	if(input->move_y < 0) {
		Vector2 dir = GetForward(player->angle);
		dir = Vector2Normalize(dir);

		player->velocity = Vector2Add(player->velocity, Vector2Scale(dir, 5 * dt));

		if(p->state == PLR_IDLE && p->jetpack_timer <= 0) 
			p->state = PLR_SWIM;

	} else {

		if(p->state == PLR_SWIM)
			p->state = PLR_IDLE;
	}

	// Set jetpack active state
	jetpack_on = (p->jetpack_cooldown <= 0 && input->jetpack);

	// Get mouse cursor position in world space
	if(!p->input->use_gamepad) {
		p->cursor_pos = GetScreenToWorld2D(GetMousePosition(), *p->camera);

		Vector2 targ = GetDirectionNormalized(p->cursor_pos, EntCenter(player));
		//p->aim_dir = GetDirectionNormalized(p->cursor_pos, EntCenter(player));
		p->aim_dir = Vector2Lerp(p->aim_dir, targ, GetFrameTime() * 20);

	} else {
		Vector2 look_input = (Vector2){p->input->look_x, p->input->look_y};
		float a = atan2f(look_input.x, -look_input.y) + player->angle;

		Vector2 targ = (Vector2){cosf(a), sinf(a)};

		if(Vector2Length(look_input) < GP_CURSOR_DEADZONE) {
			targ = last_look;
		} else {
			last_look = targ;
		}

		p->aim_dir = Vector2Lerp(p->aim_dir, targ, GetFrameTime() * 20);

		p->cursor_pos = Vector2Add(EntCenter(player), Vector2Scale(p->aim_dir, 100));
	}

	// Manage input for harpoon
	HarpoonInput(player, p, &p->harpoon, dt);
}

// Handle player entity's physics
void PlayerPhysics(Entity *player, float dt) {
	PlayerData *p = PlayerFetchData(player);

	// Update player position
	EntUpdatePosition(player, dt);

	// Add velocity from jetpack
	PlayerApplyJetpack(player, p, dt, jetpack_on);		

	// Update harpoon and rope
	HarpoonUpdate(player, p, &p->harpoon, dt);

	player->velocity = Vector2ClampValue(player->velocity, -PLAYER_MAX_VEL, PLAYER_MAX_VEL);
}

// Update camera
void PlayerCameraControls(Entity *player, float dt) {
	PlayerData *p = PlayerFetchData(player);
	Camera2D *cam = p->camera;

	// Translate
	Vector2 player_center = EntCenter(player);
	cam->target = Vector2Lerp(cam->target, player_center, dt * 7.5f);

	// Rotate
	float rot_target = -player->angle * RAD2DEG - 90;
	cam->rotation = Lerp(cam->rotation, rot_target, dt * 5);

	//cam->zoom = CAM_ZOOM_DEFAULT;
	cam->zoom = Lerp(cam->zoom, zoom_targ, dt);

	// Apply screenshake
	if(screenshake > 0) {
		// Set random offset values
		short shake_x = GetRandomValue(-SCREENSHAKE_MAX, SCREENSHAKE_MAX) * screenshake;
		short shake_y = GetRandomValue(-SCREENSHAKE_MAX, SCREENSHAKE_MAX) * screenshake;

		// Set temporary camera position
		Vector2 offset = (Vector2) { shake_x, shake_y };
		cam->target = Vector2Add(cam->target, offset);
		
		// Decrement screenshake value
		screenshake -= dt;
	} 

	if(p->harpoon.state == HARPOON_AIM) {
		zoom_targ = CAM_ZOOM_FOCUSED;
	} else {
		float vel_mod = (Vector2Length(player->velocity)) * 0.005f;
		vel_mod = Clamp(vel_mod, 1, 1.25f);

		zoom_targ = CAM_ZOOM_DEFAULT / vel_mod;
	}
}

// Apply and manage velocity from jetback to player
void PlayerApplyJetpack(Entity *player, PlayerData *p, float dt, uint8_t on) {
	// Decrement cooldown timer, skip adding velocity, if inactive 
	if(!on) {
		p->jetpack_timer = 0;
		p->jetpack_accel = 0;
		p->jetpack_cooldown -= dt;

		return;
	}

	// Increment timer 
	p->jetpack_timer += dt;

	// Set cooldown if timer is over threshold
	if(p->jetpack_timer >= 5) {
		p->jetpack_cooldown = 1;

		return;
	} 	

	// Accumulate and clamp acceleration
	p->jetpack_accel += (500 / (p->jetpack_timer + EPSILON)) * dt;
	p->jetpack_accel = Clamp(p->jetpack_accel, 100, 1000);

	// Calculate direction
	Vector2 forward = (Vector2){ cosf(player->angle), sinf(player->angle) };
	forward = Vector2Normalize(forward);

	// Add velocity calculated by scaling direction by acceleration
	player->velocity = Vector2Add(player->velocity, Vector2Scale(forward, p->jetpack_accel * dt));
}

// Handle player entity's collision response to asteroids
void PlayerHandleBodyCollision(Entity *player, PlayerData *p, Entity *ent, float dt) {
	//if(ent->type != ENT_ASTEROID) return;

	// Set camera screenshake value
	screenshake = Vector2Length(player->velocity) * 0.0025f;
	screenshake = Clamp(screenshake, 0, SCREENSHAKE_MAX);

	SetVibrate(p->input, 2, 0.25f);

	// Calculate direction
	Vector2 to_ent = Vector2Subtract(EntCenter(ent), EntCenter(player));

	// Calculate penetration
	float pen = Vector2Length(to_ent) - (player->radius + ent->radius);

	// Normalize direction
	to_ent = Vector2Normalize(to_ent);

	// Push player out of collision bounds
	player->position = Vector2Subtract(player->position, Vector2Scale(to_ent, pen * 1.25f));

	// Invert and scale down player velocity 
	player->velocity = Vector2Scale(player->velocity, -0.75f);	

	if(p->harpoon.state == HARPOON_PULL) {
		p->harpoon.flags &= ~HARPOON_ACTIVE;
	}

	PlayEffect(ptr_player_handler->ap, 2);
}

// Handle player entity's collision response to asteroids
void PlayerHandleFishCollision(Entity *player, PlayerData *p, Entity *ent, float dt) {
	if(!(ent->flags & ENT_ACTIVE)) return;
	if(p->harpoon.state == HARPOON_REEL) return;

	FishData *f = ent->data;
	
	ent->flags &= ~ENT_ACTIVE;

	ptr_player_handler->fish_collected++;

	PlayEffect(ptr_player_handler->ap, 0);

	f->timer = GetRandomValue(20, 45);

	if(p->harpoon.hit_id == ent->id) {
		p->harpoon.state = HARPOON_RETRACT;
	} 
}

// Update harpoon and connected components
void HarpoonUpdate(Entity *player, PlayerData *p, Harpoon *h, float dt) {
	rope_grav_targ = Vector2Lerp(rope_grav_targ, Vector2Zero(), dt);
	
	// Skip update if inactive
	if(!(h->flags & HARPOON_ACTIVE)) return;

	// Update position
	h->position = Vector2Add(h->position, Vector2Scale(h->velocity, dt));	

	// Update rope
	if(h->state > HARPOON_AIM) RopeUpdate(p->rope, dt);

	switch(h->state) {
		case HARPOON_AIM:
			HarpoonAim(player, p, h, dt);
			break;

		case HARPOON_EXTEND:
			HarpoonExtend(player, p, h, dt);
			break;

		case HARPOON_RETRACT:
			HarpoonRetract(player, p, h, dt);
			break;

		case HARPOON_STUCK:
			HarpoonStuck(player, p, h, dt);
			break;

		case HARPOON_PULL:
			HarpoonPull(player, p, h, dt);
			break;

		case HARPOON_REEL:
			HarpoonReel(player, p, h, dt);
			break;
	}

	RopeUpdate(p->rope, dt);
}

// Draw harpoon and connected components
void HarpoonDraw(Entity *player, PlayerData *p, Harpoon *h, SpriteLoader *sl) {
	// Draw aim reticle
	if(h->state == HARPOON_AIM) {
		DrawCircleLinesV(p->cursor_pos, 15, RAYWHITE);
	}

	// Skip draw if inactive
	if(!(h->flags & HARPOON_ACTIVE)) return;

	if(h->state <= HARPOON_AIM) return; 

	// Draw rope segements
	RopeDraw(p->rope);
	
	// Draw harpoon sprite
	Vector2 center_offset = (Vector2){51 * 0.5f, 31 * 0.54};

	DrawSpritePro(&sl->spr_pool[SHEET_HARPOON], 0, Vector2Subtract(h->position, center_offset), h->angle * RAD2DEG, 1, 0);
}

// Handle input related to harpoon
void HarpoonInput(Entity *player, PlayerData *p, Harpoon *h, float dt) {
	if(!(h->flags & HARPOON_ACTIVE)) {
		if(h->state == HARPOON_AIM) {
			HarpoonAim(player, p, h, dt);
			return;
		}
		
		//if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) h->state = HARPOON_AIM;
		if(p->input->aim) h->state = HARPOON_AIM;

		return;
	}

	switch(h->state) {
		case HARPOON_EXTEND:
			if(p->input->retract) h->state = HARPOON_RETRACT;
			break;

		case HARPOON_RETRACT:
			break;

		case HARPOON_STUCK:
			//p->rope->start_id = temp_rope_start;
			p->rope->start_id = 0;

			if(IsKeyPressed(KEY_X)) {
				//player->velocity = Vector2Zero();
				PlayerFlingStart(player, p, h);
				h->flags ^= HARPOON_ACTIVE;

				break;
			}

			if(p->input->shoot) {
				Entity *hit_ent = &ptr_player_handler->ents[h->hit_id];
				if(hit_ent->type == ENT_FISH) h->state = HARPOON_REEL;
				else h->state = HARPOON_PULL;
			}

			break;

		case HARPOON_PULL:
			break;

		case HARPOON_REEL:
			break;
	}
}

// Handle harpoon collisions
void HarpoonCollision(Entity *player, PlayerData *p, Harpoon *h, float dt) {
	if(h->state == HARPOON_STUCK) return;

	if(Vector2Distance(h->position, EntCenter(player)) > p->camera->offset.x * 1.25f) {
		h->state = HARPOON_RETRACT;
		return;
	}

	Grid *grid = &ptr_player_handler->grid;

	uint16_t h_cell_x = h->position.x / grid->cell_size;
	uint16_t h_cell_y  = h->position.y / grid->cell_size;

	if(h_cell_x < 0 || h_cell_y < 0 || h_cell_x >= grid->row_count - 1 || h_cell_y >= grid->row_count - 1) return;

	uint16_t h_cell_id = (h_cell_x + h_cell_y * grid->row_count);
	Cell *player_cell = &grid->cells[h_cell_id];

	short dirs_x[] = { -1, 0, 1 };
	short dirs_y[] = { -1, 0, 1 };
	
	for(uint16_t r = 0; r < 3; r++) {
		for(uint16_t c = 0; c < 3; c++) {
			int16_t cell_x = h_cell_x + dirs_x[c];
			int16_t cell_y = h_cell_y + dirs_y[r];

			if(cell_x < 0 || cell_y < 0 || cell_x >= grid->row_count || cell_y >= grid->row_count) return;
			Cell *cell = &grid->cells[cell_x + cell_y * grid->row_count];

			for(uint16_t j = 0; j < cell->ent_count; j++) {
				Entity *ent = &ptr_player_handler->ents[cell->ids[j]];

				// Skip checks with inactive entities
				if(!(ent->flags & ENT_ACTIVE))	continue;

				// Skip checks with entities not marked as collision bodies
				if(ent->type == ENT_PLAYER) continue;

				// Scare the feesh
				if(ent->type == ENT_FISH) {
					FishData *fish_data = ent->data;

					bool do_spook = (
						fish_data->state != FISH_SPOOKED 				&&
						fish_data->spook_timer <= 0	 					&&
						Vector2Distance(EntCenter(ent), h->position) < 450
					);

					if(do_spook) {
						fish_data->state = FISH_SPOOKED;
						fish_data->timer = 1;
						fish_data->spook_timer = 5;

						Vector2 to_hook = GetDirectionNormalized(h->position, EntCenter(ent));
						Vector2 away_hook = (Vector2){-to_hook.y, to_hook.x};

						Vector2 to_player = GetDirectionNormalized(EntCenter(player), EntCenter(ent));

						short dir = GetRandomValue(-1, 1); 
						if(dir == 0) dir++;

						//fish_data->dir.x = dir;

						//Vector2 spook_vel = Vector2Normalize(Vector2Add(away_hook, Vector2Scale(to_player, -0.2f)));
						Vector2 fish_to_harpoon = GetDirectionNormalized(h->position, EntCenter(ent));

						//Vector2 spook_vel = Vector2Normalize(h->velocity);
						Vector2 spook_vel = Vector2Normalize(Vector2Add(away_hook, Vector2Scale(to_player, -0.2f)));
						spook_vel = Vector2Reflect(Vector2Normalize(spook_vel), Vector2Scale(fish_to_harpoon, -1));

						ent->velocity = Vector2Scale(spook_vel, 10 * dir);

						Vector2 dir_vec = Vector2Normalize(Vector2Scale(ent->velocity, dir));
						ent->sprite_angle = atan2f(-dir_vec.y, dir_vec.x);
					}
				}	
				
				Vector2 forward = Vector2Normalize(h->velocity);
				Vector2 ray_start = h->position;
				Vector2 ray_end = Vector2Add(ray_start, Vector2Scale(forward, 20));

				if(!CheckCollisionPointCircle(h->position, EntCenter(ent), ent->radius * 1.00f)) 
					continue; 

				Vector2 harpoon_tan = (Vector2){-h->velocity.y, h->velocity.x};
				harpoon_tan = Vector2Normalize(harpoon_tan);

				for(uint8_t k = ROPE_TAIL - 1; k > 1; k--) {
					float v = (1 - (k * 0.1f)) * sinf(GetTime() * GetTime());
					p->rope->nodes[k].pos_prev = Vector2Add(p->rope->nodes[k].pos_prev, Vector2Scale(harpoon_tan, v));
				}

				h->state = HARPOON_STUCK;

				p->rope->nodes[ROPE_TAIL].flags |= NODE_PINNED;

				h->hit_id = cell->ids[j];
				h->hit_pos = h->position;
				h->hit_angle = ent->angle;
				h->offset = Vector2Subtract(h->position, ent->position);

				screenshake = 0.5f;
				SetVibrate(p->input, 0.85f, 0.15f);
				
				if(ent->type == ENT_FISH) {
					h->position = Vector2Add(EntCenter(ent), Vector2Scale(h->offset, 0.5f));

					FishData *f = ent->data;
					f->state = FISH_CAUGHT;

					ent->velocity = Vector2Zero();
					//ent->velocity = Vector2Scale(h->velocity, 0.04f);

					screenshake += 0.1f;
				}

				h->velocity = Vector2Zero();
			}
		}
	}
}

void HarpoonAim(Entity *player, PlayerData *p, Harpoon *h, float dt) {
	// Slow time by decreasing time modifier smoothly
	*p->time_mod = Lerp(*p->time_mod, 0.15f, dt * 5);

	// Decrease player velocity
	player->velocity = Vector2Lerp(player->velocity, Vector2Scale(player->velocity, 0.85f), GetFrameTime() * 2.5f);

	if(p->input->shoot) { 
		HarpoonShoot(player, p, h);
		return;
	}

	if(p->input->aim) {
		h->state = HARPOON_NONE; 
		*p->time_mod = 1;
		return;
	}
}

void HarpoonShoot(Entity *player, PlayerData *p, Harpoon *h) {
	if(h->flags & HARPOON_ACTIVE) return;

	// Get desired shoot direction
	Vector2 direction = Vector2Subtract(p->cursor_pos, EntCenter(player));
	direction = Vector2Normalize(direction);

	//recoil_dir = (Vector2Rotate(direction, player->angle).x < 0) ? 0 : 1;
	recoil_dir = (GetWorldToScreen2D(p->cursor_pos, *p->camera).x < 1920 * 0.5f) ? 0 : 1;

	// Flag harpoon as active
	h->flags |= HARPOON_ACTIVE;

	screenshake = 0.5f;

	// Set node positions to player entity center
	for(uint8_t i = 0; i < ROPE_TAIL; i++) { 
		RopeNodeSetPos(&p->rope->nodes[i],  Vector2Add(EntCenter(player), Vector2Scale(direction, i * 1.2f)));
	}

	// Set initial segment length to 0
	p->rope->segment_dist = 1.0f;

	// Pin first and last nodes
	p->rope->nodes[0].flags |= NODE_PINNED;
	p->rope->nodes[ROPE_TAIL].flags |= NODE_PINNED;

	// Set harpoon's initial position and velocity 
	h->position = EntCenter(player);
	h->velocity = Vector2Scale(direction, 1000);

	// Set harpoon state to extend
	h->state = HARPOON_EXTEND;

	// Reset time modifer back to default
	*p->time_mod = 1.0f;

	// Apply recoil velocity to player entity
	player->velocity = Vector2Add(player->velocity, Vector2Scale(direction, RECOIL_AMOUNT * -1));

	p->state = PLR_RECOIL;

	// Reset recoil animations
	AnimReset(recoil_anims[0]);
	AnimReset(recoil_anims[1]);
}

void HarpoonExtend(Entity *player, PlayerData *p, Harpoon *h, float dt) {
	p->rope->start_id = 0;

	Vector2 harpoon_tan = (Vector2){-h->velocity.y, h->velocity.x};
	harpoon_tan = Vector2Normalize(harpoon_tan);

	for(uint8_t k = ROPE_TAIL - 64; k < ROPE_TAIL - 32; k++) {
		float v = 0.5f * sinf(GetTime() * GetTime());
		//p->rope->nodes[k].pos_curr = Vector2Add(p->rope->nodes[k].pos_curr, Vector2Scale(harpoon_tan, v));
		p->rope->nodes[k].pos_prev = Vector2Add(p->rope->nodes[k].pos_prev, Vector2Scale(harpoon_tan, -v));
	}

	/*
	if(p->rope->stretch >= p->rope->max_stretch && p->rope->segment_dist && p->rope->segment_dist >= 3.9f) {
		p->rope_timer -= dt;
		if(p->rope_timer < 0) {
			if(p->rope_timer < 0) h->state = HARPOON_RETRACT; 
			p->rope_timer = 1;
		}
	}
	*/

	p->rope->segment_dist = Lerp(p->rope->segment_dist, 4.0f, dt * 20);

	/*
	if(p->rope->segment_dist >= 3.9f && p->rope->stretch >= p->rope->max_stretch) {
		p->rope_timer -= dt;
		if(p->rope_timer < -1) {
			p->rope_timer = HARPOON_RETRACT;
			return;
		}
	}
	*/

	Vector2 dir = Vector2Normalize(h->velocity);
	h->angle = atan2f(dir.y, dir.x);

	h->velocity = (Vector2Add(h->velocity, Vector2Scale(dir, (Vector2Distance(EntCenter(player), h->position)) * -0.1f * dt)));
	if(Vector2Length(h->velocity) <= 10.99f) {
		h->state = HARPOON_RETRACT;
		return;
	}

	HarpoonCollision(player, p, h, dt);
}

void HarpoonRetract(Entity *player, PlayerData *p, Harpoon *h, float dt) {
	Rope *rope = p->rope;

	rope->nodes[ROPE_TAIL].flags &= ~NODE_PINNED;
	h->position = rope->nodes[ROPE_TAIL].pos_curr;

	p->rope->segment_dist *= (0.9999f * dt);

	//Vector2 to_node = Vector2Subtract(p->rope->nodes[ROPE_TAIL-1].pos_curr, h->position);
	//to_node = Vector2Normalize(to_node);
	//h->angle = atan2f(to_node.y, to_node.x);

	for(uint8_t i = 0; i < ROPE_LENGTH; i++) {
		RopeNode *node = &rope->nodes[i];
		
		Vector2 to_player = Vector2Normalize(Vector2Subtract(node->pos_curr, EntCenter(player))); 
		node->pos_prev = Vector2Add(node->pos_prev, Vector2Scale(to_player, dt));
	}

	float vel_mod = Vector2Length(player->velocity);
	vel_mod = Clamp(vel_mod, 1.0f, 3.0f);

	if(Vector2Distance(rope->nodes[ROPE_TAIL].pos_curr, EntCenter(player)) <= (player->radius * vel_mod) * 2) {
		h->flags &= ~HARPOON_ACTIVE;
		h->state = HARPOON_NONE;
	}
}

void HarpoonStuck(Entity *player, PlayerData *p, Harpoon *h, float dt) {
	if(h->hit_id < 0 || h->hit_id > ptr_player_handler->count) return;

	p->rope->segment_dist = 3.25f;
	p->rope_timer = 0;

	Entity *hit_ent = &ptr_player_handler->ents[h->hit_id];
	AsteroidData *a = hit_ent->data; 
	
	rope_grav_targ = Vector2Normalize(Vector2Subtract(EntCenter(hit_ent), EntCenter(player)));

	Vector2 harpoon_pos_prev = h->position;
	Vector2 pull_dir = Vector2Normalize(Vector2Subtract(p->rope->nodes[1].pos_prev, EntCenter(player)));

	float dot_min = 0;

	if(hit_ent->angle != h->hit_angle) {
		Vector2 offset = Vector2Subtract(h->hit_pos, EntCenter(hit_ent));
		float angle_diff = hit_ent->angle - h->hit_angle;

		h->angle = angle_diff;

		Vector2 rot_offset = (Vector2) {
			offset.x * cosf(angle_diff) - offset.y * sinf(angle_diff),
			offset.x * sinf(angle_diff) + offset.y * cosf(angle_diff)
		};

		Vector2 radial = Vector2Subtract(EntCenter(hit_ent), EntCenter(player));
		Vector2 tangent = (Vector2){-radial.y, radial.x};

		Vector2 new_harpoon_pos = Vector2Add(EntCenter(hit_ent), rot_offset);
		new_harpoon_pos = Vector2Add(new_harpoon_pos, Vector2Scale(Vector2Normalize(radial), -0.1f));
		h->position = new_harpoon_pos;

		Vector2 harpoon_dir = Vector2Subtract(h->position, harpoon_pos_prev);

		float angle_speed = hit_ent->angle_vel * Vector2Length(radial);
		Vector2 tan_vel = Vector2Scale(Vector2Normalize(tangent), angle_speed * 0.1f * dt);

		rope_grav_targ = Vector2Scale(radial, fabsf(hit_ent->angle_vel) * hit_ent->radius * 0.033f);
		rope_grav_targ = Vector2Lerp(rope_grav_targ, Vector2Add(rope_grav_targ, tan_vel), dt);

		//rope_grav_targ = Vector2Lerp(rope_grav_targ, Vector2Scale(Vector2Normalize(radial), -0.2f), dt);

		for(uint8_t i = ROPE_TAIL; i > 1; i--) {
			RopeNode *node = &p->rope->nodes[i];

			Vector2 r  = Vector2Subtract(node->pos_curr, EntCenter(hit_ent));
			Vector2 rn = Vector2Normalize(r); 
			Vector2 vp  = (Vector2){-rn.y, rn.x};

			//Vector2 node_vel = Vector2Scale(vp, (hit_ent->angle_vel * Vector2Length(r)) * dt);
			//node_vel = Vector2Add(node_vel, player->velocity);
			//node_vel = Vector2Scale(node_vel, Vector2Distance(EntCenter(player), node->pos_curr));

			Vector2 node_vel = (Vector2Scale(rn, (-10.0f * i) * Vector2Length(r)));
			node_vel = Vector2Add(node_vel, rot_offset);
				
			node->pos_curr = Vector2Subtract(node->pos_curr, Vector2Scale(node_vel, dt * dt));
		}

		player->velocity = Vector2Lerp(player->velocity, Vector2Zero(), dt * 0.001f);
		player->velocity = Vector2Add(player->velocity, Vector2Scale(tan_vel, dt * 1.0f));

		p->fling_vel = Vector2Scale(player->velocity, -1.0f);

		if(Vector2Length(player->velocity) > 5)
			p->fling_timer += (Vector2Length(tan_vel) * 0.1f) * dt;

		p->fling_timer = Clamp(p->fling_timer, 0, 1 + fabsf(hit_ent->angle_vel) * hit_ent->radius);
	}

	float pull_amount = fabs(p->rope->stretch - p->rope->max_stretch) * 10.0f;

	float dir_dot = Vector2DotProduct(pull_dir, Vector2Normalize(player->velocity));
	float harpoon_dist = Vector2Distance(EntCenter(player), h->position);

	p->rope->gravity = Vector2Lerp(p->rope->gravity, rope_grav_targ, dt * 2);

	bool pull_static = (
		dir_dot < 0.0f &&
		p->rope->stretch >= p->rope->max_stretch &&
		h->state != HARPOON_REEL  
	);

	bool pull_spin = (
		fabsf(hit_ent->angle_vel) > 0  
	);
	
	//if(pull_spin) pull_amount *= 0.7f;

	if(hit_ent->type == ENT_FISH)
		h->position = Vector2Add(EntCenter(hit_ent), Vector2Scale(h->offset, 0.5f));

	if(pull_static) {
		if(hit_ent->type == ENT_FISH) {
			pull_dir = Vector2Normalize(Vector2Subtract(EntCenter(hit_ent), EntCenter(player)));
			rope_grav_targ = pull_dir;
			hit_ent->velocity = Vector2Subtract(hit_ent->velocity, Vector2Scale(pull_dir, pull_amount * dt));

			pull_amount *= 0.1f;
			pull_amount = Clamp(pull_amount, 0, 10);
		} else 
			player->velocity = Vector2Add(player->velocity, Vector2Scale(pull_dir, pull_amount * dt));

		player->velocity = Vector2Add(player->velocity, Vector2Scale(pull_dir, pull_amount * dt));
	}

	p->rope->gravity = rope_grav_targ;
}

void HarpoonPull(Entity *player, PlayerData *p, Harpoon *h, float dt) {
	p->rope_timer -= dt;
	if(p->rope_timer < 0) {
		p->rope->start_id++;	
		p->rope->segment_dist *= (0.95f);
		p->rope_timer = 0.01f;
	}

	// Unpin start node
	p->rope->nodes[p->rope->start_id].flags &= ~NODE_PINNED;

	// Move player to start node
	Vector2 new_center = p->rope->nodes[p->rope->start_id].pos_curr; 
	Vector2 new_pos = Vector2Subtract(new_center, player->center_offset);
	player->position = Vector2Lerp(player->position, new_pos, dt * 10);
	
	Vector2 dir = Vector2Subtract(new_center, EntCenter(player));
	player->velocity = dir;
	
	if(Vector2Distance(EntCenter(player), p->harpoon.position) <= player->radius * 2) {
		p->harpoon.state = HARPOON_NONE;
		p->harpoon.flags ^= HARPOON_ACTIVE;
		return;
	}

	RopeUpdate(p->rope, dt);
}

void HarpoonReel(Entity *player, PlayerData *p, Harpoon *h, float dt) {
	Entity *fish = &ptr_player_handler->ents[h->hit_id];

	Vector2 ftop = Vector2Subtract(Vector2Add(EntCenter(player), Vector2Scale(player->velocity, dt)), EntCenter(fish));
	//Vector2 ftop = Vector2Subtract(p->rope->nodes[ROPE_TAIL - 1].pos_prev, EntCenter(fish));
	ftop = Vector2Normalize(ftop);

	Vector2 new_center = p->rope->nodes[ROPE_TAIL].pos_curr; 
	Vector2 new_pos = Vector2Subtract(new_center, fish->center_offset);

	p->rope->nodes[ROPE_TAIL].flags |= NODE_PINNED;

	fish->position = Vector2Lerp(fish->position, p->rope->nodes[ROPE_TAIL - 1].pos_curr, dt);

	fish->velocity = Vector2Add(fish->velocity, Vector2Scale(ftop, 5000 * dt));
	fish->velocity = Vector2Lerp(fish->velocity, Vector2Scale(ftop, 1), dt);

	h->position = Vector2Add(EntCenter(fish), Vector2Scale(h->offset, 0.5f));

	/*
	p->rope_timer -= dt;
	if(p->rope_timer < 0) {
	}
	*/

	p->rope->segment_dist = Lerp(p->rope->segment_dist, 0.1f, dt);

	if(Vector2Distance(EntCenter(fish), EntCenter(player)) <= player->radius * 1.6f + (Vector2Length(fish->velocity) * dt)) {
		h->flags &= ~HARPOON_ACTIVE;
		h->state = 0;

		screenshake = 0.1f;
		fish->flags &= ~ENT_ACTIVE;

		ptr_player_handler->fish_collected++;

		PlayEffect(ptr_player_handler->ap, 0);

		FishData *f = fish->data;
		f->timer = GetRandomValue(20, 45);
	}

	RopeUpdate(p->rope, dt);
}

void PlayerFlingStart(Entity *player, PlayerData *p, Harpoon *h) {
	Entity *ent = &ptr_player_handler->ents[h->hit_id];

	Vector2 pivot = EntCenter(ent);
	Vector2 point = EntCenter(player);

	Vector2 to_piv = Vector2Subtract(pivot, point);
	Vector2 radial = Vector2Normalize(to_piv);

	float rad_len = Vector2DotProduct(player->velocity, radial);
	Vector2 rad_vel = Vector2Scale(radial, rad_len);

	Vector2 tan_vel = Vector2Subtract(player->velocity, rad_vel);

	player->velocity = Vector2Scale(tan_vel, 2);
}

void PlayerDrawAimLine(Entity *player) {
	PlayerData *p = PlayerFetchData(player);

	if(p->harpoon.state != HARPOON_AIM) return;

	/*
	if(!p->input->use_gamepad)
		return;
	*/

	Vector2 line_start = EntCenter(player);
	Vector2 line_end = Vector2Add(line_start, Vector2Scale(p->aim_dir, 2000));

	if(!p->input->use_gamepad)
		line_end = p->cursor_pos;

	DrawLineEx(line_start, line_end, 4, ColorAlpha(RAYWHITE, 0.9f));
}

