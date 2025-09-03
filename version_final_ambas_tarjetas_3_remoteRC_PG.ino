/*Version integrada del sistema de Adquisicion de datos para los Laboratorios de Fisica de la Universidad de Antioquia empleando las tarjetas inferior y superior.
Al momento se tienen siete funciones, pero se podran crear nuevas e incluirlas en el menu principal.
Tiempo muerto por envio serial: 0,07ms por caracter enviado (Usando baudrate=115200).
(No se han usado tildes para evitar problemas de compatibilidad entre sistemas operativos).
Programado por: Luis Felipe Ramirez Garcia, Instituto de Fisica, Universidad de Antioquia, 2016.
Esta versión incluye la posibilidad de retornar los valores de los puertos análogos y la temperatura cuando se recibe la letra 'a'(tarjeta superior) ó 'A' (tarjeta inferior) por el puerto serial. Para retornar al modo normal se debe recibir la letra 'B'.
Se ha añadido a las funciones fotocompuerta 1 y fotocompuerta 2 la posibilidad de escoger entre ms o us.
Este código esta cargado en todos los sistemas.
*/

#include <LiquidCrystal.h>

#define boton0 40 //Boton 0
#define boton1 39 //Boton 1
#define foto 9  //Senal del Photogate en USB1. En versiones posteriores del sistema se pueden cambiar los cables del photogate para que la señal de salida actue sobre el pin 7 (INT 2 del Chipkit)
#define foto1 11
#define estimulo 12 //Pin para estimular los circuitos RC y LC.
#define respuesta 0 //(A0) Pin para medir el voltaje en el capacitor.
//Pines para el protocolo de comunicación con la tarjeta de termocupla.
#define MISOS 6 //Pin 3 conector TEMP.
#define SCKS A10  //Pin 4 conector TEMP.
#define CSS A11  //Pin 10 conector TEMP.

int numero_funcion;
LiquidCrystal lcd(26, 28, 29, 30, 36, 37); //Define RS, E, D4, D5, D6, D7
char* funciones[]={" Fotocompuerta 1    ", " Fotocompuerta 2    ", " Doble Fotocompuerta", " T. oscuridad (ms)  ", " T. oscuridad (us)  ", " Sensores USB3,USB4 ", "Sensores A0 A1 A2 A3", " Termocupla tipo K  ", " Circuito RC - serie", " Informacion        "};      //Modificar de acuerdo con las funciones que se vayan definiendo.
boolean remote=false; //indica si se está trbajando con la interfaz gráfica

void setup(){
  pinMode(foto, INPUT);
  pinMode(foto1, INPUT);     // sets the digital pin 11 as input
  pinMode(boton0, INPUT);    // sets the digital pin 2 as input
  pinMode(boton1, INPUT);    // sets the digital pin 4 as input
  pinMode(MISOS, INPUT);      // Configures the communication pins for MAX6674 thermocouple chip.
  pinMode(SCKS, OUTPUT);
  pinMode(CSS, OUTPUT);
  pinMode(27, OUTPUT);
  digitalWrite(SCKS,0);       // Inicia el reloj deshabilitado.
  digitalWrite(CSS,1);        // Inicia el "Chip Select" en 1.
  digitalWrite(27,1);
  pinMode(estimulo, OUTPUT);
  digitalWrite(respuesta,0); 
  lcd.begin(20, 4);          // initializes LCD
  Serial.begin(115200);      // initializes Serial Port
  Serial.println("Presione P para iniciar la función Fotocompuerta 2"); //Para una prOxima versión se puede crear un menú para el modo remoto
  Serial.println("Presione R para iniciar el experimento con el circuito RC"); //Para una prOxima versión se puede crear un menú para el modo remoto
}


void loop(){
  delay(200);
  numero_funcion = menu_ppal();
  delay(200);
  switch(numero_funcion){ //Modificar de acuerdo con las funciones que se vayan definiendo.
    case 0:
      polea1();
      break;
    case 1:
      polea2();
      break;
    case 2:
      Doble_Photogate();
      break;
    case 3:
      oscuridad_ms();
      break;
    case 4:
      oscuridad_us();
      break;
    case 5:
     dato_adc();
     break;
    case 6:
     dato_adc2();
     break; 
    case 7:
     termocupla();
     break;
    case 8:
     circuito_RC();
     break;
    case 9:
     informacion();
     break;
    default:
     break;
  }
}


