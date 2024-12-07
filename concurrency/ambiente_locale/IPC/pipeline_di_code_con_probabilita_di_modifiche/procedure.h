#ifndef PROCEDURE_H
#define PROCEDURE_H

void client(int id_iniettore, const char* str, int num_messaggi);

void iniettore(int id_client, int id_server, float value_corruption_chance, float str_corruption_chance, int num_messaggi);
void server(int id_iniettore, int num_messaggi);

#endif