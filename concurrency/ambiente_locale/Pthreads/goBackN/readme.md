Implementazione di un'applicazione **multiprocesso** in linguaggio C basata su
**code di messaggi UNIX** e **thread**, che simuli l'algoritmo di Go-Back-N.
Per semplificare, il mittente è assimilato ad un client, che può solo inviare
dati e ricevere ACK dal ricevitore, mentre il ricevitore è implementato come
un server, che può solo ricevere dati e inviare ACK al mittente.

# Compilazione
Compilare con make ed eseguire ./master

# Master
Il processo master deve generare due processi figli (due eseguibili differenti dal master):
- un processo per il client (client.c);
- un processo per il server (server.c);

Inoltre il master deve allocare due code per la comunicazione (tra i livelli trasporto) delle applicazioni:
- una coda per inviare i messaggi trasporto dal client al server;
- una coda per inviare i messaggi di ack dal server al client;

Il processo master termina quando hanno terminato i processi figli, dopo aver deallocato le code.

# Processi client e server
Il processo client e il processo server creano a loro volta due processi figli:
- un processo che esegue la funzione del "livello applicativo"
- un processo che esegue la funzione del "livello trasporto"

E allocano le code necessarie alla comunicazione tra questi due livelli.
Nel dettaglio:
Il client crea le seguenti code:
- una per scambiare messaggi RTS (request to send) e OTS (ok to send)
- una per inviare dal livello applicativo al livello trasporto i messaggi applicativi (dopo aver effettuato la sincronia con RTS e OTS)

Il server crea la seguente coda:
- una per inviare dal livello trasporto al livello applicativo i messaggi dell'client in ordine di invio del client

Sia il client che il server terminano quando hanno terminato i due figli, dopo aver deallocato le code.

# Livello applicativo (app_client.c e app_server.c)
L'applicazione client invia 10 pacchetti dati, contenenti delle lettere inviate separatamente.

L'applicazione server deve ricevere i pacchetti in maniera integra e in ordine,
per ricostruire il messaggio a partire dai singoli caratteri disposti nell'ordine corretto.

Il processo app_client termina quando è certo che il server ha ricevuto tutti i messaggi, ovvero
dopo che il livello trasporto gli abbia inviato un messaggio di ACK globale del messaggio
(mandato dal livello trasporto dopo che questi ha ricevuto tutti gli ACK da parte del server riferiti ai messaggi inviati)

Il processo app_server termina appena ha ricevuto tutti i messaggi dal client e dopo aver mostrato a video
il messaggio ricostruito. In particolare i messaggi inviati dal client li riceve a partire dal livello trasporto sottostante, che glieli invia nell'ordine corretto.

# Livello trasporto
Il livello trasporto del client si occupa di incapsulare i messaggi
dell'applicazione con un "header", in cui inserisce informazioni necessarie all'invio corretto
dei messaggi correttamente e in ordine, come il numero di sequenza del pacchetto.
Questi messaggi vengono mandati tramite un'apposita coda creata dal master al livello trasporto del server,
che i livelli trasporto devono ottenere con una chiamata a msgget.
Inoltre transport_client si occupa anche di attendere la ricezione di un messaggio di ACK per ogni pacchetto inviato
entro un timeout specifico a partire dall'invio del pacchetto, in modo da gestire la ritrasmissione
del pacchetto in caso di non ricezione dell'ACK (buffer dei messaggi inviati implementato qui).

Il livello trasporto del server si occupa di ricevere i messaggi dal livello
trasporto del server, inviando un ACK al livello trasporto del client ad ogni ricezione
e di inviare al livello applicativo i messaggi ricevuti nell'ordine corretto (eliminao l'"header" creato dal livello trasporto client)

I livelli trasporto implementano quindi l'algoritmo Go-Back-N.

NEL DETTAGLIO:
- Il processo transport_server si pone in attesa di ricevere messaggio dal client.
    Il server non presenta un buffer, quindi attende sempre e solo il messaggio successivo a quello ricevuto in precedenza (o il primo).
    Se il server riceve un pacchetto con un numero di sequenza diverso da quello che si aspetta, lo scarta.
    Una volta ricevuto il messaggio che si aspetta, il server:
    - incrementa il numero di sequenza del messaggio che si aspetta di ricevere
    - invia un ack al client con numero di ack uguale al numero di sequenza del pacchetto ricevuto
    - manda al livello applicativo server il messaggio del client incapsulato nel messaggio del livello trasporto ricevuto

