// Mateusz Maciejewski
// 15.02.2017
// saves.c

//handles the save files

#include "g7_common.h"

//save file descriptor
FILE* g7_current_save_file;


//write a command to save file
bool g7_append_to_save(char *cmd)
{
	if (g7_current_save_file == NULL)
		return false;

	printf("APPENDING: %.31s\n", cmd);

	fprintf(g7_current_save_file, "%.31s\n", cmd);

	return true;
}

//dump map state to save file
bool g7_write_map_to_save()
{
	//if we have an open save then proceed
	if (g7_current_save_file == NULL)
		return false;

	//write the map size
	fprintf(g7_current_save_file, "X %d %d\n", game_state.map.size.x, game_state.map.size.y);


	//write the individual tiles
	for (int ii = 0; ii < game_state.map.size.y; ++ii)
	{
		for (int i = 0; i < game_state.map.size.x; ++i)
		{
			fputc((int)game_state.map.tab[i][ii] + '0', g7_current_save_file);
		}
		fputs("\n", g7_current_save_file);
	}

	//write positions and max velocity difference
	fprintf(g7_current_save_file, "PA %d %d\nPB %d %d\nD %d\n", game_state.playerA.pos.x
														, game_state.playerA.pos.y
														, game_state.playerB.pos.x
														, game_state.playerB.pos.y
														, game_state.max_delta);

	return true;
	
}


//create new save file
void g7_create_save_file()
{
	char save_name_buf[64];
	char cat_buf[128]="saves/";

	//fetch current time
	time_t t = time(NULL);
	struct tm *tm_buf = localtime(&t);

	//create filename from time
	strftime(save_name_buf, 64, "%Y-%m-%d %H-%M-%S.sav", tm_buf);

	strcat(cat_buf, save_name_buf);


	g7_current_save_file = fopen(cat_buf, "at");

	//if error
	if (g7_current_save_file == NULL)
	{
		perror("could not create save file");
		exit(-1);
	}

}

//close the save file
void g7_close_save()
{
	if(g7_current_save_file != NULL	)
		fclose(g7_current_save_file);
}