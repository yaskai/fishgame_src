#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "raylib.h"
#include "raymath.h"
#include "ent_handler.h"
#include "map.h"
#include "entity.h"
#include "sprites.h"
#include "config.h"

uint16_t type_max[] = {
	MAX_PLAYERS,
	MAX_FISH,
	MAX_NPCS,
	MAX_ASTEROIDS,
	MAX_ITEMS
};

Vector2 ray_start;
Vector2 ray_end;
Vector2 ray_coll_point;

int16_t player_cell_x;
int16_t player_cell_y;

// Entity reservation function prototype and array 
typedef void(*ReserveDataFunc)(EntHandler *handler, Entity *ent);
ReserveDataFunc data_reserve_funcs[] = {
	&ReserveDataPlayer,
	&ReserveDataFish,
	&ReserveDataNpc,
	&ReserveDataAsteroid,  
	NULL
};

// Entity update function prototype and array 
typedef void(*EntUpdateFunc)(Entity *ent, float dt);
EntUpdateFunc ent_update_funcs[] = { 
	&PlayerUpdate,
	&FishUpdate,
	NULL,
	&AsteroidUpdate,
	NULL
};

// Entity draw function prototype and array 
typedef void(*EntDrawFunc)(Entity *ent, SpriteLoader *sl);
EntDrawFunc ent_draw_funcs[] = { 
	&PlayerDraw,
	&FishDraw,
	NULL,
	&AsteroidDraw,
	NULL
};

// Initialize entity handler 
void EntHandlerInit(EntHandler *handler, SpriteLoader *sprite_loader, Camera2D *camera) {
	handler->sprite_loader = sprite_loader;
	handler->camera = camera;

	handler->time_mod = 1.0f;

	handler->ents = calloc(ENT_ARENA_CAP, sizeof(Entity));

	handler->player_data 	= calloc(MAX_PLAYERS, sizeof(PlayerData));
	handler->fish_data 		= calloc(MAX_FISH, sizeof(FishData));
	handler->npc_data 		= calloc(MAX_NPCS, sizeof(NpcData));
	handler->asteroid_data 	= calloc(MAX_ASTEROIDS, sizeof(AsteroidData));
	handler->item_data 		= calloc(MAX_ITEMS, sizeof(ItemData));

	handler->game_timer = 180.0f;
	handler->fish_collected = 0;

	handler->grid = (Grid){0};
	handler->grid.row_count = 128;
	handler->grid.cell_size = 1024;
	handler->grid.cell_count = (handler->grid.row_count * handler->grid.row_count);
	handler->grid.cells = calloc(handler->grid.cell_count, sizeof(Cell));
}

// Update all entities
void EntHandlerUpdate(EntHandler *handler, float dt) {
	Entity *player_ent = &handler->ents[handler->player_id];
	PlayerData *p = player_ent->data;
	
	if(IsKeyPressed(KEY_R)) {
		player_ent->position = player_ent->start_pos;
		player_ent->velocity = Vector2Zero();
		p->harpoon.flags &= ~HARPOON_ACTIVE;
	}

	Grid *grid = &handler->grid;

	for(uint16_t i = 0; i < handler->count; i++) {
		// Get entity pointer
		Entity *ent = &handler->ents[i];

		// Skip update if not active
		if(!(ent->flags & ENT_ACTIVE)) {
			// Fish respawn behavior
			if(ent->type == ENT_FISH) {
				FishData *fish_data = ent->data;
				fish_data->timer -= dt;

				if(fish_data->timer < 0) {
					ent->flags |= ENT_ACTIVE;
					ent->velocity = Vector2Zero();

					ent->position = Vector2Add(
						ent->start_pos,  (Vector2){GetRandomValue(-30, 30) ,GetRandomValue(-30, 30)});

					fish_data->timer = 0;
					fish_data->state = FISH_IDLE;
				}
			}

			continue;
		}

		// Update spatial partitioning
		GridUpdate(handler, ent);

		// Save entitiy's previous position for grid update on next frame
		ent->prev_pos = ent->position;
		//ent->prev_pos = EntCenter(ent);

		// Call entity's update function
		if(ent->update) ent->update(ent, dt * handler->time_mod);
	}

	// Find cell containing player entity
	player_cell_x = player_ent->position.x / grid->cell_size;
	player_cell_y = player_ent->position.y / grid->cell_size;
	handler->player_cell = (player_cell_x + player_cell_y * grid->row_count);

	Cell *player_cell = &grid->cells[handler->player_cell];
		
	int8_t dir_x[] = { -1, 0, 1 };
	int8_t dir_y[] = { -1, 0, 1 };

	for(uint8_t c = 0; c < 3; c++) {
		for(uint8_t r = 0; r < 3; r++) {
			int16_t cell_x = player_cell_x + dir_x[c];
			int16_t cell_y = player_cell_y + dir_y[r];
			
			if(cell_x < 0 || cell_y < 0 || cell_x >= grid->row_count || cell_y >= grid->row_count) continue;

			//printf("checking cell[%d, %d] for collisions\n", cell_x, cell_y);

			Cell *cell = &grid->cells[cell_x + cell_y * grid->row_count];

			for(uint16_t j = 0; j < cell->ent_count; j++) {
				Entity *ent = &handler->ents[cell->ids[j]];

				if(ent->type == ENT_ASTEROID) {
					if(CheckCollisionCircles(EntCenter(player_ent), player_ent->radius, EntCenter(ent), ent->radius * 1.05f))
						PlayerHandleBodyCollision(player_ent, p, ent, dt * handler->time_mod);

				}

				if(ent->type == ENT_FISH) {
					if(CheckCollisionCircles(EntCenter(player_ent), player_ent->radius, EntCenter(ent), ent->radius * 1.25f)) {
						PlayerHandleFishCollision(player_ent, p, ent, dt * handler->time_mod);
					}
				}
				
			}
		}
	}

	AnimPlay(&handler->sprite_loader->anims[1], dt);
}

