bool net_send_buffer(char* buf, size_t len);
bool net_send_byte(char byte);
int net_receive_buffer(char* buf, size_t max_len);
bool net_receive_byte(char* byte);
void net_atexit(char pref);