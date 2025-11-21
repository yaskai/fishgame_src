#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "raylib.h"
#include "raymath.h"
#include "ent_handler.h"
#include "entity.h"
#include "map.h"
#include "sprites.h"

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

	handler->game_timer = 0.0f;
	handler->fish_collected = 0;

	handler->grid = (Grid){0};
	handler->grid.row_count = 255;
	handler->grid.cell_size = 768;
	handler->grid.cell_count = (handler->grid.row_count * handler->grid.row_count);
	handler->grid.cells = calloc(handler->grid.cell_count, sizeof(Cell));
}

// Update all entities
void EntHandlerUpdate(EntHandler *handler, float dt) {
	Entity *player_ent = &handler->ents[handler->player_id];
	PlayerData *p = player_ent->data;

	Grid *grid = &handler->grid;

	for(uint16_t i = 0; i < handler->count; i++) {
		// Get entity pointer
		Entity *ent = &handler->ents[i];

		// Skip update if not active
		if(!(ent->flags & ENT_ACTIVE)) {
			if(ent->type == ENT_FISH) {
				FishData *f = ent->data;
				f->timer -= dt;

				if(f->timer < 0) {
					ent->flags |= ENT_ACTIVE;
					ent->velocity = Vector2Zero();
					ent->position = Vector2Add(ent->start_pos, 
								(Vector2){GetRandomValue(-30, 30) ,GetRandomValue(-30, 30)});

					f->timer = 0;
					f->state = FISH_IDLE;
				}
				
			}

			continue;
		}

		if(!Vector2Equals(ent->prev_pos, ent->position)) {
			// Find origin cell
			int16_t src_x  = (uint16_t)(ent->prev_pos.x / grid->cell_size);
			int16_t src_y  = (uint16_t)(ent->prev_pos.y / grid->cell_size);
			int16_t src_id = (src_x + src_y * grid->row_count);
			Cell *cell_src  = &grid->cells[src_id];

			// Find destination cell
			int16_t dest_x  = (uint16_t)(ent->position.x / grid->cell_size);
			int16_t dest_y  = (uint16_t)(ent->position.y / grid->cell_size);
			int16_t dest_id = (dest_x + dest_y * grid->row_count);
			Cell *cell_dest  = &grid->cells[dest_id];

			if(src_id != dest_id) {
				// Remove entity from source cell
				for(uint8_t j = 0; j < cell_src->ent_count - 1; j++) {
					if(cell_src->ids[j] == ent->id) {
						for(uint8_t n = j; n < cell_src->ent_count; n++)
							cell_src->ids[n] = cell_src->ids[n + 1];  

						cell_src->ent_count--;
						break;
					}
				}

				cell_dest->ids[(cell_dest->ent_count++)] = ent->id;
			}
		}

		// Call entity's update function
		if(ent->update) ent->update(ent, dt * handler->time_mod);
		ent->prev_pos = ent->position;
	}

	// Find cell containing player entity
	player_cell_x = player_ent->position.x / grid->cell_size;
	player_cell_y = player_ent->position.y / grid->cell_size;
	int16_t player_cell_id = (player_cell_x + player_cell_y * grid->row_count);

	Cell *player_cell = &grid->cells[player_cell_id];
		
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

				if(ent->type == ENT_ASTEROID)
					if(CheckCollisionCircles(EntCenter(player_ent), player_ent->radius, EntCenter(ent), ent->radius * 1.25f))
						PlayerHandleBodyCollision(player_ent, p, ent, dt * handler->time_mod);
				
			}
		}
	}

	AnimPlay(&handler->sprite_loader->anims[1], dt);
}

