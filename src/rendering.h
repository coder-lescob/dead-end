#ifndef RENDERING_H
#define RENDERING_H

#include "platform.h"

// TYPES
#ifdef PLATFORM_CALCULATOR

#include <eadk.h>

// constants
#define SCREEN_WIDTH  EADK_SCREEN_WIDTH
#define SCREEN_HEIGHT EADK_SCREEN_HEIGHT
#define LITTLE_FONT_HEIGHT 14 // 14p
#define HUD_HIGHT      32
#define VIEW_3D_HEIGHT SCREEN_HEIGHT - HUD_HIGHT
#define HUD_RECT (rect_t) { 0, VIEW_3D_HEIGHT, SCREEN_WIDTH, HUD_HIGHT }

#define COLOR_WHITE eadk_color_white
#define COLOR_BLACK eadk_color_black
#define COLOR_RED eadk_color_red

// other types
typedef eadk_rect_t rect_t;
typedef eadk_color_t color_t;

typedef struct {
    int16_t x, y;
} point_t;

#else
#error PLATFORM NOT SUPPORTED
#endif

#define SCREEN_RECT (rect_t) { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }

// FUNCTION WRAPPERS

/**
 * creates a renderer
 * @note MUST init SCREEN_WIDTH and SCREEN_HEIGHT to some value if there undefined for this platform
 */
void create_renderer();

/**
 * destroys the renderer
 */
void destroy_renderer();

/**
 * draws an uniform rectangle with the color color.
 */
void draw_rect_uniform(rect_t rect, color_t color);

/**
 * draws a textured rectangle to the screen.
 * @note pixels MUST be of length rect.width * rect.height 
 *  not doing so would result in segmentation fault
 */
void draw_rect_textured(rect_t rect, const color_t *pixels);

/**
 * draw a white string on a black background using the little font (on the calculator)
 * @note str MUST be null terminated
 */
void draw_string(point_t point, const char *str);

/**
 * swap the backbuffer with the screen buffer
 * @note on the calculator it only waits for vblank
 */
void swap_buffer();

#endif
