#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "map.h"
#include "raylib.h"
#include "raymath.h"

void MapLoad(EntHandler *handler, char *path) {
	puts("Loading map...");

	FILE *pf = fopen(path, "r");

	if(!pf) {
		printf("\e[0;31mERROR: map, could not load level from path: \e[0;37m \n%s\n", path);
		return;
	}

	int16_t curr_id = 0;
	Entity *curr_ent = NULL;

	uint8_t type;

	char line[128];
	while(fgets(line, sizeof(line), pf)) {
		if(line[0] == '[') {
			curr_id++;

			char *name = line + 1;
			char *last = strchr(line, ']');
			*last = '\0';

			//printf("map: adding entity, name: %s\n", name);

			if(!strcmp(name, "player")) {
				curr_id = EntMake(handler, ENT_PLAYER);
				handler->player_id = curr_id;

				type = ENT_PLAYER;

			} else if(!strcmp(name, "asteroid")) {
				curr_id = EntMake(handler, ENT_ASTEROID);

				type = ENT_ASTEROID;

			} else if(!strcmp(name, "spawner_fish")) {
				curr_id = EntMake(handler, ENT_FISH);
				type = ENT_FISH;
			}

			curr_ent = &handler->ents[curr_id];
			curr_ent->type = type;

		} else 
			MapParseLine(handler, curr_id, line);
	}

	fclose(pf);

	// Initialize grid
	for(uint16_t i = 0; i < handler->count; i++) {
		Entity *ent = &handler->ents[i];
		if(!(ent->flags & ENT_ACTIVE)) continue;

		uint16_t cell_x  = floor(ent->position.x / handler->grid.cell_size);
		uint16_t cell_y  = floor(ent->position.y / handler->grid.cell_size);
		uint16_t cell_id = (cell_x + cell_y * handler->grid.row_count);

		if(cell_id < 0 || cell_id > handler->grid.cell_count - 1) 
			continue;

		Cell *cell = &handler->grid.cells[cell_id];	
		cell->ids[cell->ent_count++] = i;
	}
}

void MapParseLine(EntHandler *handler, int16_t curr_ent, char *line) {
	char *sep = strchr(line, ':');
	if(!sep) return;

	if(line[0] == '#') return;

	*sep = '\0';
	char *key = line;
	char *val = sep + 1;

	Entity *ent = &handler->ents[curr_ent];

	if(!strcmp(key, "rotation")) {
		float angle;
		sscanf(val, "%f", &angle);
		ent->angle = angle;
	} 

	else if(!strcmp(key, "scale")) {
		float scale;
		sscanf(val, "%f", &scale);
		ent->scale = scale;
		ent->radius *= ent->scale;
	}

	else if(!strcmp(key, "position")) {
		Vector2 position;
		sscanf(val, "%f, %f", &position.x, &position.y);
		//ent->position = Vector2Add(position, (Vector2){2048, 2048});
		ent->position = position;
		ent->start_pos = ent->position;
		ent->prev_pos = ent->position;
	}
}

void GridUpdate(EntHandler *handler, Entity *ent) {
	// Skip entities that haven't moved
	if(Vector2Equals(ent->prev_pos, ent->position)) return;

	Grid *grid = &handler->grid;
	
	// Find origin cell
	int16_t src_x  = (int16_t)(floor(ent->prev_pos.x / grid->cell_size));
	int16_t src_y  = (int16_t)(floor(ent->prev_pos.y / grid->cell_size));
	int16_t src_id = (src_x + src_y * grid->row_count);

	// Find destination cell
	int16_t dest_x  = (uint16_t)(floor(ent->position.x / grid->cell_size));
	int16_t dest_y  = (uint16_t)(floor(ent->position.y / grid->cell_size));
	int16_t dest_id = (dest_x + dest_y * grid->row_count);

	// Skip update if out of bounds
	if(dest_x < 0 || dest_y < 0 || dest_x >= grid->row_count - 1 || dest_y >= grid->row_count - 1) return;

	Cell *cell_src  = &grid->cells[src_id];
	Cell *cell_dest  = &grid->cells[dest_id];

	// Skip entities that have not changed cells
	if(src_id == dest_id) return;	

	// Remove entity from source cell
	for(uint8_t j = cell_src->ent_count - 1; j > 0; j--) {
		uint16_t id = cell_src->ids[j];

		if(ent->id == id) {
			// Shift array indices one place starting from removed index
			for(uint8_t n = j; n < cell_src->ent_count - 1; n++)
				cell_src->ids[n] = cell_src->ids[n + 1];  

			// Decrement count
			cell_src->ent_count--;

			break;
		}
	}

	// Add entity to destination cell
	if(cell_dest->ent_count - 1 >= handler->cell_ent_cap) return;
	cell_dest->ids[cell_dest->ent_count++] = ent->id;
}