//FUNCIONES
//--------------------------------------------------------------------------------------
// Menu principal
int menu_ppal(){
  int i=0;
  
  lcd.setCursor(0, 0);
  lcd.print("SELECT         ENTER");
  lcd.setCursor(0, 1);
  lcd.print(funciones[0]);  
  lcd.setCursor(0, 3);
  lcd.print("Lab. de fisica, UdeA"); 
  
  while(digitalRead(boton1) == 1){
    if(digitalRead(boton0) != 1){
      i=i+1; 
      if(i==10){       //Modificar de acuerdo con el numero de funciones.
        i=0;  
      }
      lcd.setCursor(0, 1);
      lcd.print(funciones[i]);       
      delay(400);
     }
  if(Serial.available()!=0){
    analiza_recibido(); //analiza recibido cambia el estado de la variable 'remote'.
  }
  while(remote==true){
    if(Serial.available()!=0){
       analiza_recibido(); //analiza recibido cambia el estado de la variable 'remote'.
    }
  }//Si está en modo remoto el programa se queda en este punto.
  }
  lcd.clear();
  delay(200);
  while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna la funcion escogida.
  return i;  //Devuelve el numero de la funcion que se haya escogido
}



//---------------------------------------------------------------------------------------
//Dato ADC
//Muestra cada 1s el valor de los dos ADC conectados a los puertos USB3 y USB4
void dato_adc(){
  
  int voltajeUSB3;
  int voltajeUSB4;
  int j=3;
  int i=1000;
  char* opciones[]= {" 1 ms               ", " 10 ms              ", " 100 ms             ", " 1 s                "};
  int boton=0;
  
  lcd.setCursor(0, 0);
  lcd.print("SELECT         ENTER");
  lcd.setCursor(0, 2);
  lcd.print("Tiempo entre datos: ");
  lcd.setCursor(0, 3);
  lcd.print(opciones[3]);
  
  while(digitalRead(boton1) == 1){
    if(digitalRead(boton0) != 1){
      j=j+1;
      i=i*10;
      if(j==4){       //Modificar de acuerdo con el numero de opciones.
        j=0;
        i=1;  
      }
      lcd.setCursor(0, 3);
      lcd.print(opciones[j]);       
      delay(400);
     }
  }
  lcd.clear();
  delay(200);
  while(digitalRead(boton1)!=1){} //Hasta que no suelte el boton no continua.
  delay(200);
  lcd.setCursor(0, 0);
  lcd.print("               STOP ");
  lcd.setCursor(0,1);
  lcd.print(" Valor ADC (10 bits)");
  lcd.setCursor(0,2);
  lcd.print("USB3     USB4");
  Serial.println("USB3\t \tUSB4");
  while(boton != 1){
   voltajeUSB3= analogRead(A8);
   voltajeUSB4= analogRead(A6);
   lcd.setCursor(0,3);
   lcd.print(voltajeUSB3);
   lcd.print("   ");
   lcd.setCursor(9,3);
   lcd.print(voltajeUSB4);
   lcd.print("   ");
   Serial.print(voltajeUSB3);
   Serial.print("\t \t");
   Serial.println(voltajeUSB4);
   boton = delay_int(i); //Fija tiempo entre datos y estado del boton 1.
  }
  Serial.println("FIN");
  lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
  delay(200);
  while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
  return;
}

