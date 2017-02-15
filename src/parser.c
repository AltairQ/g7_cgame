// Mateusz Maciejewski
// 15.02.2017
// parser.c

#include "g7_common.h"

//allocate map in memory
bool g7_allocate_map(g7_map* in_map)
{
	//nothing special here.

	(*in_map).tab = malloc(sizeof(g7_tile*) * (size_t)(*in_map).size.x);

	if ((*in_map).tab == NULL)
	{
		return false;
	}

	for (int i = 0; i < (*in_map).size.x; ++i)
	{
		(*in_map).tab[i] = malloc(sizeof(g7_tile) * (size_t)(*in_map).size.y);

		if((*in_map).tab[i] == NULL)
		{
			return false;
		}
	}

	return true;

}


//read state from file descriptor
bool g7_read_state_from_file(FILE* map)
{
	int ret_code = 0;
	g7_map tmp_map;

	//read map size
	fscanf(map, "X %d %d ", &(tmp_map.size.x), &(tmp_map.size.y));
	if(tmp_map.size.x <= 0 || tmp_map.size.y <=0 || tmp_map.size.x > (1<<30) || tmp_map.size.y > (1<<30) )
	{
		ret_code = -2;
		goto cleanup;
	}

	//try to allocate memory
	if (!g7_allocate_map(&tmp_map))
	{
		ret_code = -3;
		goto cleanup;
	}

	//read tiles
	for (int ii = 0; ii < tmp_map.size.y; ++ii)
	{
		for (int i = 0; i < tmp_map.size.x; ++i)
		{
			char buf;
			fscanf(map, "%c ", &buf);
			tmp_map.tab[i][ii] = buf - '0';
		}
	}


	game_state.map = tmp_map;

	char* linebuf = malloc(sizeof(char)*1024);

	//read all commands in file
	while(fgets(linebuf, 1024, map) != NULL)
		g7_command_parse(linebuf);
	
	free(linebuf);


	cleanup:
	fclose(map);

	return ret_code == 0;

}

//read gamestate from file
//and create save if it doesn't exist
bool g7_init_gamestate_from_file(char* fname, bool issave)
{
	FILE *sfile;
	sfile = fopen(fname, "r+");

	if (sfile == NULL)
		return false;

	//try to read state
	if (!g7_read_state_from_file(sfile))
		return false;

	fclose(sfile);	

	//if it's a save
	if (issave)
	{
		//set the file descriptor
		g7_current_save_file = fopen(fname, "at");
	}
	else
		//if not
	{
		//create save
		//copy data from map
		g7_create_save_file();
		g7_write_map_to_save();
	}

	return true;
}


//parse and execute command from string
void g7_command_parse(char* cmd)
{
	int x, y;

	switch(cmd[0])
	{

	//move player from old position to new one
	//and update their velocity
	case 'M':
		if (cmd[1] == 'A')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerA.vel.x = x;
				game_state.playerA.vel.y = y;

				game_state.playerA.pos.x += x;
				game_state.playerA.pos.y += y;
			}
		}

		if (cmd[1] == 'B')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerB.vel.x = x;
				game_state.playerB.vel.y = y;

				game_state.playerB.pos.x += x;
				game_state.playerB.pos.y += y;
			}
		}

	break;

	//just set the position
	case 'P':
	
		if (cmd[1] == 'A')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerA.pos.x = x;
				game_state.playerA.pos.y = y;
			}
		}

		if (cmd[1] == 'B')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerB.pos.x = x;
				game_state.playerB.pos.y = y;
			}
		}

	break;

	//just set the velocity
	case 'V':
		if (cmd[1] == 'A')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerA.vel.x = x;
				game_state.playerA.vel.y = y;
			}
		}

		if (cmd[1] == 'B')
		{			
			if(sscanf(cmd+3, "%d %d", &x, &y) == 2 )
			{
				game_state.playerB.vel.x = x;
				game_state.playerB.vel.y = y;
			}
		}
	break;

	//set the maximum velocity difference
	case 'D':
		if(sscanf(cmd+1, "%d", &x) == 1)
			game_state.max_delta = x;
	break;

	//info from server that the game ended
	//and it's a draw
	case 'R':
		game_state.draw = true;
	break;

	//info from server that the game ended
	//and someone won
	case 'W':

		if (cmd[1] == 'A')
		{		
			game_state.a_win = true;
		}

		if (cmd[1] == 'B')
		{			
			game_state.b_win = true;
		}

	break;

	//ignore other
	default:
	break;
	}

}