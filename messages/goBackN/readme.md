Implementazione di un'applicazione **multiprocesso** in linguaggio C basata su
**code di messaggi UNIX** e **thread**, che simuli l'algoritmo di Go-Back-N.
Per semplificare, il mittente è assimilato ad un client, che può solo inviare
dati e ricevere ACK dal ricevitore, mentre il ricevitore è implementato come
un server, che può solo ricevere dati e inviare ACK al mittente.

# Master
Il processo master deve generare due processi figli:
- livello applicativo del client (app_client.c);
- livello applicativo del server (app_server.c);

Inoltre il master deve allocare due code per la comunicazione tra i livelli trasporto delle applicazioni:
- una coda per inviare i messaggi trasporto dal client al server;
- una coda per inviare i messaggi di ack dal server al client;

Il livello applicativo del server e del client si occupano solo di inviare/ricevere
messaggi al/dal livello trasporto associato, tramite le apposite code (create dal livello applicativo).
La ricezione dei messaggi è realizzata a livello applicativo in maniera astratta,
usufruendo delle funzioni implementate al livello trasporto, nei file transport_client.c
e transport_server.c, dove è effettivamente implementato l'algoritmo di Go-Back-N.
Il livelli inferiori dello stack del modello ISO-OSI (rete e fisico) sono implementati
dal sistema operativo tramite l'utilizzo delle code di messaggi UNIX.

# Livello applicativo (app_client.c e app_server.c)
I processi al livello applicativo generano il rispettivo processo figlio per simulare
il livello trasporto sottostante:
- livello trasporto del client (trasport_client.c);
- livello trasporto del server (trasport_server.c);

Il livello applicativo si occupa anche di creare due code per la comunicazione con il livello trasporto sottostante:
- una per inviare dal livello applicativo al livello trasporto i messaggi applicativi
- una per inviare dal livello trasporto al livello applicativo un ACK finale

L'applicazione client (app_client.c) invia 10 pacchetti dati, contenenti delle lettere inviate separatamente
e l'applicazione server (app_server.c) deve ricevere i pacchetti in maniera integra e in ordine,
per ricostruire il messaggio a partire dai singoli caratteri disposti nell'ordine corretto.

Il processo client termina appena è certo che il server ha ricevuto tutti i messaggi, ovvero
dopo che il livello trasporto sottostante gli abbia inviato un messaggio di ACK globale del messaggio
(mandato dal livello trasporto dopo che questi ha ricevuto tutti gli ACK da parte del server riferiti ai messaggi inviati)

Il processo server termina appena ha ricevuto tutti i messaggi dal client e dopo aver mostrato a video
il messaggio ricostruito. In particolare i messaggi inviati dal client li riceve a partire dal livello trasporto sottostante, che glieli invia nell'ordine corretto.

# Livello trasporto
Il livello trasporto del client (transport_client.c) si occupa di incapsulare i messaggi
dell'applicazione con un "header", in cui inserisce informazioni necessarie all'invio corretto
dei messaggi correttamente e in ordine, come il numero di sequenza del pacchetto.
Questi messaggi vengono mandati tramite un'apposita coda creata dal master al livello trasporto del server.
Inoltre si occupa anche di attendere la ricezione di un messaggio di ACK per ogni pacchetto inviato
entro un timeout specifico a partire dall'invio del pacchetto, in modo da gestire la ritrasmissione
del pacchetto in caso di non ricezione dell'ACK (buffer dei messaggi inviati implementato qui).
Quando il livello trasporto ha ricevuto l'ACK di tutti i messaggi inviati, manda un messaggio di
ACK generale al livello applicativo.

Il livello trasporto del server (transport_server.c) si occupa di ricevere i messaggi dal livello
trasporto del server, ottenere il messaggio vero (quello originato dall'app client) e di
inviare un ACK al livello trasporto del client.
Attraverso l'algoritmo Go-Back-N, il livello trasporto invia al livello applicativo
i messaggi estratti nell'ordine corretto.

# Note
Per simulare dei problemi di rete nell'invio di messaggi, c'è una piccola probabilità che i pacchetti
non vengano mandati a livello trasporto (ma il mittente deve comunque "credere" di averli inviati,
ad esempio incapsulando l'invio di un pacchetto in una funzione "invia_pacchetto", nel quale c'è
una probabilità che il pacchetto non viene mandato, senza che il chiamante lo sappia).
In questo modo è possibile apprezzare il funzionamento dell'algoritmo.

Vedere slide di Canonico per spiegazioni sull'algoritmo