// Draw all entities
void EntHandlerDraw(EntHandler *handler, uint8_t flags) {
	for(uint16_t i = 0; i < handler->count; i++) {
		if(i == handler->player_id) continue;

		// Get entity pointer
		Entity *ent = &handler->ents[i];
			
		// Skip draw call if entitiy inactive
		if(!(ent->flags & ENT_ACTIVE)) continue;
		
		// Call entity's draw function
		if(ent->draw) ent->draw(ent, handler->sprite_loader);

		//DrawText(TextFormat("%d", ent->type), ent->position.x, ent->position.y, 128, RAYWHITE);
	}

	Grid *grid = &handler->grid;
	uint16_t player_cell_id = (player_cell_x + player_cell_y * grid->row_count);
	Cell *player_cell = &grid->cells[player_cell_id];
		
	int8_t dir_x[] = { -2, -1, 0, 1, 2 };
	int8_t dir_y[] = { -2, -1, 0, 1, 2 };

	for(uint8_t c = 0; c < 5; c++) {
		for(uint8_t r = 0; r < 5; r++) {
			int16_t cell_x = player_cell_x + dir_x[c];
			int16_t cell_y = player_cell_y + dir_y[r];
			
			if(cell_x < 0 || cell_y < 0 || cell_x >= grid->row_count || cell_y >= grid->row_count) continue;

			Cell *cell = &grid->cells[cell_x + cell_y * grid->row_count];

			for(uint16_t j = 0; j < cell->ent_count; j++) {
				Entity *ent = &handler->ents[cell->ids[j]];

				if(!(ent->flags & ENT_ACTIVE)) continue;

				if(ent->draw) ent->draw(ent, handler->sprite_loader);
			}
		}
	}

	Entity *player_ent = &handler->ents[handler->player_id];	
	PlayerData *p = player_ent->data;
	player_ent->draw(player_ent, handler->sprite_loader);

	/*
	Grid *grid = &handler->grid;
	uint16_t player_cell_x 	= player_ent->position.x / grid->cell_size;
	uint16_t player_cell_y  = player_ent->position.y / grid->cell_size;
	uint16_t player_cell_id = (player_cell_x + player_cell_y * grid->row_count);
	Cell *player_cell = &grid->cells[player_cell_id];
	
	Rectangle rec = (Rectangle) {
		.x = player_cell_x * grid->cell_size,
		.y = player_cell_y * grid->cell_size,
		.width = grid->cell_size,
		.height = grid->cell_size
	};

	DrawRectangleLinesEx(rec, 2, SKYBLUE);
	*/
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

	ent->radius = handler->sprite_loader->spr_pool[1].frame_w * 0.5f * ent->scale;
	ent->center_offset = (Vector2){ent->radius / ent->scale, ent->radius / ent->scale};

	ent->flags |= ENT_IS_BODY;

	printf("reserved data for asteroid entity[%d]\n", data_id);
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

	ent->radius = handler->sprite_loader->spr_pool[2].frame_h * 0.5f * ent->scale;
	ent->center_offset = (Vector2){ent->radius / ent->scale, ent->radius / ent->scale};

	printf("reserved data for fish entity[%d]\n", data_id);
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

	printf("reserved data for npc entity[%d]\n", data_id);
}

// Spawn an asteroid entity at provided position
void AsteroidSpawn(EntHandler *handler, Vector2 position, float scale, float angle_vel) {
	int16_t id = EntMake(handler, ENT_ASTEROID);
	if(id == -1) return;

	Entity *ast = &handler->ents[id];
	ast->position = position;
	ast->type = ENT_ASTEROID;
	ast->flags |= ENT_IS_BODY;

	ast->angle_vel = angle_vel;

	ast->angle = GetRandomValue(0, 360) * DEG2RAD; 
	ast->scale = scale;

	ast->radius = handler->sprite_loader->spr_pool[1].frame_w * 0.5f * ast->scale;
	ast->center_offset = (Vector2){ast->radius / ast->scale, ast->radius / ast->scale};
}

void FishSpawn(EntHandler *handler, Vector2 position) {
	int16_t id = EntMake(handler, ENT_FISH);
	if(id == -1) return;
	
	Entity *fish = &handler->ents[id];

	fish->position = position;
	fish->scale = 1;

	fish->type = ENT_FISH;

	fish->radius = handler->sprite_loader->spr_pool[2].frame_w * 0.5f * fish->scale;
	fish->center_offset = (Vector2){fish->radius / fish->scale, fish->radius / fish->scale};
}

