#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include "msg.h"


using namespace std;

//Vector donde se guarda el nombre de las tareas a ejecutar
vector<string>vsTareas;
//Vector donde se guardan las estadisticas que completan los hilos
vector<Estadistica*>vEstadisticas;

sem_t mutex, semHilos;
//numProc = numero de hilos libres
int numProc;
//id del PCP en el anillo
int idProc;
string tarea; 

void *HiloTareas(void * id){
  char *dir;

  short tid = (short)(intptr_t) id;
  dir = getenv("PLN_DIR_TAREAS");
  while(true){
    string task;
    //Esperar a que el proceso padre le asigne una tarea
    sem_wait(&semHilos);
    sem_wait(&mutex);
    task = vsTareas.back();
    vsTareas.pop_back();
    sem_post(&mutex);
    //fork para ejecutar la tarea asignada
    int npid = fork();
    if(npid==0){
      string exec = dir + task;
      execl(exec.c_str(), task.c_str(), NULL);
      cerr << "Error - No se ejecuto la tarea" << endl << flush;
      return 0;
    }
    int status;
    //Espera a que el hijo termine su ejecucion
    waitpid(npid, &status, 0);
    sem_wait(&mutex);
    //cerr << "Tarea finalizada con: " << WEXITSTATUS(status) << endl << flush;
    numProc++;
    //Crear una nueva estadistica para pasarla al mensaje
    Estadistica* est = new Estadistica;
    est->procesoId = idProc; 
    for (int j = 0; j < 7; ++j)
    {
      est->tareaAEjecutar[j] = task[j];
    }
    est->hiloId = tid;
    vEstadisticas.push_back(est);
    sem_post(&mutex);
  }
}


int main(int argc, char ** argv) {
  //Leer argumentos
  //ID proceso (Numero dentro del anillo)
  idProc = stoi(argv[1]);
  int in = 0;
  int out = 1;
  string tarea = "Task";
  //n = numero de hilos - MIN 3 - MAX 10
  int n = stoi(argv[2]);
  
  //Se le asigna a numProc el numero de hilos libres inicial
  numProc = n;
  sem_init(&semHilos,0,0);
  sem_init(&mutex,0,1);
  //Se crean n semaforos para los hilos
  pthread_t hilos[n];
  for(int i=0;i<n;i++) {
    pthread_create(&hilos[i], NULL, HiloTareas, (void *)(intptr_t) i); 
  }

  while(true){
    sem_wait(&mutex);
    Mensaje* m = new Mensaje;

    //Leer mensaje
    read(in, m, sizeof(*m));
    int nuevasEst = vEstadisticas.size();
    //Estadisticas dentro del mensaje + estadisticas dentro del vector
    int numEstadisticas = (m->nEstadisticas)+nuevasEst;
    int numTareas = m->nTareas;
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
    for (int i = 0; i < numEstadisticas-nuevasEst; ++i)
    {
      Estadistica* nuevaEstadistica = new Estadistica;
      read(in, nuevaEstadistica, sizeof(*nuevaEstadistica));
      estadisticas[i] = nuevaEstadistica;
    }
    m->estadisticas = estadisticas;

    //Agregar nuevas estadisticas al mensaje
    for (int i=(numEstadisticas-vEstadisticas.size());i<numEstadisticas;++i)
    {
      m->estadisticas[i] = vEstadisticas.back();
      vEstadisticas.pop_back();
    }
    
    //Actualizar nEstadisticas
    m->nEstadisticas = numEstadisticas;
    
    //Asignar tareas a hilos libres
    for (int i = 0;i<numTareas; ++i) {
      //Si la tarea no esta asignada, darsela a un hilo
      if(!(m->tareas[i]->asignado) && (numProc > 0)){
        m->tareas[i]->asignado = true;
        tarea = m->tareas[i]->tareaAEjecutar;
        vsTareas.push_back(tarea);
        numProc--;
        sem_post(&semHilos);
      }
    }
    
    //Enviar mensaje
    write(out, m, sizeof(*m));

    //Enviar Tareas
    for (int i = 0; i < numTareas; ++i) {
      write(out, m->tareas[i], sizeof(*m->tareas[i]));
    }

    //Enviar Estadisticas
    for (int i = 0; i < numEstadisticas; ++i) {
      write(out, m->estadisticas[i], sizeof(*m->estadisticas[i]));
    }

    //Si nTareas = 0, terminar el PCP
    if(m->nTareas < 1){
      return 0;
    }

    for (int i = 0; i < numTareas; ++i) {
      delete tareas[i];
    }
    for (int i = 0; i < numEstadisticas; ++i) {
      delete estadisticas[i];
    }

    delete[] estadisticas;
    delete[] tareas;
    delete m;
    sem_post(&mutex);
  }

  return 0;
}