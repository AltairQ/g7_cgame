#include "g7_common.h"

g7_game_state game_state;
//global game state

g7_vertex screen_to_tile(g7_vertex mouse)
{
	g7_vertex ret;

	ret.x = (int)floor((float)(mouse.x - x_offset)/scale);
	ret.y = (int)floor((float)(mouse.y)/scale);

	return ret;

}


