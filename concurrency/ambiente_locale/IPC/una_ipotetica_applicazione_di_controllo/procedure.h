#ifndef PROCEDURE_H
#define PROCDEURE_H

void controllore(int id_comandi_macchina,
                 int id_log_interfaccia,
                 int id_rts_termostato,
                 int id_ots_termostato,
                 int id_comandi_termostato,
                 int id_feedback_termostato,
                 int id_rts_mixer,
                 int id_ots_mixer,
                 int id_comandi_mixer);

void termostato(int id_rts,
                int id_ots,
                int id_comandi,
                int id_feedback);

void mixer(int id_rts,
           int id_ots,
           int id_comandi);

void log_message(const char* message);
void termina_macchina(const char* message);

// copia AL MASSIMO dest_dim-1 caratteri di source in dest e termina SEMPRE con il carattere '\0'
// restituisce la lunghezza di dest (numero di caratteri copiati eccetto '\0')
size_t safe_strcpy(char* dest, const char* source, size_t dest_dim);

#endif