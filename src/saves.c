#include "g7_common.h"

FILE* g7_current_save_file;


void g7_append_to_save(char *cmd)
{
	if (g7_current_save_file == NULL)
		return false;

	printf("APPENDING: %.31s\n", cmd);

	fprintf(g7_current_save_file, "%.31s\n", cmd);

}

bool g7_write_map_to_save()
{
	if (g7_create_save_file == NULL)
		return false;

	fprintf(g7_current_save_file, "X %d %d\n", game_state.map.size.x, game_state.map.size.y);


	for (int ii = 0; ii < game_state.map.size.y; ++ii)
	{
		for (int i = 0; i < game_state.map.size.x; ++i)
		{
			fputc(game_state.map.tab[i][ii] + '0', g7_current_save_file);
		}
		fputs("\n", g7_current_save_file);
	}

	fprintf(g7_current_save_file, "PA %d %d\nPB %d %d\n", game_state.playerA.pos.x
														, game_state.playerA.pos.y
														, game_state.playerB.pos.x
														, game_state.playerB.pos.y);

	return true;
	
}


void g7_create_save_file()
{
	char save_name_buf[64];
	char cat_buf[128]="saves/";
	time_t t = time(NULL);
	struct tm *tm_buf = localtime(&t);
	strftime(save_name_buf, 64, "%Y-%m-%d %H-%M-%S.sav", tm_buf);
	// printf("%s\n", save_name_buf);
	strcat(cat_buf, save_name_buf);
	puts(cat_buf);

	g7_current_save_file = fopen(cat_buf, "at");

	if (g7_current_save_file == NULL)
	{
		perror("could not create save file");
		exit(-1);
	}

}