//---------------------------------------------------------------------------------------
//Dato ADC2
//Muestra cada 1s el valor de los dos ADC conectados a los puertos A0, A1, A2, A3
void dato_adc2(){
  
  int voltajeA0;
  int voltajeA1;
  int voltajeA2;
  int voltajeA3;
  int j=3;
  int i=1000;
  char* opciones[]= {" 1 ms               ", " 10 ms              ", " 100 ms             ", " 1 s                "};
  int boton=0;
  
  lcd.setCursor(0, 0);
  lcd.print("SELECT         ENTER");
  lcd.setCursor(0, 2);
  lcd.print("Tiempo entre datos: ");
  lcd.setCursor(0, 3);
  lcd.print(opciones[3]);
  
  while(digitalRead(boton1) == 1){
    if(digitalRead(boton0) != 1){
      j=j+1;
      i=i*10;
      if(j==4){       //Modificar de acuerdo con el numero de opciones.
        j=0;
        i=1;  
      }
      lcd.setCursor(0, 3);
      lcd.print(opciones[j]);       
      delay(400);
     }
  }
  lcd.clear();
  delay(200);
  while(digitalRead(boton1)!=1){} //Hasta que no suelte el boton no continua.
  delay(200);
  lcd.setCursor(0, 0);
  lcd.print("               STOP ");
  lcd.setCursor(0,1);
  lcd.print(" Valor ADC (10 bits)");
  lcd.setCursor(0,2);
  lcd.print("A0  |A1  |A2  |A3   ");
  Serial.println("A0\t \tA1\t \tA2\t \tA3");
  while(boton != 1){
   voltajeA0= analogRead(A0);
   voltajeA1= analogRead(A1);
   voltajeA2= analogRead(A2);
   voltajeA3= analogRead(A3);
   lcd.setCursor(0,3);
   lcd.print(voltajeA0);
   lcd.print("   ");
   lcd.setCursor(4,3);
   lcd.print("|");
   lcd.print(voltajeA1);
   lcd.print("   ");
   lcd.setCursor(9,3);
   lcd.print("|");
   lcd.print(voltajeA2);
   lcd.print("   ");
   lcd.setCursor(14,3);
   lcd.print("|");
   lcd.print(voltajeA3);
   lcd.print("   ");
   
   Serial.print(voltajeA0);
   Serial.print("\t \t");
   Serial.print(voltajeA1);
   Serial.print("\t \t");
   Serial.print(voltajeA2);
   Serial.print("\t \t");
   Serial.println(voltajeA3);
   boton = delay_int(i); //Fija tiempo entre datos y estado del boton 1.
  }
  Serial.println("FIN");
  lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
  delay(200);
  while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
  return;
}

