#include "g7_common.h"


void DrawCircle(float cx, float cy, float r, int num_segments)
{
    glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < num_segments; ii++)
    {
        float theta = 2.0f * 3.1415926f * (float)(ii) / (float)(num_segments);//get the current angle

        float x = r * cosf(theta);//calculate the x component
        float y = r * sinf(theta);//calculate the y component

        glVertex2f(x + cx, y + cy);//output vertex

    }
    glEnd();
}

int gameplay_stageloop(SDL_Window *win, struct nk_context *ctx, int win_width, int win_height)
{

	win_width = 512;
	win_height = 512;
	SDL_SetWindowSize(win, win_width, win_height);
	ctx = G7_nk_sdl_reset(win);

	int running = 1;
	struct nk_color background = nk_rgb(28,48,62);

	float x = 340;
	float y = 120;
	float z = 100;

	SDL_Event evt;
	while (running && SDL_WaitEvent(&evt))
	{
		
		nk_input_begin(ctx);

		do
		{
			if (evt.type == SDL_QUIT) return 0;
			nk_sdl_handle_event(&evt);
		}
		while (SDL_PollEvent(&evt));

		nk_input_end(ctx);

		if (nk_begin(ctx, "Blah", nk_rect(0, 0, 120, win_height), //NK_WINDOW_MOVABLE| NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|
		   
			NK_WINDOW_TITLE))
		{

			nk_layout_row_dynamic(ctx, 30, 1);

			if (nk_button_label(ctx, "Up"))
			{
				y-=10.0f;
				printf("y: %f\n", y );
			}
			if (nk_button_label(ctx, "Left"))
			{
				x-=10.f;
				printf("x: %f\n", x );
			}
			if (nk_button_label(ctx, "Rad+"))
			{
				z+=10.0f;
				printf("z: %f\n", z );
			}
			if (nk_button_label(ctx, "Exit"))
			{
				running = 0;
			}


		}
		nk_end(ctx);

		//Drawing 
		{float bg[4];
		nk_color_fv(bg, background);
		SDL_GetWindowSize(win, &win_width, &win_height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, win_width, win_height);
		glOrtho(0.0f, win_width, win_height, 0.0f, -1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// for (int i = 0; i < 10; ++i)
			DrawCircle(x, y, z, 1000);
	
		glClearColor(bg[0], bg[1], bg[2], bg[3]);
		//MAY BREAK
		//blending
		//scissor
		//face culling
		//depth test
		//viewport
		nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
		SDL_GL_SwapWindow(win);
		}


	}
	return 0;
}