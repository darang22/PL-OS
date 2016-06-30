#include <vector>
#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msg.h"

using namespace std;

int
main(int argc, char ** argv) {

  int in = 0;
  int out = 1;

  //Crear mensaje inicial
  srand(time(0));
  int numTareas = rand()%(252)+3;
  Tarea* tareas[numTareas];
  Estadistica* estadisticas[0];

  string listaTareas[6] = {"tarea01",
                           "tarea02",
                           "tarea03",
                           "tarea04",
                           "tarea05",
                           "tarea06"};

  Mensaje* mensaje = new Mensaje;
  mensaje->nTareas = numTareas;
  mensaje->nEstadisticas = 0;
  for (int i = 0; i < numTareas; ++i)
  {
    //Se crean n tareas aleatorias entre tarea01 y tarea06
    //con sus respectivas estadisticas
    random_shuffle(std::begin(listaTareas), std::end(listaTareas));

    Tarea* tarea = new Tarea;
    tarea->asignado = false;

    for (int j = 0; j < 7; ++j)
    {
      tarea->tareaAEjecutar[j] = listaTareas[0][j];
    }
    tareas[i] = tarea;
  }
  mensaje->tareas = tareas;

  int completas = 0;
  numTareas = mensaje->nTareas;
  int numEstadisticas = 0;
  
  //Enviar primer mensaje
  write(out, mensaje, sizeof(*mensaje));
  for (int i = 0; i < numTareas; ++i)
  {    
    Tarea* t = new Tarea;
    write(out, mensaje->tareas[i], sizeof(*mensaje->tareas[i]));
  }
  for (int i = 0; i < numEstadisticas; ++i)
  {
    write(out, mensaje->estadisticas[i], sizeof(*mensaje->estadisticas[i]));
  }

  while(true){
    Mensaje* m = new Mensaje;

    //Leer mensaje
    read(in, m, sizeof(*m));
    numEstadisticas = m->nEstadisticas;
    Tarea** tareas = new Tarea* [numTareas];
    Estadistica** estadisticas = new Estadistica* [numEstadisticas];

    //Leer tareas
    for (int i = 0; i < numTareas; ++i)
    {
      Tarea* nuevaTarea = new Tarea;
      read(in, nuevaTarea, sizeof(*nuevaTarea));
      tareas[i] = nuevaTarea;
    }
    m->tareas = tareas;


    //Leer Estadisticas
    for (int i = 0; i < numEstadisticas; ++i)
    {
      Estadistica* nuevaEstadistica = new Estadistica;
      read(in, nuevaEstadistica, sizeof(*nuevaEstadistica));
      estadisticas[i] = nuevaEstadistica;

    }
    m->estadisticas = estadisticas;

    //Imprimir y eliminar estadisticas
    for (int i = 0; i < numEstadisticas; ++i)
    {
      completas++;
      Estadistica* est = m->estadisticas[i];
      cerr << "completas: " << completas << endl << flush;
      cerr << "Tarea completa: " << est->tareaAEjecutar
      << " - En el PCP " << est->procesoId
      << " - En el hilo: " << est->hiloId << endl << flush;
      //Borrar basura
      delete estadisticas[i];
    }
    m->nEstadisticas = 0;
    numEstadisticas = 0;
    
    //Si nTareas < 1 los PCP terminaran
    //A nTareas se le dara el valor de 0 cuando se completen todas las tareas
    if(completas == numTareas){
      m->nTareas = 0;
    }
    
    //Enviar mensaje
    write(out, m, sizeof(*m));
    
    //Enviar Tareas
    for (int i = 0; i < numTareas; ++i)
    {
      write(out, m->tareas[i], sizeof(*m->tareas[i]));
    }
    
    //Enviar Estadisticas
    for (int i = 0; i < numEstadisticas; ++i)
    {
      write(out, m->estadisticas[i], sizeof(*m->estadisticas[i]));
    }
    
    //Borrar basura
    for (int i = 0; i < numTareas; ++i)
    {
      delete tareas[i];
    }
    delete[] estadisticas;
    delete[] tareas;
    delete m;

    //Si el numero de tareas es igual al numero de tareas completas, terminar
    if(completas == numTareas){
      cerr << "PLP termino" << endl << flush;
      cerr << "Tareas completas: " << completas << endl << flush;
      return 0;
    }
  }
  return 0;
}