//----------------------------------------------------------------------------------------
//Fotocompuerta 1
//Envia el valor del tiempo transcurrido cada vez que hay una transición claro-oscuro en la polea conectada al puerto USB1
void polea1(){
 int flag_start = 0;
 int start_time;
 int counter = 0; //Para contar los eventos detectados cuando se usa la opcion de us.
 unsigned long times[1000]; //Arreglo donde se guardan los tiempos detectados.
  int j=0;
  char* opciones[]= {" ms                ", " us                "};
  int boton=0;

  lcd.setCursor(0, 0);
  lcd.print("SELECT         ENTER");
  lcd.setCursor(0, 2);
  lcd.print("Seleccione unidades:");
  lcd.setCursor(1, 3);
  lcd.print(opciones[j]);
  
  while(digitalRead(boton1) == 1){
    if(digitalRead(boton0) != 1){
      j=j+1;
      if(j==2){       //Modificar de acuerdo con el numero de opciones.
        j=0; 
      }
      lcd.setCursor(1, 3);
      lcd.print(opciones[j]);       
      delay(400);
     }
  }
  lcd.clear();
  delay(200);
  while(digitalRead(boton1)!=1){} //Hasta que no suelte el boton no continua.
  delay(200);
 
 lcd.setCursor(0,0);
 lcd.print("               STOP ");
 lcd.setCursor(0,2);
 lcd.print("  FOTOCOMPUERTA 1   ");
 lcd.setCursor(0,3);
 lcd.print("TOMANDO DATOS (USB1)");
 
 if(j==0){
   Serial.println("Tiempos de transicion claro -> oscuro (ms)");
   start_time = millis();
 
   while(digitalRead(boton1) == 1){
     if(digitalRead(foto)==0 && flag_start==0){  
      Serial.println(millis()-start_time);
      flag_start = 1;
     }
     if(digitalRead(foto)==1){
      flag_start = 0;
     }
   }
   Serial.println("FIN");
   lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
   delay(200);
   while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
   return;
 } else{ //En este caso los debe guardar en un arreglo y despues enviarlos por el puerto serial, para evitar el tiempo muerto por envio de cada caracter.
   Serial.println("Tiempos de transicion claro -> oscuro (us)");
   Serial.println("(Se esperan 1000 eventos o  que se pulse el boton STOP antes de mostrar los tiempos respectivos)");
   start_time = micros();
   while(digitalRead(boton1) == 1 && counter<999){
     if(digitalRead(foto)==0 && flag_start==0){
       times[counter]=micros()-start_time;
       flag_start = 1;
       counter++;
     }
     if(digitalRead(foto)==1){
       flag_start = 0;
     }
  }
  for(int i=0;i<counter;i++){
    Serial.println(times[i]); 
  }
  Serial.println("FIN");
  lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
  delay(200);
  while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
  return; 
 }
}
 
 //--------------------------------------------------------------------------------------------
 //Polea 2
 //Envia el valor del tiempo transcurrido cada vez que hay una transición claro-oscuro u oscuro-claro en la polea conectada al puerto USB0
 void polea2(){
 int foto_actual=0;
 int foto_anterior =1;
 int start_time;
 int counter = 0; //Para contar los eventos detectados cuando se usa la opcion de us.
 unsigned long times[1000]; //Arreglo donde se guardan los tiempos detectados.
 int j=0;
 char* opciones[]= {" ms                ", " us                "};
 int boton=0;

  lcd.setCursor(0, 0);
  lcd.print("SELECT         ENTER");
  lcd.setCursor(0, 2);
  lcd.print("Seleccione unidades:");
  lcd.setCursor(1, 3);
  lcd.print(opciones[j]);
  
  while(digitalRead(boton1) == 1){
    if(digitalRead(boton0) != 1){
      j=j+1;
      if(j==2){       //Modificar de acuerdo con el numero de opciones.
        j=0; 
      }
      lcd.setCursor(1, 3);
      lcd.print(opciones[j]);       
      delay(400);
     }
  }
  lcd.clear();
  delay(200);
  while(digitalRead(boton1)!=1){} //Hasta que no suelte el boton no continua.
  delay(  200);
  
  lcd.setCursor(0,0);
  lcd.print("               STOP ");
  lcd.setCursor(0,2);
  lcd.print("  FOTOCOMPUERTA 2   ");
  lcd.setCursor(0,3);
  lcd.print("TOMANDO DATOS (USB1)");
  if(j==0){
  Serial.println("Tiempos de transicion claro <-> oscuro (ms)");
  start_time = millis();
 
  while(digitalRead(boton1) == 1){
   foto_actual=digitalRead(foto);
   if(foto_actual-foto_anterior != 0){  
     Serial.println(millis()-start_time);
   }
   foto_anterior=foto_actual;
  }
  Serial.println("FIN");
  lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
  delay(200);
  while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
  return;
  }else{
   Serial.println("Tiempos de transicion claro <-> oscuro (us)");
   Serial.println("(Se esperan 1000 eventos o  que se pulse el boton STOP antes de mostrar los tiempos respectivos)");
   start_time = micros();
   while(digitalRead(boton1) == 1 && counter<999){
     foto_actual=digitalRead(foto);
     if(foto_actual-foto_anterior != 0){  
       times[counter]=micros()-start_time;
       counter++;
     }
     foto_anterior=foto_actual;
    }
   for(int i=0;i<counter;i++){
     Serial.println(times[i]); 
   } 
  Serial.println("FIN");
  lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
  delay(200);
  while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
  return;  
  }
 }

 //-----------------------------------------------------------------------------------------------
 //Tiempo de paso de objeto entre dos fotocompuertas
 //" Doble fotocompuerta" 
 //Envia el tiempo transcurrido entre dos obstrucciones sucesivas a las fotocompuertas
