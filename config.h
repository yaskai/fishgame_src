#include <stdint.h>

#ifndef CONFIG_H_
#define CONFIG_H_

// Configuration defaults, revert to these values if config file not found
// Default window width, height and refresh rate
#define CONFIG_DEFAULT_WW	1920
#define CONFIG_DEFAULT_WH	1080
#define CONFIG_DEFAULT_RR	  60

#define AUTO "auto"
#define streq(a, b) (strcmp((a), (b)) == 0)

#define SHOW_GRID		0x01
#define SHOW_COLLIDERS	0x02

#define PLATFORM_LINUX	0
#define PLATFORM_WIN64	1
#define PLATFORM_MACOS	2
#define TARGET_PLATFORM (PLATFORM_LINUX)

typedef struct {
	uint8_t debug_flags;

	unsigned int window_width;
	unsigned int window_height;

	float refresh_rate;

	char level_path[128];
} Config;

void ConfigRead(Config *conf, char *path);
void ConfigParseLine(Config *conf, char *line);
void ConfigSetDefault(Config *conf);
void ConfigPrintValues(Config *conf);

#endif // !CONFIG_H_
