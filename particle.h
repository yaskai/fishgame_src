#include <stdint.h>
#include "raylib.h"

#ifndef PARTICLE_H_
#define PARTICLE_H_

#define PARTICLES_CAPACITY	255

typedef struct {
	uint8_t id; 
	uint8_t type;
	uint8_t flags;

	float scale;
	float lifetime;

	Vector2 position;
	Vector2 velocity;
} Particle;

typedef struct {
	uint8_t count;
	Particle Particle[PARTICLES_CAPACITY];
} ParticleHandler;

void ParticlesInit(ParticleHandler *part_handler);
void ParticlesUpdate(ParticleHandler *part_handler, float dt);
void ParticlesDraw(ParticleHandler *part_handler);

#endif 