// Draw all entities
void EntHandlerDraw(EntHandler *handler, uint8_t flags) {
	Entity *player_ent = &handler->ents[handler->player_id];	

	/*
	for(uint16_t i = 0; i < handler->count; i++) {
		//if(i == handler->player_id) continue;

		// Get entity pointer
		Entity *ent = &handler->ents[i];
			
		// Skip draw call if entitiy inactive
		//if(!(ent->flags & ENT_ACTIVE)) continue;
		
		// Call entity's draw function
		//if(ent->draw) ent->draw(ent, handler->sprite_loader);

		//DrawText(TextFormat("%d", ent->type), ent->position.x, ent->position.y, 128, RAYWHITE);
		if(ent->type != ENT_FISH) continue;
		if(!(ent->flags & ENT_ACTIVE)) continue;

		FishDraw(ent, handler->sprite_loader);
		DrawLine(EntCenter(ent).x, EntCenter(ent).y, EntCenter(player_ent).x, EntCenter(player_ent).y, GREEN);
	}
	*/

	Grid *grid = &handler->grid;
	uint16_t player_cell_id = (player_cell_x + player_cell_y * grid->row_count);
	Cell *player_cell = &grid->cells[player_cell_id];
		
	int8_t dir_x[] = { -2, -1, 0, 1, 2 };
	int8_t dir_y[] = { -2, -1, 0, 1, 2 };

	for(uint8_t c = 0; c < 5; c++) {
		for(uint8_t r = 0; r < 5; r++) {
			int16_t cell_x = player_cell_x + dir_x[c];
			int16_t cell_y = player_cell_y + dir_y[r];
			
			if(cell_x < 0 || cell_y < 0 || cell_x >= grid->row_count - 1 || cell_y >= grid->row_count - 1) continue;
			Cell *cell = &grid->cells[cell_x + cell_y * grid->row_count];

			if((handler->debug_flags & SHOW_GRID) != 0) {
				Rectangle rec = (Rectangle) {
					.x = cell_x * grid->cell_size,
					.y = cell_y * grid->cell_size,
					.width = grid->cell_size,
					.height = grid->cell_size
				};

				DrawRectangleLinesEx(rec, 2, SKYBLUE);
				DrawText(TextFormat("count: %d", cell->ent_count), rec.x, rec.y, 32, SKYBLUE);
			}

			for(uint16_t j = 0; j < cell->ent_count; j++) {
				Entity *ent = &handler->ents[cell->ids[j]];

				if(!(ent->flags & ENT_ACTIVE)) continue;
				
				//if(ent->type == ENT_FISH) continue;
				if(ent->draw) ent->draw(ent, handler->sprite_loader);

				if(handler->debug_flags & SHOW_COLLIDERS) {
					//DrawCircleV(EntCenter(ent), 16, RED);

					DrawCircleLinesV(EntCenter(ent), ent->radius, GREEN);
				}
			}
		}
	}

	PlayerData *p = player_ent->data;
	player_ent->draw(player_ent, handler->sprite_loader);
}

