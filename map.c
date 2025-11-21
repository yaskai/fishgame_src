#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "map.h"
#include "raylib.h"
#include "raymath.h"

void MapLoad(EntHandler *handler, char *path) {
	FILE *pf = fopen(path, "r");

	printf("map: attempting read...\n");

	if(!pf) {
		printf("map: could not load level from path: \n%s\n", path);
		return;
	}

	int16_t curr_id = 0;
	Entity *curr_ent = NULL;

	uint8_t type;

	printf("map: parsing lines...\n");

	char line[128];
	while(fgets(line, sizeof(line), pf)) {
		if(line[0] == '[') {
			curr_id++;

			char *name = line + 1;
			char *last = strchr(line, ']');
			*last = '\0';

			printf("map: adding entity, name: %s\n", name);

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

		} else MapParseLine(handler, curr_id, line);
	}

	fclose(pf);

	// Initialize grid
	for(uint16_t i = 0; i < handler->count; i++) {
		Entity *ent = &handler->ents[i];
		if(!(ent->flags & ENT_ACTIVE)) continue;

		uint16_t cell_x  = ent->position.x / handler->grid.cell_size;
		uint16_t cell_y  = ent->position.y / handler->grid.cell_size;
		uint16_t cell_id = (cell_x + cell_y * handler->grid.row_count);
		Cell *cell = &handler->grid.cells[cell_id];	
		
		cell->ids[cell->ent_count++] = i;
	}
}

void MapParseLine(EntHandler *handler, int16_t curr_ent, char *line) {
	char *sep = strchr(line, ':');
	if(!sep) return;

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
	}

	else if(!strcmp(key, "position")) {
		Vector2 position;
		sscanf(val, "%f, %f", &position.x, &position.y);
		ent->position = Vector2Add(position, (Vector2){2048, 2048});
		ent->start_pos = position;
	}
}

