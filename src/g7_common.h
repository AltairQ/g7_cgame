#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <dirent.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

/* nuklear - v1.17 - public domain */

#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 450

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

#define G7_PARAM_NEWGAME 1
#define G7_PARAM_LOADGAME 2
#define G7_PARAM_FULLSCREEN 4

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_sdl_gl2.h"

#include "utils.h"

#include "main_menu.h"
#include "load_dialog.h"
#include "gameplay.h"