void Doble_Photogate(){
  unsigned long t1, t2, delta_t;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("               STOP ");
  lcd.setCursor(0,2);
  lcd.print(" DOBLE FOTOCOMPUERTA");
  lcd.setCursor(0,3);
  lcd.print("Datos de USB1 y USB2");
  while(digitalRead(boton1)==1){ //mientras que el boton 1 no sea presionado
     if (digitalRead(foto)==1 && digitalRead(foto1)== 0){
         Serial.println("Primero");
         lcd.setCursor(1, 1);
         lcd.print("DOBLE FOTOCOMPUERTA");
         lcd.setCursor(0, 2);
         lcd.print("    USB2 -> USB1     ");
         lcd.setCursor(0, 3);
         lcd.print("      Espere...      ");
         t1=micros();
         while(digitalRead(foto)==1 && digitalRead(boton1)== 1){
          t2= micros();
          }
         delta_t=t2-t1;
         lcd.setCursor(0, 3);
         lcd.print("      ");
         lcd.print(delta_t);
         lcd.print("  us         "); 
         Serial.println(delta_t);
         while(digitalRead(foto1)==1 && digitalRead(foto)== 0){}
         Serial.println("Salio1");
         
        }
      if (digitalRead(foto)==0 && digitalRead(foto1)== 1){
         Serial.println("segundo");
         lcd.setCursor(1, 1);
         lcd.print("DOBLE FOTOCOMPUERTA");
         lcd.setCursor(0, 2);
         lcd.print("    USB1 -> USB2     ");
         lcd.setCursor(0, 3);
         lcd.print("      Espere...      ");
         t1=micros();
         while(digitalRead(foto1)==1 && digitalRead(boton1)== 1){
          t2= micros();
          }
          delta_t=t2-t1;
          lcd.setCursor(0, 3);
          lcd.print("      ");
          lcd.print(delta_t);
          lcd.print("  us         ");
          Serial.println(delta_t);
          while(digitalRead(foto)==1 && digitalRead(foto1)==0 ){}
          Serial.println("Salio2");
          
        }
      
    }
  Serial.println("FIN");
  lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
  delay(200);
  while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
  return;
} 
 
 //-----------------------------------------------------------------------------------------------
 //Tiempo de oscuridad (ms)
 //Envia el tiempo que demora una interrupción del foto sensor conectado en USB0.
void oscuridad_ms(){

unsigned long t1, t2, delta_t;

lcd.setCursor(0,0);
lcd.print("               STOP ");
lcd.setCursor(0,1);
lcd.print("  T. OSCURIDAD (ms) ");
lcd.setCursor(0,2);
lcd.print("       (USB1)       ");

Serial.println("Tiempo de oscuridad (ms)");

while(digitalRead(boton1)==1){
 if(digitalRead(foto)==0){
     t1 = millis();
     while (digitalRead(foto)==0 && digitalRead(boton1)== 1){}
     t2 = millis();
     delta_t = t2-t1;
     lcd.setCursor(0, 3);
     lcd.print(delta_t);
     lcd.print("     ");
     Serial.println(delta_t);
 } 
}
Serial.println("FIN"
);
lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
delay(200);
while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
return; 
}

//-----------------------------------------------------------------------------------------------
//Tiempo de oscuridad (us)
//Envia el tiempo que demora una interrupción del foto sensor conectado en USB0.
void oscuridad_us(){

unsigned long t1, t2, delta_t;

lcd.setCursor(0,0);
lcd.print("               STOP ");
lcd.setCursor(0,1);
lcd.print("  T. OSCURIDAD (us) ");
lcd.setCursor(0,2);
lcd.print("       (USB1)       ");

Serial.println("Tiempo de oscuridad (us)");

while(digitalRead(boton1)==1){
 if(digitalRead(foto)==0){
     t1 = micros();
     while(digitalRead(foto)==0 && digitalRead(boton1)== 1){}
     t2= micros();
     delta_t = t2 - t1;//suprimir esto, ya que no hace nada.
     lcd.setCursor(0, 3);
     lcd.print(delta_t);
     lcd.print("          ");
     Serial.println(delta_t);
 } 
}
Serial.println("FIN");
lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
delay(200);
while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
return; 
}

