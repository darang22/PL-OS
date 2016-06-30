
const int MAX_TAREAS = 256;
const int MAX_TEXT_TAREAS = 64;

struct Tarea {
    bool asignado;
    char tareaAEjecutar[MAX_TEXT_TAREAS];
    unsigned short procesoId;
    unsigned short hiloId;
};

struct Estadistica {
    char tareaAEjecutar[MAX_TEXT_TAREAS];
    unsigned short procesoId;
    unsigned short hiloId;
};

struct Mensaje {
    unsigned nTareas;
    unsigned nEstadisticas;
    Tarea** tareas;
    Estadistica** estadisticas;
};
