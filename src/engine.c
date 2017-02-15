// Mateusz Maciejewski
// 15.02.2017
// engine.c

#include "g7_common.h"

//global game state
g7_game_state game_state;

//client state
g7_client_state client_state;

//convert from screen coordinates to map coords
g7_vertex screen_to_tile(g7_vertex mouse)
{
	g7_vertex ret;

	ret.x = (int)floor((float)((float)mouse.x - client_state.x_offset)/client_state.scale);
	ret.y = (int)floor((float)((float)mouse.y)/client_state.scale);

	return ret;

}

//any possible moves for the player?
bool g7_has_moves(g7_player_state player)
{
	for (int ii = 0; ii < game_state.map.size.y; ++ii)
	{
		for (int i = 0; i < game_state.map.size.x; ++i)
		{
			g7_vertex btile;

			btile.x = i;
			btile.y = ii;

			if(g7_is_accessible(player, btile) && g7_path_is_nonblocked(player.pos, btile))
				return true;
		}
	}

	return false;
}

//is the player on finish line?
bool g7_on_finish(g7_player_state player)
{
	int x = player.pos.x;
	int y = player.pos.y;

	return (game_state.map.tab[x][y] == finish);
}


//is the tile within player's reach
bool g7_is_accessible(g7_player_state player, g7_vertex tile)
{
	if (player.vel.x -game_state.max_delta <= tile.x - player.pos.x  && player.vel.x + game_state.max_delta >= tile.x - player.pos.x &&
	 player.vel.y -game_state.max_delta <= tile.y - player.pos.y  && player.vel.y + game_state.max_delta >= tile.y - player.pos.y)
	{
		return true;
	}
	else
	{
		return false;
	}

}


//regular Bresenham's line algorithm checking if 
//the line between two points passes only through non-blocked tiles
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

	if (dx > dy)
	{
		ai = (dy - dx) * 2;
		bi = dy * 2;
		d = bi - dx;

		while (x != x2)
		{ 

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
	else
	{ 
		ai = ( dx - dy ) * 2;
		bi = dx * 2;
		d = bi - dy;
		while (y != y2)
		{ 
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

