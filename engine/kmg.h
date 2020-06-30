#pragma once

#define KMG_MAGICK			0x00474d4b
#define KMG_VERSION			1
#define KMG_PLAT_DC			1

#define KMG_DCFMT_4BPP_PAL	0x01
#define KMG_DCFMT_8BPP_PAL	0x02
#define KMG_DCFMT_RGB565	0x03
#define KMG_DCFMT_ARGB4444	0x04
#define KMG_DCFMT_ARGB1555	0x05
#define KMG_DCFMT_YUV422	0x06
#define KMG_DCFMT_BUMP		0x07
#define KMG_DCFMT_MASK		0xff
#define KMG_DCFMT_VQ		0x0100
#define KMG_DCFMT_TWIDDLED	0x0200
#define KMG_DCFMT_MIPMAP	0x0400

typedef struct _KMG_header
{
	uint32_t	magick;
	uint32_t	version;
	uint32_t	platform;
	uint32_t	format;
	uint32_t	width;
	uint32_t	height;
	uint32_t	size;
	uint8_t		padding[36];
}KMG_header;