//---------------------------------------------------------------------------------------------------
//Termocupla tipo K
void termocupla(){

  int j=0;
  char* opciones[]= {" MAX6674           ", " MAX6675           "};
  double temp_C;
  int boton=0;

  lcd.setCursor(0, 0);
  lcd.print("SELECT         ENTER");
  lcd.setCursor(0, 2);
  lcd.print("Tipo de amplificador");
  lcd.setCursor(0, 3);
  lcd.print(":");
  lcd.setCursor(1, 3);
  lcd.print(opciones[j]);
  
  while(digitalRead(boton1) == 1){
    if(digitalRead(boton0) != 1){
      j=j+1;
      if(j==2){       //Modificar de acuerdo con el numero de opciones.
        j=0; 
      }
      lcd.setCursor(1, 3);
      lcd.print(opciones[j]);       
      delay(400);
     }
  }
  lcd.clear();
  delay(200);
  while(digitalRead(boton1)!=1){} //Hasta que no suelte el boton no continua.
  delay(200);
  lcd.setCursor(0, 0);

  lcd.print("               STOP ");
  lcd.setCursor(0,2);
  lcd.print("  Temperatura (");
  lcd.write(B11011111);    //Simbolo de grado aceptado por el LCD.
  lcd.print("C) ");

  Serial.print("Temperatura (");
  Serial.write(167); //Codigo Ascii del simbolo de grado
  Serial.println("C)");
  while(boton != 1){
   if(j==0){
   temp_C=lee_temp6674()*0.125;
   lcd.setCursor(0,3);
   lcd.print(temp_C);
   lcd.print(" ");
   lcd.write(B11011111); 
   lcd.print("C");
   lcd.print("  ");
   Serial.println(temp_C);
   boton = delay_int(1000); //Tiempo entre datos y retorna estado del boton 1.
   }else{
    temp_C=lee_temp6675()*0.25;
    lcd.setCursor(0,3);
    lcd.print(temp_C);
    lcd.print(" ");
    lcd.write(B11011111); 
    lcd.print("C");
    lcd.print("  ");
    Serial.println(temp_C);
    boton = delay_int(1000); //Tiempo entre datos y retorna estado del boton 1.  
   } 
  }
  Serial.println("FIN");
  lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
  delay(200);
  while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
  return; 
}

//---------------------------------------------------------------------------------------------------
//Circuito RC

void circuito_RC(){
  
  unsigned long start_time=0;

  lcd.setCursor(0,0);
  lcd.print("               STOP ");
  lcd.setCursor(0,1);
  lcd.print("R->Pin 12, C->Pin A0");
  lcd.setCursor(0,3);
  lcd.print("Cargando capacitor..");
  Serial.println("");
  Serial.println("Cargando capacitor >>>>>");
  Serial.println("Tiempo (us) \t Voltaje (V)");
  digitalWrite(estimulo,1);
  start_time=micros();
  while(analogRead(respuesta)<1010 && digitalRead(boton1)==1){ //Puede disminuirse el umbral.
    Serial.print((micros() - start_time));
    Serial.print("\t");
    Serial.println(analogRead(respuesta)*3.3/1023,2);
  }
  lcd.setCursor(0,3);
  lcd.print("Descargando capac...");
  Serial.println("Descargando capacitor <<<<<");
  Serial.println("Tiempo (us) \t Voltaje (V)");
  digitalWrite(estimulo,0);
  start_time=micros();
  while(analogRead(respuesta)>0 && digitalRead(boton1)==1){ //Puede aumentarse el umbral.
    Serial.print((micros() - start_time));
    Serial.print("\t");
    Serial.println(analogRead(respuesta)*3.3/1023,2);
 }
  Serial.println("FIN");
  lcd.clear(); //Todas las funciones deben borrar la pantalla al terminar
  while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
  return;
}

//////////////FUNCIONES PARA EL MODO REMOTO
//----------------------------------
void circuito_RC_remote(){
  
  unsigned long start_time=0;

  Serial.println("");
  Serial.println("Cargando capacitor, presione una tecla para cancelar >>>>>");
  Serial.println("Tiempo (us) \t Voltaje (V)");
  digitalWrite(estimulo,1);
  start_time=micros();
  while(analogRead(respuesta)<1010 && digitalRead(boton1)==1 && Serial.available()==0){ //Puede disminuirse el umbral.
    Serial.print((micros() - start_time));
    Serial.print("\t");
    Serial.println(analogRead(respuesta)*3.3/1023,2);
  }
  Serial.read();
  Serial.println("Descargando capacitor, presione una tecla para cancelar <<<<<");
  Serial.println("Tiempo (us) \t Voltaje (V)");
  digitalWrite(estimulo,0);
  start_time=micros();
  while(analogRead(respuesta)>0 && digitalRead(boton1)==1 && Serial.available()==0){ //Puede aumentarse el umbral.
    Serial.print((micros() - start_time));
    Serial.print("\t");
    Serial.println(analogRead(respuesta)*3.3/1023,2);
 }
  Serial.read();
  Serial.println("FIN");
  Serial.println("Presione P para iniciar la función Fotocompuerta 2"); //Para una prOxima versión se puede crear un menú para el modo remoto
  Serial.println("Presione R para iniciar el experimento con el circuito RC"); //Para una prOxima versión se puede crear un menú para el modo remoto
  return;
}

