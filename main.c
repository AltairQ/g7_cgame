#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL2_IMPLEMENTATION
/* nuklear - v1.17 - public domain */
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_sdl_gl2.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

int main(int argc, char* argv[])
{
	/* Platform */
	SDL_Window *win;
	SDL_GLContext glContext;
	struct nk_color background;
	int win_width, win_height;
	int running = 1;

	/* GUI */
	struct nk_context *ctx;

	/* SDL setup */
	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	win = SDL_CreateWindow("Demo",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
	glContext = SDL_GL_CreateContext(win);
	SDL_GetWindowSize(win, &win_width, &win_height);

	//if fullscreen
	// SDL_DisplayMode modein;
	// SDL_GetDesktopDisplayMode(0, &modein);
	// SDL_SetWindowSize(win, modein.w, modein.h);
	// SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);

	ctx = nk_sdl_init(win);
	{
	struct nk_font_atlas *atlas;

	nk_sdl_font_stash_begin(&atlas);
	// struct nk_font *custom_font = nk_font_atlas_add_from_file(atlas, "./font/font.ttf", 13, 0);
	nk_sdl_font_stash_end();

	// nk_style_load_all_cursors(ctx, atlas->cursors);
	// nk_style_set_font(ctx, &custom_font->handle);
	}


	background = nk_rgb(28,48,62);
	static int property = 20;
	while (running)
	{
		//add filtering!
		SDL_Event evt;
		nk_input_begin(ctx);
		while (SDL_PollEvent(&evt)) {
			if (evt.type == SDL_QUIT) goto cleanup;
			nk_sdl_handle_event(&evt);
		}
		nk_input_end(ctx);

		if (nk_begin(ctx, "TEST WINDOW", nk_rect(0, 0, 210, win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		   
			NK_WINDOW_BORDER|NK_WINDOW_TITLE))
		{
			enum {EASY, HARD};
			static int op = EASY;
			

			nk_layout_row_static(ctx, 30, 80, 1);
			if (nk_button_label(ctx, "button"))
				fprintf(stdout, "button pressed %d\n", property);
			nk_layout_row_dynamic(ctx, 30, 2);
			if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
			if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
			nk_layout_row_dynamic(ctx, 25, 1);
			nk_property_int(ctx, "Test:", 0, &property, 100, 10, 1);

			nk_layout_row_dynamic(ctx, 20, 1);
			nk_label(ctx, "background:", NK_TEXT_LEFT);
			nk_layout_row_dynamic(ctx, 25, 1);
			if (nk_combo_begin_color(ctx, background, nk_vec2(nk_widget_width(ctx),400))) {
				nk_layout_row_dynamic(ctx, 120, 1);
				background = nk_color_picker(ctx, background, NK_RGBA);
				nk_layout_row_dynamic(ctx, 25, 1);
				background.r = (nk_byte)nk_propertyi(ctx, "#R:", 0, background.r, 255, 1,1);
				background.g = (nk_byte)nk_propertyi(ctx, "#G:", 0, background.g, 255, 1,1);
				background.b = (nk_byte)nk_propertyi(ctx, "#B:", 0, background.b, 255, 1,1);
				background.a = (nk_byte)nk_propertyi(ctx, "#A:", 0, background.a, 255, 1,1);
				nk_combo_end(ctx);
			}
		}
		nk_end(ctx);

		//Drawing 
		{float bg[4];
		nk_color_fv(bg, background);
		SDL_GetWindowSize(win, &win_width, &win_height);
		glViewport(0, 0, win_width, win_height);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glColor3f(1.0,0.0,0.0);
			glBegin(GL_QUADS);
			glVertex3f(-0.10f*(float)(property)*0.1f,-0.10f*(float)(property)*0.1f,0.00f);
			glVertex3f(0.10f*(float)(property)*0.1f,-0.10f*(float)(property)*0.1f,0.00f);
			glVertex3f(0.10f*(float)(property)*0.1f,0.10f*(float)(property)*0.1f,0.00f);
			glVertex3f(-0.10f*(float)(property)*0.1f,0.10f*(float)(property)*0.1f,0.00f);
		glEnd();
		 glClearColor(bg[0], bg[1], bg[2], bg[3]);
		 //MAY BREAK
		 //blending
		 //scissor
		 //face culling
		 //depth test
		 //viewport
		nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
		SDL_GL_SwapWindow(win);}
	}

cleanup:
	nk_sdl_shutdown();
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}

