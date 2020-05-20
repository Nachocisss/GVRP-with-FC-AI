#include <bits/stdc++.h> 
#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
using namespace std;


// -------------------------------------------------------------------------
// ------------------------      solucion       ------------------------------
// -------------------------------------------------------------------------

int* recursive(int*,float,float,int,float,int);

float radian(float x){
   return (x * (atan(1)*4/ 180)) ; // atan(1)*4 = muy similar a PI
}

// ------------ harvesine ------------------
   float distanciaHaversine(float lat1, float lat2, float lon1, float lon2){
   float dif_lat, dif_lon;
   lat1 = radian(lat1);   lat2 = radian(lat2);  lon1 = radian(lon1);    lon2 = radian(lon2);        //pasar a radianes
   dif_lat = lat2 - lat1;    dif_lon = lon2 - lon1;

   float radioTierra = 4182.44949;
   float a = sin(dif_lat/2)*sin(dif_lat/2) + cos(lat1)*cos(lat2)*sin(dif_lon/2)*sin(dif_lon/2);
   float c = 2 * atan2(sqrt(a), sqrt(1-a));
    return radioTierra*c;
}

// -------------------------------------------------------------------------------------
//                            CODIGO PRINCIPAL GVRP 
// -------------------------------------------------------------------------------------

float matrizDistancias[100][100], mejortiempo = 10000000, velocidad, tiempoMaximo, distanciaMaxima,tiempoServicio, tiempoRecarga;
int cantidadClientes, cantidadEstaciones,  totalnodos, minimoEncontrado= 0;
int numerovisitados = 0, vehiculosEnviados = 1, parar= 0;
char mejorsolucion[1000]= "",solucionactual[1000]="";

int main() 
{ 
    fstream file; 
    string word, filename, id, b, c, d, e, f ,g ,h; 
    string dx, dy; 
    string nombre = "AB107";                    //  < ------------------ NOMBRE DE ARCHIVO !!!
    filename = nombre+".dat";                                           
    file.open(filename.c_str()); 

   //------------- PRIMERA LINEA DE ARCHIVO, CONVERSION A INT --------------
   
   file >> id;     file >> b;    file >> c;   file >> d;   file >> e;   file >> f;   file >> g;   file >> h;

   cantidadClientes = stoi(b);
   cantidadEstaciones = stoi(c);
   tiempoMaximo = stof(d);
   distanciaMaxima = stof(e);
   velocidad = stof(f);
   tiempoServicio = stof(g);
   tiempoRecarga = stof(h);
   totalnodos = cantidadClientes + cantidadEstaciones + 1 ;


   file >> word; file >> word; file >> dx; file >> dy;  
   float depox = stof(dx); float depoy = stof(dy);    //posicion deposito (depox, depoy)

 // ----------------------- LEER EL RESTO DEL ARCHIVO ------------------------

   string x, y; 
   float posicionX,posicionY;
   int status [cantidadClientes][4], nodoVisitado[cantidadClientes];
   float posicionesEstaciones[totalnodos][2] , auxiliar[cantidadEstaciones][2];


//         NODOS
   for (int i = 0; i < totalnodos; i++) {
      if (i < cantidadEstaciones){
         file >> word; file >> word; file >> x; file >> y;     //obtener posicion de estacion 
         posicionX = stof(x); posicionY = stof(y);        // castear a float
         auxiliar[i][0] = posicionX;
         auxiliar[i][1] = posicionY;
      }

      else {
        file >> word; file >> word; file >> x; file >> y;     //obtener posicion de clientes
        posicionX = stof(x); posicionY = stof(y);        // castear a float

        posicionesEstaciones[i-cantidadEstaciones+1][0] = posicionX;
        posicionesEstaciones[i-cantidadEstaciones+1][1] = posicionY;
      }
   }

   for (int i = 0; i < cantidadClientes; i++) {
      nodoVisitado[i] = 0;
   }

      posicionesEstaciones[0][0] = depox;
      posicionesEstaciones[0][1] = depoy;

   for (int i = 0; i < cantidadEstaciones; i++) {          // JUNTAR AMBOS ARREGLOS
      posicionesEstaciones[i + cantidadClientes+1][0] = auxiliar[i][0];
      posicionesEstaciones[i + cantidadClientes+1][1] = auxiliar[i][1];
   }

// -------------------------------------------------------------------------
// ------------------------      INSTANCIAS INICIALES       ------------------------------
// -------------------------------------------------------------------------

   float distancia;

// PASO 2: Creamos matriz distancia entre los nodos .

   for (int i = 0; i < totalnodos; i++) {
      for (int j = 0; j < totalnodos; j++) {
         if (i == j){            // nodo comparado con sigo mismo
            matrizDistancias[i][j] = -1;
         }
         else{
            distancia = distanciaHaversine(posicionesEstaciones[i][1],posicionesEstaciones[j][1],posicionesEstaciones[i][0],posicionesEstaciones[j][0]);
            if (distancia > distanciaMaxima or distancia == 0){
               distancia = -1;
            }
            matrizDistancias[i][j] = distancia;
         }
      }
   }
   

//PASO 3: Comenzamos a probar las soluciones
recursive(nodoVisitado, distanciaMaxima, tiempoMaximo, 0, 0, vehiculosEnviados);


//PASO 4: Registrar mejor resultado. Crear archivo de salida
ofstream myfile (nombre+".out");
myfile << mejortiempo << "     " << cantidadClientes << "    " <<vehiculosEnviados << "\n";

   for(int i = 0; mejorsolucion[i] != '\0'; i++){            // alamacenar solucion anterior
         myfile << mejorsolucion[i];
   }
   myfile << "\n";

myfile.close();

return 0;
} 

