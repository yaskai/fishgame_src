#include "raylib.h"
#include "ent_handler.h"

#ifndef MAP_H_
#define MAP_H_

void MapLoad(EntHandler *handler, char *path);
void MapParseLine(EntHandler *handler, int16_t curr_ent, char *line);

void GridUpdate(EntHandler *handler, Entity *ent);

#endif
