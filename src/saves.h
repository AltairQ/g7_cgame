extern FILE* g7_current_save_file;

void g7_create_save_file();
bool g7_write_map_to_save();
void g7_append_to_save(char *cmd);
void g7_close_save();