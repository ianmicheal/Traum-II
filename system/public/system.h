#pragma once
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_CONTROLLERS 4

enum AXIS
{
	AXIS_L1 = 0,
	AXIS_R1,
	AXIS_LX,
	AXIS_LY,
	AXIS_RX,
	AXIS_RY,
	MAX_AXIS
};
enum KEY
{/*Matches the order given in kallistios*/
	KEY_C = (1<<0),
	KEY_B = (1<<1),
	KEY_A = (1<<2),
	KEY_START = (1<<3),
	KEY_UP = (1<<4),
	KEY_DOWN = (1<<5),
	KEY_LEFT = (1<<6),
	KEY_RIGHT = (1<<7),
	KEY_Z = (1<<8),
	KEY_Y = (1<<9),
	KEY_X = (1<<10),
	KEY_D = (1<<11),
	MAX_KEY
};
enum SE
{
	SE_NOP = 0,
	SE_QUIT,
	SE_KEYUP,
	SE_KEYDOWN,
	SE_MOUSE,
	SE_AXIS,
	SE_GAMEPAD,
	SE_MAX
};
typedef struct _Mouse_state
{
	int x, y;
}Mouse_state;
typedef struct _Axis_state
{
	uint32_t cont;
	uint32_t axis;
	float value;
}Axis_state;
typedef struct _Sys_app
{
	int (*init) (int, char **);
	void (*event) (int, void *);
	void (*frame) (uint32_t);
	void (*shutdown) (void);
}Sys_app;

/*OpenGL video*/
void sys_video_gl_swap (void);
void sys_video_gl_enable (void);
void sys_video_gl_disable (void);
int sys_video_gl_init (const char *, int, int);
void sys_video_gl_shutdown (void);

/*IO*/
FILE *sys_fopen (const char *file);

/*General interface*/
int sys_run (Sys_app *, int, char **);