int* recursive(int visitados[], float bencina, float tiempo, int actual, float recorrido, int idVehiculo){
   int eliminar;
   char solucionAnterior[1000];
   if (parar == 10000){
      return visitados;
   }
   parar = parar +1;
      for (int nodocliente = 1 ; nodocliente < totalnodos; nodocliente++){           
         float posible = matrizDistancias[actual][nodocliente];               // DISTANCIA EN MATRIZ 

//---------------------------------------------------------------------------------------         
//-------------------------- candidato es estacion de recarga ---------------------------   
//--------------------------------------------------------|------------------------------   

         if (nodocliente > cantidadClientes){                 

             if (posible != -1 and (bencina - posible) > 0 and tiempo - (posible / velocidad) - tiempoRecarga > 0 and numerovisitados != cantidadClientes){     // cumple requisitos tiempo / distacia

                  int bencinaPrevio = bencina;
                  bencina = distanciaMaxima;
                  tiempo = tiempo - (posible / velocidad) - tiempoRecarga;                    
                  recorrido = recorrido + posible;                            // actualizar valores 

                  // --------------------------------- Escrirtura posible solucion  --------------------
                                 
                     char s1 = idVehiculo;  char s2 = nodocliente; int i;
                     std::string s = solucionactual + std::to_string(idVehiculo) + "->" + std::to_string(nodocliente) + "  |  " ;

                     for(i = 0; solucionactual[i] != '\0'; i++){            // alamacenar solucion anterior
                        solucionAnterior[i] = solucionactual[i];
                     }
                     solucionAnterior[i] = '\0';

                     for(i = 0; s[i] != '\0'; i++){                     //agregar nuevo paso
                        solucionactual[i] = s[i];
                     }
                     solucionactual[i] = '\0';

                  // --------------------------------- FIN ESCRITURA  ---------------------------------

                  recursive(visitados, bencina, tiempo, nodocliente, recorrido, vehiculosEnviados);         // Chequear siguiente paso

                  for(i = 0; solucionAnterior[i] != '\0'; i++){         // volver a solucion anterior
                        solucionactual[i] = solucionAnterior[i];
                     }
                     solucionactual[i] = '\0';

                  bencina = bencinaPrevio;                            // resetear valores para continuar iteraciones
                  tiempo = tiempo = tiempo + (posible / velocidad) + tiempoRecarga;
                  recorrido = recorrido - posible;  
               }
         }
//---------------------------------------------------------------------------------------         
//--------------------------       candidato es cliente ---------------------------------   
//--------------------------------------------------------|-------------------------------         

         else{                   
            if (visitados[nodocliente-1] == 0){               // nodo no visitado

               if (posible != -1 and (bencina - posible) > 0 and tiempo - (posible / velocidad) - tiempoRecarga and numerovisitados != cantidadClientes){     // cumple requisitos tiempo / distacia

                  visitados[nodocliente-1] = 1;   
                  numerovisitados = numerovisitados + 1 ;
                  bencina = bencina - posible;
                  tiempo = tiempo - (posible / velocidad) - tiempoServicio;
                  recorrido = recorrido + posible;                            // actualizar valores 

                  // --------------------------------- Escrirtura posible solucion  --------------------
                     char s1 = idVehiculo;  char s2 = nodocliente;  int i;
                     std::string s = solucionactual + std::to_string(idVehiculo) + "->" + std::to_string(nodocliente) + "  |  " ;

                     for(i = 0; solucionactual[i] != '\0'; i++){            // alamacenar solucion anterior
                        solucionAnterior[i] = solucionactual[i];
                     }
                     solucionAnterior[i] = '\0';

                     for(i = 0; s[i] != '\0'; i++){                     //agregar nuevo paso
                        solucionactual[i] = s[i];
                     }
                     solucionactual[i] = '\0';

                  // --------------------------------- FIN ESCRITURA  ---------------------------------

                  if (numerovisitados == cantidadClientes){         //si luego de revisar opciones, tengo todo chequeado, debo guardar solucion
                     minimoEncontrado = idVehiculo;
                     if(recorrido < mejortiempo){
                           mejortiempo = recorrido;

                           for(i = 0; solucionactual[i] != '\0'; i++){      //copiar ruta
                              mejorsolucion[i] = solucionactual[i];
                           }
                           mejorsolucion[i] = '\0';

                     }
                     visitados[nodocliente-1] = 0;                
                     numerovisitados = numerovisitados - 1 ;
                     return visitados;
                  }

                  recursive(visitados, bencina, tiempo, nodocliente, recorrido,vehiculosEnviados);           // Chequear siguiente paso

                  visitados[nodocliente-1] = 0;                                    
                  numerovisitados = numerovisitados - 1 ;
                  bencina = bencina + posible;
                  tiempo = tiempo + (posible / velocidad) + tiempoServicio;
                  recorrido = recorrido - posible;                            // // resetear valores para continuar iteraciones 


                  // ----------------------------- volver a escritura anterior---  --------------------
                  for(i = 0; solucionAnterior[i] != '\0'; i++){         // volver a solucion anterior
                        solucionactual[i] = solucionAnterior[i];
                     }
                     solucionactual[i] = '\0';
                  // --------------------------------- FIN ESCRITURA  ---------------------------------
                  }
               }
            }
         }
         
// fin iteraciones ! 
   if (minimoEncontrado != 0){                     //no mandar mas autos
      return visitados;
   }
   else{                                           // si no esta todo chequeado, debo mandar otro auto
      vehiculosEnviados = vehiculosEnviados + 1;
      recursive(visitados, distanciaMaxima, tiempoMaximo, 0, recorrido, vehiculosEnviados);
      return visitados;

      }
return visitados;
}