////////////////////////////////////
void remote_photogate(){ //Version remota de polea2()
 int foto_actual=0;
 int foto_anterior =1;
 int start_time;
 int counter = 0; //Para contar los eventos detectados cuando se usa la opcion de us.
 unsigned long times[1000]; //Arreglo donde se guardan los tiempos detectados.
 int j=0;
 int boton=0;

 Serial.println("Presione 'm' para medir en milisegundos o 'u' para medir en microsegundos");
 while(Serial.available()==0); //Espera hasta que ingrese el caracter.
 char recibido=Serial.read();
  if(recibido=='m'){
  Serial.println("Función iniciada. Presione cualquier tecla para terminar.");
  Serial.println("Tiempos de transicion claro <-> oscuro (ms)");
  start_time = millis();
 
  while(Serial.available()==0){
   foto_actual=digitalRead(foto);
   if(foto_actual-foto_anterior != 0){  
     Serial.println(millis()-start_time);
   }
   foto_anterior=foto_actual;
  }
  Serial.read(); //Para desocupar el búfer serial
  Serial.println("FIN");
  Serial.println("Presione P para iniciar la función Fotocompuerta 2"); //Para una prOxima versión se puede crear un menú para el modo remoto
  Serial.println("Presione R para iniciar el experimento con el circuito RC"); //Para una prOxima versión se puede crear un menú para el modo remoto
  return;
  }else if(recibido=='u'){
   Serial.println("Función iniciada. Presione cualquier tecla para terminar.");
   Serial.println("Tiempos de transicion claro <-> oscuro (us)");
   Serial.println("(Se esperan 1000 eventos o  que se pulse cualquier tecla antes de mostrar los tiempos respectivos)");
   start_time = micros();
   while(Serial.available() == 0 && counter<999){
     foto_actual=digitalRead(foto);
     if(foto_actual-foto_anterior != 0){  
       times[counter]=micros()-start_time;
       counter++;
     }
     foto_anterior=foto_actual;
    }
   for(int i=0;i<counter;i++){
     Serial.println(times[i]); 
   }
  Serial.read(); 
  Serial.println("FIN");
  Serial.println("Presione P para iniciar la función Fotocompuerta 2"); //Para una prOxima versión se puede crear un menú para el modo remoto
  Serial.println("Presione R para iniciar el experimento con el circuito RC"); //Para una prOxima versión se puede crear un menú para el modo remoto
  return;  
  }else {
    Serial.read();
    Serial.println("Opción no válida... Regresando al menú principal");
    Serial.println("Presione P para iniciar la función Fotocompuerta 2"); //Para una prOxima versión se puede crear un menú para el modo remoto
    Serial.println("Presione R para iniciar el experimento con el circuito RC"); //Para una prOxima versión se puede crear un menú para el modo remoto
    return;
  }
 }

//------------------------------------------------------------------------------------------------------
//Informacion

void informacion(){
  Serial.println("Sistema de Adquisición de datos para los laboratorios de física básica de la Universidad de Antioquia");
  Serial.println("");
  Serial.println("Encuentre la guia del usuario y aplicaciones adicionales en:");
  Serial.println("http://tinyurl.com/hyonw8a");
  lcd.setCursor(0,0);
  lcd.print("               EXIT ");
  lcd.setCursor(0,1);
  lcd.print("Lab. de Fisica, UdeA");
  lcd.setCursor(0,2);
  lcd.print("Manual y software en");
  lcd.setCursor(0,3);
  lcd.print("tinyurl.com/hyonw8a ");
  while(digitalRead(boton1)==1){};
  Serial.println("--");
  lcd.clear();
  while(digitalRead(boton1)==0){} //Hasta que no suelte el boton no retorna al menu.
  return;
}


///////////////////////////////////////////FUNCIONES AUXILIARES///////////////////////////////////////////

///// lee_temp6674(); Ejecuta el protocolo necesario para leer el valor de la temperatura (10bits con incrementos correspondientes a 0.125°C) con el chip MAX6674

