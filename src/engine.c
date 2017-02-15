#include "g7_common.h"

g7_game_state game_state;
//global game state
g7_client_state client_state;
//client state

g7_vertex screen_to_tile(g7_vertex mouse)
{
	g7_vertex ret;

	ret.x = (int)floor((float)((float)mouse.x - x_offset)/scale);
	ret.y = (int)floor((float)((float)mouse.y)/scale);

	return ret;

}

bool g7_path_is_nonblocked(g7_vertex pos, g7_vertex dest)
{
	int x1 = pos.x;
	int y1 = pos.y;

	int x2 = dest.x;
	int y2 = dest.y;

	int d, dx, dy, ai, bi, xi, yi;
	int x = x1;
	int y = y1;

	if (x1 < x2)
	{ 
		xi = 1;
		dx = x2 - x1;
	} 
	else
	{ 
		xi = -1;
		dx = x1 - x2;
	}
	// ustalenie kierunku rysowania
	if (y1 < y2)
	{ 
		yi = 1;
		dy = y2 - y1;
	} 
	else
	{ 
		yi = -1;
		dy = y1 - y2;
	}
	// pierwszy piksel
	// g7_draw_tile(y, x, finish, false);
	// oś wiodąca OX
	if (dx > dy)
	{
		ai = (dy - dx) * 2;
		bi = dy * 2;
		d = bi - dx;
		// pętla po kolejnych x
		while (x != x2)
		{ 
			// test współczynnika
			if (d >= 0)
			{ 
				x += xi;
				y += yi;
				d += ai;
			} 
			else
			{
				d += bi;
				x += xi;
			}

			if(game_state.map.tab[x][y] == block)
				return false;
		}
	} 
	// oś wiodąca OY
	else
	{ 
		ai = ( dx - dy ) * 2;
		bi = dx * 2;
		d = bi - dy;
		// pętla po kolejnych y
		while (y != y2)
		{ 
			// test współczynnika
			if (d >= 0)
			{ 
				x += xi;
				y += yi;
				d += ai;
			}
			else
			{
				d += bi;
				y += yi;
			}

		if(game_state.map.tab[x][y] == block)
			return false;

		}
	}

	return true;

}

