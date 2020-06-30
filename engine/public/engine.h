#pragma once

typedef struct _Level_header
{
	uint32_t magick;
	uint32_t version;
	uint32_t meshes;
	uint32_t scene;
	uint32_t cdata;
	uint32_t entities;
}Level_header;

typedef struct _Input
{
	uint32_t buttons;
	float lt, rt;
	float lx, ly;
	float rx, ry;
}Input;

typedef struct _Game
{
	const char *title;
	int (*init) (int argc, char **argv);
	void (*frame) (float);
	void (*shutdown) (void);
}Game;

void engine_input (Input *, uint32_t);
void engine_input_mouse (int *, int *);
int engine_run (Game *, int, char **);
