/*
#         _\|/_   A ver..., ¿que tenemos por aqui?
#         (O-O)
# -----oOO-(_)-OOo------------------------------

#################################################
# ********************************************* #
# *              Detector de color            * #
# *     con módulo basado en chip TSC3200     * #
# *      Autor: Eulogio López Cayuela         * #
# *                                           * #
# *    Versión 1.1    Fecha: 12/03/2015       * #
# ********************************************* #
#################################################

* Descripcion de las patillas del sensor de color TSC3200

 - S0 y S1: permiten configurar un divisor de frecuencia para la señal de salida
 - S2 y S3: para seleccionar la componente del color  
            cuya intensidad que se desea medir: R, G, B o  todas a la vez
 - OUT    : patilla en la que se obtiene una señal cuya frecuencia depende de la intensidad del color
 - LED    : permite activar una luz para mejorar la calidad de la lectura


* Conexiones a realizar:
* 
(numeracion de patillas para arduino Mega)
S0 --> Digital pin 30
S1 --> Digital pin 31
S2 --> Digital pin 32
S3 --> Digital pin 33
OUT -> Digital pin 50 
LED -> Digital pin 22

*/


//*******************************************************
//   DEFINICION DE CONSTANTES
//*******************************************************
#define  S0 30
#define  S1 31   
#define  S2 32
#define  S3 33   
#define OUT 50
#define LED 22

char* color;            //variable apra contener el nombre del color (en la funcion que convierte a colores 'humanos'



//*******************************************************
//   FUNCION DE CONFIGURACION
//*******************************************************
void setup()
{
  Serial.begin(9600);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(OUT,INPUT);
  configMode(20);
}

 //*******************************************************
 //   BUCLE PRINCIPAL DEL PROGRAMA
 //*******************************************************
void loop()
{
  /* variables para almacener los datos en crudo de las distintas componentes del color*/
  double rojo, verde, azul, blanco;

  /* variable usada para controlar si se encienden o no los led durante la medicion del color */
  boolean encenderLED = true; // TRUE para que la funcion readColor() conecte los Leds para mejorar la deteccion    
  
  configMode(20); // configuramos el divisor de frecuencia al (20%) -> 1:5
  
  //seleccionamos el canal rojo y procedemos a leerlo
  selectColor(1);
  delay(10); //pausa para estabilizar el sensor entre las mediciones de cada componente
  rojo = readColor(encenderLED);
  
  //seleccionamos el canal verde y procedemos a leerlo
  selectColor(2);
  delay(10); //pausa para estabilizar el sensor entre las mediciones de cada componente
  verde = readColor(encenderLED);
      
  //seleccionamos el canal azul y procedemos a leerlo
  selectColor(3);
  delay(10); //pausa para estabilizar el sensor entre las mediciones de cada componente
  azul = readColor(encenderLED);
  
  //seleccionamos la 'intensidad de la luminosidad total' y procedemos a leerla
  selectColor(4);
  delay(10); //pausa para estabilizar el sensor entre las mediciones de cada componente
  blanco = readColor(encenderLED);
  
  configMode(0); //apagamos el sensor hasta la proxima consulta

  double r,g,b; // variables para almacenar las componentes RGB en el rango 0-1
  double ajuste = 3.0/blanco; // compensamos la luminosidad suma de los 3 colores
  r=min(1.0, ajuste + (blanco/rojo));
  g=min(1.0, ajuste + (blanco/verde));
  b=min(1.0, ajuste + (blanco/azul));
  
  // mostrar informacion en consola
  /*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/ 
  Serial.println("DATOS 'EN CRUDO' OBTENIDOS DEL SENSOR");
  Serial.print("datos      R : "); Serial.println(rojo);
  Serial.print("datos      G : "); Serial.println(verde);
  Serial.print("datos      B : "); Serial.println(azul);
  Serial.print("datos blanco : "); Serial.println(blanco);
  Serial.println("DATOS PROCESADOS Y COMPENSADOS");
  Serial.print(" Rojo  : "); Serial.print(r*100); Serial.println( "%");
  Serial.print(" verde : "); Serial.print(g*100); Serial.println( "%");
  Serial.print(" azul  : "); Serial.print(b*100); Serial.println( "%");
  Serial.println();
  Serial.println("----------------------------------------------");
  Serial.println(); 
  /*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/ 
  
  // ejecutar la funcion que convierte los colores 
  // al formato (Tono,Saturacion y Luminosidad),
  // y les asigna nombres segun el tono mas cercano.
  
  RGBtoHLS(r, g, b);
  
  delay (5000); // Pausa de 5 segundos entre mediciones de color
}



//*******************************************************
//  FUNCION PARA LECTURA DE SEÑALES DESDE EL SENSOR
//*******************************************************
float readColor(boolean ledON)
/* Lectura del sensor, que ofrece una señal cuya frecuencia dependerá de la intensidad del color medido
 * Usamos la orden pulseIn para medir el tiempo de uno de los pulsos de dicha señal.
 * Dado que el tiempo minimo que puede detectar pulseIn es de 10ns, debemos tener
 * la precaucion de haber selecionado previamente en el sensor un divisor de frecuencia al menos de (1:5)
 * ya que la frecuencia oroginal de las señales ofrecidas por el sensor son de periodo inferior a los 10ns
*/
{
  float valor;
  if (ledON == true){
    digitalWrite(LED, HIGH);    //encender el led para mejorar la deteccion si ledON = TRUE
    }
  pulseIn(OUT, LOW);            //esperar que acabe, si hay un pulso iniciado
  valor = pulseIn(OUT, HIGH);   //leer la duracion de un pulso
  return valor;
}


  
//*******************************************************
//  FUNCION PARA SELECCIONAR LA COMPONENTE RGB A MEDIR
//*******************************************************
void selectColor(int color)
/* 
 *  funcion para seleccionar el color a medir.
 *  Parametros:  1)Rojo, 2)Verde, 3)Azul, 4)Todo 
*/

{ 
  if (color == 1) {             //seleccion de la componente R (color rojo) en el sensor
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    return;
  }
  if (color == 2) {             //seleccion de la componente G (color verde) en el sensor
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    return;
  }
  if (color == 3) {             //seleccion de la componente B (color azul) en el sensor
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    return;
  }
  if (color == 4) {             //seleccion de la componente CLEAR (cantidad total de luz en el sensor)
    digitalWrite(S2, HIGH);
    digitalWrite(S3, LOW);
    return;
  }
}

//*******************************************************
//  FUNCION PARA CONFIGURAR EL SENSOR TCS3200 
//*******************************************************

void configMode(int modo)
/* 
 * funcion para activar/desactivar el sensor y configurar el divisor de frecuencia
 */
{
  if (modo == 0) {            //reposo, desactiva el sensor
    digitalWrite(LED, LOW);   //apagar el led para ahorrar energia
    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
  }
  if (modo == 2) {            //activa y selecciona fecuencia al 2%, divisor (1:50)
    digitalWrite(S0, LOW);
    digitalWrite(S1, HIGH);
  }
  if (modo == 20) {           //activa y selecciona fecuencia al 20%, divisor (1:5)
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);
  }
  if (modo == 100) {          //activa y selecciona fecuencia al 100%, divisor (1:1)
    digitalWrite(S0, HIGH);
    digitalWrite(S1, HIGH);
  }
}

