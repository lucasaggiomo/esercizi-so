#ifndef PROCEDURE_H
#define PROCEDURE_H

void client(int id_richieste, int id_risposte, int num_richieste);

void interfaccia(int id_richieste_client,
                 int id_risposte,
                 int id_rts_ots,
                 int id_richieste_server,
                 int dim_buffer);
void* ricevitore(void* arg);
void* mittente(void* arg);

void server(int id_rts_ots, int id_richieste);

#endif