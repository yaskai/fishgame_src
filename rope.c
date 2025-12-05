#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "rope.h"
#include "ent_handler.h"

#define GRAVITY (Vector2){0, 0}
#define dt2		(dt * dt)

bool allocated = false;

EntHandler *rope_handler_ptr = NULL;
void RopeSetHandler(EntHandler *handler) { rope_handler_ptr = handler; }

void RopeInit(Rope *rope, Vector2 pos) {
	rope->length = ROPE_LENGTH;
	rope->iterations = 12;
	rope->dampening = 0.825f;
	rope->segment_dist = 3.25f;
	rope->start_id = 0;
	rope->end_id = ROPE_TAIL;

	if(!allocated)
		rope->nodes = calloc(rope->length, sizeof(RopeNode));

	for(uint16_t i = 1; i < rope->length; i++) {
		RopeNode *node = &rope->nodes[i];
		node->pos_curr = pos;
		node->pos_prev = node->pos_curr;
		node->mass = 0.5f;
	}

	rope->nodes[0] = (RopeNode) {
		.flags = (NODE_PINNED),
		.pos_prev = pos,
		.pos_curr = pos,
		.mass = 1.0f
	};

	rope->nodes[rope->length-1] = (RopeNode) {
		.flags = (NODE_PINNED),
		.pos_prev = pos,
		.pos_curr = pos,
		.mass = 2.0f
	};
}

void RopeIntegrate(Rope *rope, float dt) {
	Vector2 mouse_pos = GetMousePosition();
	Vector2 mouse_delta = GetMouseDelta();

	for(uint16_t i = rope->start_id; i < rope->length; i++) {
		RopeNode *node = &rope->nodes[i];
		Vector2 new_prev = node->pos_curr;

		Vector2 vel = Vector2Subtract(node->pos_curr, node->pos_prev);	
		vel = Vector2Scale(vel, rope->dampening);
		Vector2 accel = Vector2Scale(rope->gravity, dt2);
		
		Vector2 vel_transfer = (Vector2){0, 0};
		if(i < rope->end_id) {
			vel_transfer = Vector2Scale(Vector2Subtract(rope->nodes[i+1].pos_curr, rope->nodes[i].pos_curr), 0.01f);
			rope->nodes[i+1].pos_prev = Vector2Add(rope->nodes[i+1].pos_prev, vel_transfer);
		}

		node->pos_curr = Vector2Add(Vector2Add(node->pos_curr, vel), accel);

		if(node->flags & NODE_PINNED) node->pos_curr = node->pos_prev;

		node->pos_prev = new_prev;
	}
}

void RopeSolveConstraints(Rope *rope, float dt) {
	rope->stretch = 0;
	rope->max_stretch = rope->segment_dist * (rope->end_id * 0.5f);

	for(uint16_t i = rope->start_id; i < rope->end_id; i++) {
		RopeNode *node_a = &rope->nodes[i];
		RopeNode *node_b = &rope->nodes[i + 1];

		Vector2 delta = Vector2Subtract(node_a->pos_curr, node_b->pos_curr); 	
		float dist = Vector2Length(delta);
		float correction = (dist - rope->segment_dist) / dist; 

		rope->stretch += dist;

		if(dist <= 0.1f) continue;

		Vector2 prev_a = node_a->pos_curr;
		Vector2 prev_b = node_b->pos_curr;

		Vector2 vel_transfer = Vector2Scale(Vector2Subtract(node_b->pos_curr, node_a->pos_curr), 0.15f);
		
		if((node_a->flags & NODE_PINNED) && (node_b->flags & NODE_PINNED)) {
			continue;

		} else if(node_a->flags & NODE_PINNED) {
			node_b->pos_curr = Vector2Add(node_b->pos_curr, Vector2Scale(delta, correction));	
			//node_b->pos_prev = Vector2Add(node_b->pos_prev, Vector2Scale(vel_transfer, 1));

			node_a->pos_curr = *rope->anchors[0];

			continue;

		} else if(node_b->flags & NODE_PINNED) {
			node_a->pos_curr = Vector2Add(node_b->pos_curr, Vector2Scale(delta, correction));	
			//node_a->pos_prev = Vector2Subtract(node_a->pos_prev, Vector2Scale(vel_transfer, 1));

			node_b->pos_curr = *rope->anchors[1];

			continue;
		}

		if(!Vector2Equals(prev_a, node_a->pos_curr) || !Vector2Equals(prev_b, node_b->pos_curr)) continue;

		node_a->pos_curr = Vector2Subtract(node_a->pos_curr, Vector2Scale(delta, 0.5f * correction * node_a->mass));
		node_b->pos_curr = Vector2Add(node_b->pos_curr, Vector2Scale(delta, 0.5f * correction * node_b->mass));
	}
}

