#ifndef APP_CLIENT_H
#define APP_CLIENT_H

#include "messages.h"

void app_client(int id_rts_ots, int id_app_dati);

void send_sincrona(int id_rts_ots, int id_app_dati, struct msg_app* msg);

#endif