int lee_temp6674(){

int i=0;
int bit_leido;
int dato_actual;
int dato_acumulado=0;
int temperatura;

digitalWrite(CSS,0);
delayMicroseconds(10);

for (i==0; i<16; i++)
  {
    digitalWrite(SCKS,1);
    delayMicroseconds(10); //Se fija SCK en 50KHz
    bit_leido=digitalRead(MISOS); //Se va construyendo bit a bit el dato de 16 bits arrojado por el chip MAX6674
    dato_actual=bit_leido << (15-i); //Se guarda bit_leido en la posición correspondiente de dato_actual.
    dato_acumulado=dato_actual | dato_acumulado;
    digitalWrite(SCKS,0);
    delayMicroseconds(10); 
  }
digitalWrite(CSS, 1);
delayMicroseconds(10);
temperatura=((dato_acumulado>>5) & 1023); //La temperatura está en los bits 14->5 de dato_acumulado. Temperatura es una variable entre 0 y 1023, que representa incrementos de 0.125°C
return temperatura;
}

///// lee_temp6675(); Ejecuta el protocolo necesario para leer el valor de la temperatura (12bits con incrementos correspondientes a 0.25°C) con el chip MAX6675
int lee_temp6675(){

int i=0;
int bit_leido;
int dato_actual;
int dato_acumulado=0;
int temperatura;

digitalWrite(CSS,0);
delayMicroseconds(10);

for (i==0; i<16; i++)
  {
    digitalWrite(SCKS,1);
    delayMicroseconds(10); //Se fija SCKS en 50KHz
    bit_leido=digitalRead(MISOS); //Se va construyendo bit a bit el dato de 16 bits arrojado por el chip MAX6675
    dato_actual=bit_leido << (15-i); //Se guarda bit_leido en la posición correspondiente de dato_actual.
    dato_acumulado=dato_actual | dato_acumulado;
    digitalWrite(SCKS,0);
    delayMicroseconds(10); 
  }
digitalWrite(CSS, 1);
delayMicroseconds(10);
temperatura=((dato_acumulado>>3) & 4095); //La temperatura está en los bits 14->3 de dato_acumulado. Temperatura es una variable entre 0 y 4095, que representa incrementos de 0.25°C
return temperatura;
}







/////// delay_int; Es una rutina de retardo que termina si se pulsa el boton 1 (aunque no se haya cumplido el tiempo).
int delay_int(int t){
int i=0;
for(i==0;i<t;i++){
  delay(1);
  if(digitalRead(boton1) == 0){
  return 1; //Indica que el retorno se debe a la pulsación del botón 1.
  }
}
return 0; //Indica que el retorno se debe al cumplimiento del tiempo.
}


//////Evento de recepción del puerto serial. Se usa para detectar el caracter 'A' enviado por la interfaz gráfica del usuario y devolver el valor de los sensores análogos y la temperatura medida con la termocupla.

void analiza_recibido(){
  char recibido=0;
  recibido=Serial.read();
  
  if (recibido=='A'){
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("     Modo remoto    "); 
    Serial.print(analogRead(A8));
    Serial.print("\t");
    Serial.print(analogRead(A6));
    Serial.print("\t");
    Serial.println(lee_temp6674()*0.125);
    remote=true;  
  }else if (recibido=='a'){
     lcd.clear();
     lcd.setCursor(0, 1);
     lcd.print("     Modo remoto    "); 
     Serial.print(analogRead(A0));
     Serial.print("\t");
     Serial.print(analogRead(A1));
     Serial.print("\t");
     Serial.print(analogRead(A2));
     Serial.print("\t");
     Serial.print(analogRead(A3));
     Serial.print("\t");
     Serial.println(lee_temp6674()*0.125);
     remote=true; 
   }else if (recibido=='R'){
     lcd.clear();
     lcd.setCursor(0, 1);
     lcd.print("     Modo remoto    ");
     remote=true;
     circuito_RC_remote();
   }else if (recibido=='P'){
     lcd.clear();
     lcd.setCursor(0, 1);
     lcd.print("     Modo remoto    ");
     remote=true;
     remote_photogate();
   }else if (recibido=='B'){
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("SELECT         ENTER");
     lcd.setCursor(0, 1);
     lcd.print(funciones[0]);  
     lcd.setCursor(0, 3);
     lcd.print("Lab. de fisica, UdeA");
     remote=false;
   }
}
