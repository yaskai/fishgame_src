#include "raylib.h"

#ifndef AUDIOPLAYER_H_
#define AUDIOPLAYER_H_

#include <stdint.h>

#define SFX_MAX 	32
#define TRACK_MAX	16

#define AP_FILE_LOADED		0x01

enum SFX_ID : uint8_t {
	effect_1,
	effect_2,
	effect_3,
	effect_4,
	effect_5,
	effect_6,
};

enum TRACK_ID : uint8_t {
	track_1,
	track_2,
	track_3,
	track_4,
	track_5,
	track_6,
};

typedef struct {
	uint8_t flags;
	Sound sound;
} SoundEffect;

typedef struct {
	uint8_t flags;
	Music music;
} Track;

typedef struct {
	uint8_t sfx_count;
	uint8_t track_count;

	//SoundEffect sfx[SFX_MAX];
	//Track tracks[TRACK_MAX];
	
	SoundEffect *sfx;
	Track *tracks;

} AudioPlayer;

void AudioPlayerInit(AudioPlayer *ap);
void AudioPlayerClose(AudioPlayer *ap);

SoundEffect LoadSoundEffect(uint8_t flags, char *file_path);
Track LoadTrack(uint8_t flags, char *file_path);

void EffectSetPos(AudioPlayer *ap, uint8_t id, Vector3 position);
void TrackSetPos(AudioPlayer *ap, uint8_t id, Vector3 position);

void PlayEffect(AudioPlayer *ap, uint8_t id);
void PlayTrack(AudioPlayer *ap, uint8_t id);

void StopEffect(AudioPlayer *ap, uint8_t id);
void StopTrack(AudioPlayer *ap, uint8_t id);

#endif // !AUDIOPLAYER_H_
