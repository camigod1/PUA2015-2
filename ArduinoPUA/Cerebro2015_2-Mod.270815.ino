//---------------------------------------------------------------------------
// Cerebro PUA 2015-2
// Camilo Gomez Duarte
// Universidad de los Andes
//---------------------------------------------------------------------------
// Modificaciones:
// Formato: Mod. <fecha> <descripcion>
// Nicolás Rocha P. 27/08/15 
// Posibilidad de guardar lecturas de datos en una tarjeta SD.
// Cambios de pines. 
//---------------------------------------------------------------------------
/*
            ____  __  _____   ___   ____ _________     ___ 
           / __ \/ / / /   | |__ \ / __ <  / ____/    |__ \
          / /_/ / / / / /| | __/ // / / / /___ \________/ /
         / ____/ /_/ / ___ |/ __// /_/ / /___/ /_____/ __/ 
        /_/    \____/_/  |_/____/\____/_/_____/     /____/ 
                                                 
*/

#include <SoftwareSerial.h>
#include <SPI.h>      //Mod. 27/08/15 inclusion de las librerias para salvar los datos de la memoria.
#include <SD.h>

//*********************************************
//             CONSTANTES
//*********************************************
    String MESSAGE_ABORTAR = "ABORTAR";
    String MESSAGE_TESTBT = "TESTBT";
    String MESSAGE_TESTRF = "TESTRF\0";
    String MESSAGE_DIEZ = "DIEZ\0";
    String MESSAGE_TRES= "TRES\0";
    String MESSAGE_UNO = "UNO\0";

    int TIEMPO_IG_0 = 0;
    int TIEMPO_IG_1 = 10000;
    int TIEMPO_IG_2 = 10000;
    int TIEMPO_IG_3 = 10000;
    
    int ignitor0 = 8;  //Mod. 27/08/15 Cambio de pines de los sensores 7, 6, 5 y 4 a 8, 7, 6 y 5
    int ignitor1 = 7;
    int ignitor2 = 6;
    int ignitor3 = 5;
    
    const int chipSelect = 4;
    
//*********************************************
//             ATRIBUTOS
//*********************************************
    SoftwareSerial mySerial(2, 3);        // Se asignan los puertos RX y TX. Mod. 27/08/15 Cambio de pines 10 y 11 a los pines 2 y 3.
    String bluetoothData = "";            // Datos recibidos por Bluetooth
    boolean abortar;

//*********************************************
//            IMPLEMENTACION
//*********************************************

/*
 *  Metodo en donde se establece la comunicacion serial
 */
void setup() 
{
  mySerial.begin(9600);
  abortar = false;
  mySerial.println("CONECTADO");
  pinMode( ignitor0, OUTPUT );
  pinMode( ignitor1, OUTPUT );
  pinMode( ignitor2, OUTPUT );
  pinMode( ignitor3, OUTPUT );
 
  digitalWrite( ignitor0, LOW );
  digitalWrite( ignitor1, LOW );
  digitalWrite( ignitor2, LOW );
  digitalWrite( ignitor3, LOW );
  
  // Mod. 27/08/15 Inicializacion de la tarjeta SD
  if (!SD.begin(chipSelect)) {
    mySerial.println("No se pudo inicializar la tarjeta");
    return;
  }
  mySerial.println("Tarjeta no inicializada...");
}

/*
 *  Metodo que lee la palabra bluetooth y elige que hacer segun el comando
 */
void loop() 
{
   while ( mySerial.available() )
    {
        char character = mySerial.read(); // Recuperar un caracter del puerto serial
        bluetoothData.concat(character);  // Concatenar caracter a dato completo
        if (character == '0')
        {
            //mySerial.print("Comando recibido: ");
            //mySerial.println(bluetoothData);
            comandoRecibido();
            bluetoothData = "";            // Borrar el dato para poder recibir el siguiente
        }
    }
}

/*
 *  Metodo que reconoce el comando recibido y ejecuta alguna accion frente al mismo
 */
void comandoRecibido( )
{
      //mySerial.println( "Revisando Comando" + bluetoothData);
      
      if( bluetoothData.startsWith(MESSAGE_ABORTAR , 0))
      {
        procesoAbortar();
      }
   
      else if( bluetoothData.startsWith(MESSAGE_TESTBT, 0) )
      {
        mySerial.println( "TestBT: Bluetooth funcionando correctamente" );
      }
      
      else if( bluetoothData.startsWith( MESSAGE_TESTRF, 0 ) )
      {
        mySerial.println( "TestRF: " );
      }
      
      else if( bluetoothData.startsWith( MESSAGE_DIEZ, 0 ) )
      {
        mySerial.println( "Diez (10)" );
      }
      
      else if( bluetoothData.startsWith(MESSAGE_TRES, 0) )
      {
         mySerial.println( "Tres (3)" );
      }
      
      else if( bluetoothData.startsWith("UNO", 0) )
      {
         mySerial.println( "Uno (1)" );
         procesoDespegar();
         
      }
      
      else
      {
        mySerial.println( "ERROR: Comando " +bluetoothData +" no reconocido" );
      }
}

/*
 *  Metodo se encarga abortar el despegue.
 *  1. Apaga los pines de los ignitores.
 *  2. Notifica
 */
void procesoAbortar()
{
  abortar = true;
  digitalWrite( ignitor0, LOW );
  digitalWrite( ignitor1, LOW );
  digitalWrite( ignitor2, LOW );
  digitalWrite( ignitor3, LOW );
  
  mySerial.println( "**********************IGNITORES DESACTIVADOS**********************" );
}

void procesoDespegar()
{
  for( int i = 10; i > 0; i--)
  {
    mySerial.println(i);
     delay(1000);
  }
  
  mySerial.println( "**********************IGNITOR 0 ACTIVADO**********************" );
  
  delay( TIEMPO_IG_0 );
  digitalWrite( ignitor0, HIGH );
  
  delay( TIEMPO_IG_1 );
  digitalWrite( ignitor1, HIGH );
  
  delay( TIEMPO_IG_2 );
  digitalWrite( ignitor2, HIGH );
  
  delay( TIEMPO_IG_3 );
  digitalWrite( ignitor3, HIGH );
  
  delay(10000);
  apagarIgnitores();
  procesoTomarMedidas();
  
}

/*
 *  Metodo se encarga de tomar medidas
 */
void procesoTomarMedidas()
{
  //TODO: Tomar medidas
  String datos = "";
  // TODO: Implementar lectura de los sensores.
  //int datos = analogRead(SensorXX);
  //int datos = digitalRead(SensorXX);
  
  File archivo = SD.open("datalog.txt", FILE_WRITER);
  if(archivo)
  {
   archivo.println(datos); 
   archivo.close();
   mySerial.println(datos);
  }
  else 
  {
   mySerial.println("No se pudo escribir en el archivo"); 
  }
}

/*
 *  Metodo se encarga de apagar ignitores
 */
void apagarIgnitores()
{
  //TODO: Tomar medidas
  digitalWrite( ignitor0, LOW );
  digitalWrite( ignitor1, LOW );
  digitalWrite( ignitor2, LOW );
  digitalWrite( ignitor3, LOW );
}
