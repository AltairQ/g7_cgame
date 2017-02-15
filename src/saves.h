// Mateusz Maciejewski
// 15.02.2017
// saves.h

//save file handling

//global save file descriptor
extern FILE* g7_current_save_file;

//create save file
void g7_create_save_file();

//dump the map state to save file
bool g7_write_map_to_save();

//write a command to save file
bool g7_append_to_save(char *cmd);

//close the save file
void g7_close_save();