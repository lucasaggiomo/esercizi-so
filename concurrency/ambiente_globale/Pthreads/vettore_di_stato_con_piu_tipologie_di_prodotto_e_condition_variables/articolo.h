#ifndef ARTICOLO_H
#define ARTICOLO_H

enum tipo {
    SCARPE = 1,
    GIACCA = 2
};

enum stato {
    LIBERO,
    OCCUPATO,
    IN_USO
};

struct articolo {
    enum tipo tipo;       // SCARPE = 1, GIACCA = 2
    enum stato stato;     // LIBERO = 0, OCCUPATO = 1, IN USO = 2
};

#endif