//******************************************************* 
//   FUNCION PARA CONVERTIR RGB A COLORES 'HUMANOS'
//*******************************************************
void RGBtoHLS(float r, float g, float b)
/* 
 * Convertir color RGB a color HLS  ('Tono', Luminosidad, Saturación)
 * para facilitar la asignacion de nombres según el tono mas cercano.
 * Aplicamos formulas de teoria del color. 
 * (Consultar cualquier tutorial al respecto si se desea mas información)
 */
{
  double Tono, Luminosidad, Saturacion;
  double minRGB = min(min(r, g), b);  
  double maxRGB = max(max(r, g), b);
  double  diferenciaRGB = maxRGB - minRGB;
  //Serial.print("maxRGB: ..........");Serial.println(maxRGB);  // ---> para debug
  Luminosidad =(maxRGB + minRGB)/2.0;
  
  if (diferenciaRGB == 0.0) {// Si la diferencia entre componentes es 0, el color es un tono de GRIS
    Tono = Saturacion = 0.0;
  }

  else {
    // calcular Saturacion   
    Saturacion = diferenciaRGB / ( 1.0 - abs(2*Luminosidad-1));
    
    // calcular Tono
    if ( r == maxRGB ) {
      Tono = (g - b) / (6.0 * diferenciaRGB);
    }
      
    else if ( g == maxRGB ) {
      Tono = ((b - r) / (6.0 * diferenciaRGB))  + (1.0 / 3.0); 
    }
      
    else if ( b == maxRGB ) {
      Tono = ((r - g) / (6.0 * diferenciaRGB))  + (2.0 / 3.0); 
    }
    
    if ( Tono < 0 ) {
      Tono += 1.0;
    }
    if ( Tono > 1 ) {
      Tono -= 1.0;
    }
  }
  
  // Valores en porcentaje y grados
  Tono *= 360.0;          // pasamos a grados (0° - 360°)
  Saturacion *= 100.0;    // pasamos a porcentaje
  Luminosidad *= 100.0;   // pasamos a porcentaje
  char* colorVivo= "";
  char* color = "";
  if (Saturacion > 15.0 && Luminosidad > 55.0) {colorVivo = " muy palido";}
  if (Saturacion < 15.0 && Luminosidad <= 35.0) {color = "negro o muy oscuro";} 
  if (Saturacion < 10.0 && Luminosidad > 35.0) { color = "blanco o muy claro"; }
  if (Saturacion < 4.0 && Luminosidad > 25.0) { color = "gris"; }  

  if (color=="") { // determinar el nombre del color  
    int numColores = 8;
    char* nombresColor[] = {"naranja", "amarillo", "verde", "celeste", "azul", "violeta", "rosa", "rojo"};
    double gradosColor[] = {2.0, 25.0, 75.0, 150.0, 200.0, 255.0, 291.0, 350.0};
    if(Tono >= gradosColor[0] && Tono < gradosColor[7]) {
      for(int indice=0; indice < numColores-1; ++indice) {
        if(Tono >= gradosColor[indice] && Tono < gradosColor[indice+1]) {
          color = (nombresColor[indice]);
          break;
        }
      }
    }
    else {
      color = "rojo";
    }
  }
       
  // mostrar información en consola
  /*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/
  Serial.print("TONO: .........."); Serial.print(Tono); Serial.println("º");
  Serial.print("SATURACION: ...."); Serial.print(Saturacion); Serial.println("%");
  Serial.print("LUMINOSIDAD: ..."); Serial.print(Luminosidad); Serial.println("%");
  Serial.print("COLOR: "); Serial.print(color); Serial.println(colorVivo);
  Serial.println();
  /*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/
}
  
//*******************************************************
//                    FIN DE PROGAMA
//*******************************************************
