#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

#include "audioplayer.h"

void AudioPlayerInit(AudioPlayer *ap) {
	char *effects_prefix = "resources/audio/effects"; 
	FilePathList path_effects = LoadDirectoryFiles(effects_prefix);

	ap->sfx = malloc(sizeof(SoundEffect) * path_effects.count);

	for(uint16_t i = 0; i < path_effects.count; i++) 
		ap->sfx[i] = LoadSoundEffect(0, path_effects.paths[i]);

	char *tracks_prefix = "resources/audio/tracks"; 
	FilePathList path_tracks  = LoadDirectoryFiles(tracks_prefix);

	ap->tracks = malloc(sizeof(Track) * path_tracks.count);

	for(uint16_t i = 0; i < path_tracks.count; i++)
		ap->tracks[i] = LoadTrack(0, path_tracks.paths[i]);
}

void AudioPlayerClose(AudioPlayer *ap) {
	for(uint8_t i = 0; i < ap->sfx_count; i++)
		if(ap->sfx[i].flags & AP_FILE_LOADED) {
			UnloadSound(ap->sfx[i].sound); 
			printf("audioplayer: unloaded effect[%d]\n", i);
		}

	for(uint8_t i = 0; i < ap->track_count; i++) 	
		if(ap->tracks[i].flags & AP_FILE_LOADED) {
			UnloadMusicStream(ap->tracks[i].music);
			printf("audioplayer: unloaded track[%d]\n", i);
		}
}

SoundEffect LoadSoundEffect(uint8_t flags, char *file_name) {
	SoundEffect effect;
	effect.flags = flags;

	effect.sound = LoadSound(TextFormat("%s", file_name));
	effect.flags |= AP_FILE_LOADED;

	printf("audioplayer: effect %s loaded\n", file_name);

	return effect;
}

Track LoadTrack(uint8_t flags, char *file_name) {
	Track track;
	track.flags = flags;

	track.music = LoadMusicStream(TextFormat("%s", file_name));
	track.flags |= AP_FILE_LOADED;

	printf("audioplayer: track %s loaded\n", file_name);

	return track;
}

void PlayEffect(AudioPlayer *ap, uint8_t id) {
	SetSoundPitch(ap->sfx[id].sound, GetRandomValue(-3, 1));
	PlaySound(ap->sfx[id].sound);
}

void PlayTrack(AudioPlayer *ap, uint8_t id) {
	if(!IsMusicStreamPlaying(ap->tracks[id].music)) PlayMusicStream(ap->tracks[id].music);
	UpdateMusicStream(ap->tracks[id].music);
}

void StopEffect(AudioPlayer *ap, uint8_t id) {
	if(IsSoundPlaying(ap->sfx[id].sound)) StopSound(ap->sfx[id].sound);
}

void StopTrack(AudioPlayer *ap, uint8_t id) {
	if(IsMusicStreamPlaying(ap->tracks[id].music)) StopMusicStream(ap->tracks[id].music);
}

