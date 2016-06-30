#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "Comandos.h"
#include "msg.h"

using namespace std;

int crearPCP(int p[], int in, int out) {

  int procNum; //Numero del proceso
  //El proceso planificador se reemplazara por PLP,

  //Se crea el primer pipe
  pipe(p);
  dup2(p[0],in);
  dup2(p[1],out);
  close(p[0]);
  close(p[1]);

  //Se crean los PCP con los pipes requeridos
  for(int i=0;i<nProcess;i++){
    //Nuevo pipe 
    pipe(p);
    int pid = fork();
    if(pid < 0){
      cout << "Error creando hijos";
      return 0;
    }else if(pid > 0){
      //Proceso padre
      dup2(p[1],out);
    }else{
      dup2(p[0],in);
    }
    close(p[0]);
    close(p[1]);
    //ProcNum = id del proceso en el anillo 
    procNum = i;
    if(pid){
      //Salir del ciclo en caso de ser el padre
      break;
    }
    //Id para el ultimo proceso
    procNum = i+1;
  }

  char *dir;
  if(procNum == 0){
   //Proceso padre se convertira en el PLP
   dir = getenv("PLN_DIR_PLP");
   string plp = dir + plpName;
   execl(plp.c_str(), plpName.c_str(),(char *)0);
 }else{
  //Los hijos se convertiran en PCP
  procNum--;
  dir = getenv("PLN_DIR_PCP");
  string pcp = "PCP";
  execl((dir+pcp).c_str(),"PCP", to_string(procNum).c_str(),
   to_string(npcp[procNum]).c_str(),(char *)0);
}

return 1;
}

int main(int argc, char ** argv) {

  int p[2];    //Para el pipe
  int nroSep;
  int i;
  string sentencia;
  string msg = "//mensaje ";
  vector<string> mensaje;

  /* Recorre cada uno de los argumentos y los
   * muestra en la salida estandar */
  for (i = 1; i < argc; i++) {
    sentencia = sentencia + argv[i] + " ";
  }

  nroSep = count(sentencia.begin(), sentencia.end(), '-');
  mensaje = split(sentencia, '-');
  if((bool) estaV(mensaje)) {
   crearPCP(p,0,1);
 }


 return 0;
}
