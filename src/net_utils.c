#include "g7_common.h"

bool net_send_buffer(char* buf, size_t len)
{
	return (int)len == SDLNet_TCP_Send(client_state.socket, buf, (int)len);
}

bool net_send_byte(char byte)
{
	return 1 == SDLNet_TCP_Send(client_state.socket, &byte, 1);
}

int net_receive_buffer(char* buf, size_t max_len)
{
	return SDLNet_TCP_Recv(client_state.socket, buf, (int)max_len);
}

bool net_receive_byte(char* byte)
{
	return 1 == SDLNet_TCP_Recv(client_state.socket, byte, 1);
}

void net_atexit(char pref)
{
	char buf[32];
	sprintf(buf, "Q%c ", pref);
	net_send_buffer(buf, 32);

}