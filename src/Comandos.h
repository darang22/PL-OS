#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>

using namespace std;

string plpName = "PLP"; 
int nProcess = 1;
int npcp[20];

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        item.erase(item.find_last_not_of(" \n\r\t")+1);
        elems.push_back(item);
    }
    return elems;
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

void imprimir(vector<string> &cadena) {
    for (vector<string>::const_iterator i = cadena.begin(); i != cadena.end(); ++i)
        cout << *i << endl;
}

bool esNumerico(string str) {
  long long i = 1;
  long long len = str.length();

  if(str[0] != '-') i = 0;

  for (; i < len; ++i)
  {
      if (!isdigit(str[i]))
      {
        return false;
      }
  }
  return true;
}

bool esPositivo(string str) {
  return str[0] == '-' ? false : true;
}

bool estaVacio(string str) {
    return str == "" ? true : false;
}

bool esValido(int nroSep, int item, string call) {

    int num;
    const int maxNroProcess = 20;
    const int maxThreads = 10;
    bool sw = true, esNum, esPos, pValid, thrValid, vacio, cumple;

    esNum = esNumerico(to_string(item));
    esPos = esPositivo(to_string(item));
    vacio = estaVacio(to_string(item));
    
    pValid = ((call == "procesos") & (item <= maxNroProcess) & (item >=1));
    thrValid = ((call == "hilos") & (item <= maxThreads) & (item >=1));
    
    cumple = ((pValid) | (thrValid));

    if(!esNum | !esPos | !cumple | vacio) {
        sw = false;
    }
    return sw;
}

void initNpcp(int nProcess) {
    int i = 0;
    for (; i < nProcess; ++i)
        npcp[i] = 3;
}

bool estaV(vector<string> &msg) {

    int len = msg.size();
    int nroThr, nroSep = 0;
    int id, cont = 1;
    bool sw = false;
    string elemt, isId;
    vector<string> x;
    
    for(int pos = 1; pos < msg.size(); ++pos) {

        bool esNum = esNumerico(msg[pos]);
        bool esPos = esPositivo(msg[pos]);
        bool vacio = estaVacio(msg[pos]);
            
        elemt = msg.at(pos);
        
        isId = elemt.substr(0,1);
        isId == "t" ? pos = 3 : cont++;
        if (isId != "l" && pos == 1)
            pos++;
        
        switch (pos) {
            case 1:
                sw = (!esNum & !vacio);
                plpName = elemt.substr(2, elemt.length());
                break;
            case 2:
                nroSep = count(elemt.begin(), elemt.end(), ' ');
                nProcess = stoi(elemt.substr(2,4));
                initNpcp (nProcess);
                if(plpName.compare("PLP") == 0){
                   pos--;
                }
                sw = esValido(nroSep, nProcess, "procesos");
                cout << sw << endl;
                break;
            case 3:
                nroSep = count(elemt.begin(), elemt.end(), ' ');
                id = stoi(elemt.substr(2,4));
                nroThr = stoi(elemt.substr(4,6));
                sw = esValido(nroSep, nroThr, "hilos");
                if(sw) npcp[id] = nroThr;
                pos = pos + (cont - pos);
                cont++;
                break;
            default:
                cout << "default";
                sw = false;
                break;
        }
        if(!sw) {
            return sw;
        }
    }
    return sw;
}