void EntHandlerClose(EntHandler *handler) {
	free(handler->ents);
	free(handler->grid.cells);
}

void EntHandlerClear(EntHandler *handler) {
	handler->count = 0;

	for(uint8_t i = 0; i < ENT_TYPE_COUNT; i++) {
		handler->type_counts[i] = 0;
	}
}

// Create a new entity and add to pool (corresponding to entity type)
int16_t EntMake(EntHandler *handler, uint8_t type) {
	// Get count for entity type
	uint16_t *type_count = &handler->type_counts[type];
	
	// Dont't add entity data if slots are full
	if(*type_count >= type_max[type]) return -1;
	
	// Get entity pointer
	Entity *ent = &handler->ents[handler->count]; 
	//ent->type = type;

	// Initialize entity
	*ent = (Entity){ .type = type };
	ent->flags |= ENT_ACTIVE;

	// Set function pointers
	ent->update = ent_update_funcs[type];
	ent->draw = ent_draw_funcs[type];
	
	// Reserve data
	data_reserve_funcs[type](handler, &handler->ents[handler->count]);
	
	// Increment count for entity type
	(*type_count)++;

	// Return entity's index 
	ent->id = handler->count++;
	return ent->id;
}

// Reserve data for entity of type "player"
void ReserveDataPlayer(EntHandler *handler, Entity *ent) {
	// Get data index
	uint16_t data_id = handler->type_counts[ENT_PLAYER];

	// Init data
	PlayerData player_data = (PlayerData){0};
	handler->player_data[data_id] = player_data;

	// Set entity data pointer
	ent->data = &handler->player_data[data_id];
	PlayerInit(ent, handler->sprite_loader, handler->camera);

	PlayerData *p = ent->data;
	p->time_mod = &handler->time_mod;

	ent->scale = 1;

	printf("reserved data for player entity[%d]\n", data_id);
}

// Reserve data for entity of type "asteroid"
void ReserveDataAsteroid(EntHandler *handler, Entity *ent) {
	// Get data index
	uint16_t data_id = handler->type_counts[ENT_ASTEROID];

	// Init data
	AsteroidData data = (AsteroidData){0};
	handler->asteroid_data[data_id] = data;

	// Set entity data pointers
	ent->data = &handler->asteroid_data[data_id];
	ent->scale = 1;

	//ent->radius = handler->sprite_loader->spr_pool[1].frame_w * 0.5f * ent->scale;
	ent->radius = (handler->sprite_loader->spr_pool[SHEET_ASTEROIDS].frame_w * ent->scale) * 0.5f;
	ent->center_offset = (Vector2){ent->radius / ent->scale, ent->radius / ent->scale};

	ent->flags |= ENT_IS_BODY;

	//printf("reserved data for asteroid entity[%d]\n", data_id);
}

// Reserve data for entity of type "fish"
void ReserveDataFish(EntHandler *handler, Entity *ent) {
	// Get data index
	uint16_t data_id = handler->type_counts[ENT_FISH];

	// Init data
	FishData data = (FishData){0};
	handler->fish_data[data_id] = data;

	// Set entity data pointer
	ent->data = &handler->fish_data[data_id];

	ent->scale = 1;

	ent->radius = handler->sprite_loader->spr_pool[2].frame_w * 0.5f * ent->scale;

	ent->center_offset = (Vector2){
		handler->sprite_loader->spr_pool[2].frame_w * ent->scale * 0.5f,
		handler->sprite_loader->spr_pool[2].frame_h * ent->scale * 0.5f,
	};

	//printf("reserved data for fish entity[%d]\n", data_id);
}

// Reserve data for entity of type "npc"
void ReserveDataNpc(EntHandler *handler, Entity *ent) {
	// Get data index
	uint16_t data_id = handler->type_counts[ENT_NPC];

	// Init data
	NpcData data = (NpcData){0};
	handler->npc_data[data_id] = data;

	// Set entity data pointer
	ent->data = &handler->fish_data[data_id];

	//printf("reserved data for npc entity[%d]\n", data_id);
}