void RopeCollision(Rope *rope, float dt) {
	if(rope_handler_ptr == NULL) return;
	EntHandler *handler = rope_handler_ptr;

	rope->coll_count = 0;

	Grid *grid = &handler->grid;

	Entity *player_ent = &handler->ents[handler->player_id];
	PlayerData *player_data = PlayerFetchData(player_ent);	

	int16_t harpoon_cell_x = player_data->harpoon.position.x / grid->cell_size;
	int16_t harpoon_cell_y = player_data->harpoon.position.y / grid->cell_size;
	int16_t harpoon_cell_id = (harpoon_cell_x + harpoon_cell_y * grid->row_count);

	if(harpoon_cell_x < 0 || harpoon_cell_y < 0 || harpoon_cell_x >= grid->row_count - 1 || harpoon_cell_y >= grid->row_count - 1) return;

	Cell cells[2] = { grid->cells[handler->player_cell], grid->cells[harpoon_cell_id] };

	for(uint16_t i = 0; i < ROPE_TAIL; i++) {
		RopeNode *node = &rope->nodes[i];

		// Skip collision checks on pinned nodes
		if(node->flags & NODE_PINNED) continue;
		
		// Check for collisions with entities within nearby cells
		for(uint8_t j = 0; j < 2; j++) {
			Cell *cell = &cells[j];

			for(uint8_t k = 0; k < cell->ent_count; k++) {
				Entity *ent = &handler->ents[cell->ids[k]];

				// Skip inactive entities
				if(!(ent->flags & ENT_ACTIVE)) continue;

				// Skip entities not marked as collision bodies
				if(!(ent->flags & ENT_IS_BODY)) continue;
				
				Vector2 center = EntCenter(ent);
				Vector2 to_node = Vector2Subtract(node->pos_curr, center);
				float dist = Vector2Length(to_node);

				if(dist <= ent->radius && dist > 0) {
					Vector2 norm = Vector2Normalize(to_node);

					float depth = ent->radius - dist;

					node->pos_curr = Vector2Add(center, Vector2Scale(norm, ent->radius * 1.01f));

					Vector2 vel = Vector2Subtract(node->pos_curr, node->pos_prev);

					float vel_to_body = Vector2DotProduct(vel, norm);
					if(vel_to_body < 0) 
						vel = Vector2Subtract(vel, Vector2Scale(norm, vel_to_body * 1.25f));

					vel = Vector2Scale(vel, -0.01f);
					node->pos_prev = Vector2Subtract(node->pos_curr, vel);

					rope->coll_count++;
				}
			}
		}
	}
}

void RopeDiffuse(Rope *rope, float dt) {
}

void RopeUpdate(Rope *rope, float dt) {
	rope->bounce_timer -= dt;

	for(uint8_t i = 0; i < rope->iterations; i++) {
		RopeIntegrate(rope, dt);
		RopeCollision(rope, dt);

		for(uint8_t j = 0; j < 5; j++) { 
			RopeSolveConstraints(rope, dt);
		}
	}
}

void RopeDraw(Rope *rope) {
	for(uint16_t i = rope->start_id; i < rope->end_id; i++) {
		if(rope->nodes[i].flags & NODE_SKIP_DRAW) continue;

		Vector2 p0 = rope->nodes[i].pos_curr;
		Vector2 p1 = rope->nodes[i + 1].pos_curr;

		Vector2 diff = Vector2Subtract(p1, p0);
		float dist = Vector2Length(diff);

		float alpha = ((float)(i + 1) / 255) * 2.0f;
		if(alpha < 0.35f) alpha = 0.35f;
		
		DrawCircleV(p0, 6, ColorAlpha(SKYBLUE, alpha));
		DrawLineEx(p0, p1, 12, ColorAlpha(SKYBLUE, alpha));	
	}
}

void RopeClose(Rope *rope) {
	free(rope->nodes);
}

void RopeNodeSetPos(RopeNode *node, Vector2 pos) {
	node->pos_curr = pos;
	node->pos_prev = pos;
}