- Il processo transport_client crea una window come memoria condivisa tra i thread (ovvero nell'heap) e crea i seguenti thread:
    1) Un thread che attende i messaggi dall'applicazione (inviandogli un'OTS dopo aver ricevuto un RTS, nel caso in cui sia pronto a ricevere,
       ovvero se la window non è piena). Quindi, alla ricezione del messaggio dell'applicazione, questo viene incapsulato
       in un messaggio trasporto, a cui è associato un numero di sequenza progressivo, ottenuto in base al numero di messaggi inviati finora.
       Successivamente questo thread inserisce il messaggio app nella window ai thread (agisce quindi da produttore).
    2) Un thread che preleva messaggi app dalla window e li invia al livello trasporto server
    3) Un thread che si mette in attesa della ricezione dell'ack del pacchetto con numero di sequenza minore che è stato inviato per cui si
       attende ancora l'ack. Implementa al suo interno la logica dell'algoritmo, per gestire opportunemante i seguenti casi:
       - l'ack è stato ricevuto prima del timeout e:
          - l'ack viene scartato se corrisponde ad un messaggio di cui il mittente era già certo che fosse stato ricevuto dal destinatario
            (che avviene perché l'ack è cumulativo, quindi per esempio può avvenire se è stato ricevuto un ack con numero N e poi un ack con
            valore M < N)
       - l'ack non è stato ricevuto prima del timeout, quindi vengono ritrasmessi i pacchetti per cui si attende l'ack

# Note
Per simulare dei problemi di rete nell'invio di messaggi, c'è una piccola probabilità che i pacchetti
non vengano mandati a livello trasporto (ma il mittente deve comunque "credere" di averli inviati,
incapsulando la logica dell'invio di un messaggio o di un ack all'interno di una funzione "send_maybe",
che ha in ingresso un parametro che indica la probabilità che il pacchetto venga effettivamente inviato).
Per questioni di debug il mittente scrive opportunamente a video l'esito dell'invio del messaggio (successo o fallimento),
ma comunque il mittente si comporta in tutti i casi come se il pacchetto fosse giunto a destinazione.
In questo modo è possibile apprezzare il funzionamento dell'algoritmo, in quanto in caso di perdite di messaggi dati o di ack
scatta il timeout

Il livello applicativo del server e del client si occupano solo di inviare/ricevere
messaggi al/dal livello trasporto associato, tramite le apposite code (create dal livello applicativo).
Quindi il livello applicativo non conosce la logica sottostante.

La ricezione dei messaggi è realizzata a livello applicativo in maniera astratta,
usufruendo delle funzioni implementate al livello trasporto, nei file transport_client.c
e transport_server.c, dove è effettivamente implementato l'algoritmo di Go-Back-N.
I livelli inferiori dello stack del modello ISO-OSI (rete e fisico) sono implementati
dal sistema operativo tramite l'utilizzo delle code di messaggi UNIX.
La comunicazione tra livello applicativo e livello trasporto è implementata tramite code
di messaggi UNIX (che dovrebbero simulare le socket), dove in questo caso non sono presenti
possibili corruzioni come per i messaggi tra i livelli trasporto client e server.

Per semplicità sia il server che il client sono a conoscenza del numero di messaggi da inviare/ricevere.

Nelle slide di Canonico:
• Nell’header del segmento k-bit per il num. sequenza
• Una finestra di max N pacchetti senza riscontro
• ACK numerati
• ACK cumulativo: ricevere ACK(n) significa che tutti i pkts precedenti l’n-esimo sono stati ricevuti correttamente
• Un solo timer per il primo pacchetto trasmesso e non ancora riscontrato
• timeout(n): ritrasmetti pkt n e tutti i pacchetti che seguono n
• Il ricevente non deve accumulare i pacchetti arrivati: se il pacchetto arrivato non è quello atteso, il pacchetto è scartato
