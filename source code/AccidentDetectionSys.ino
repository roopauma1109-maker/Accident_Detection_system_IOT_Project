#include <SPI.h>
#include <Ethernet.h>
#include <string.h>
#include <ctype.h>

int rxPin = 0;    // GPS RX
int txPin = 1;    // GPS TX

// GPS variables
int byteGPS = -1;
char linea[300] = "";
char comandoGPR[7] = "$GPRMC";
int cont = 0;
int bien = 0;
int conta = 0;
int indices[13];

// Function declarations
void checkCondition();
void sendmsg();
void gps();

void setup() 
{
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial.begin(9600);
  
  // Initialize GPS buffer
  for (int i = 0; i < 300; i++)
  {
    linea[i] = ' ';
  }
  
  // Wait for serial ready
  while (!Serial) {;}
}

void loop()
{
  checkCondition();
}

void checkCondition()
{
  int memsValue = analogRead(A0);

  // Trigger if MEMS is outside 250–370
  if (memsValue < 250 || memsValue > 370)
  {
    Serial.println("MEMS threshold crossed, detecting location...");
    gps();
    sendmsg();
  }

  delay(1000); // Poll every second
}

void sendmsg()
{
  Serial.println("AT+CMGF=1");    
  delay(1000);  
  Serial.println("AT+CMGS=\"+919094176434\"\r"); 
  delay(1000);
  Serial.println("***Location detected***");
  delay(1000);
  Serial.println("");
  Serial.println("Latitude and Longitude:");
  delay(1000);

  // Print GPS latitude and longitude from linea[]
  for (int i = 21; i <= 44; i++) {
    Serial.print(linea[i]);
  }
  Serial.println();
  delay(1000);
  Serial.println((char)26); // End SMS
  delay(4000);
}

void gps() 
{
  byteGPS = Serial.read();      
  if (byteGPS == -1) 
  {           
    delay(100); 
  } 
  else 
  {
    linea[conta] = byteGPS;        
    conta++;                      
    
    if (byteGPS == 13)
    {            
      cont = 0;
      bien = 0;

      // Check for $GPRMC header
      for (int i = 1; i < 7; i++)
      {     
        if (linea[i] == comandoGPR[i-1])
        {
          bien++;
        }
      }

      if (bien == 6)
      {               
        // Find commas
        for (int i = 0; i < 300; i++){
          if (linea[i] == ',')
          {    
            indices[cont] = i;
            cont++;
          }
          if (linea[i] == '*')
          {    
            indices[12] = i;
            cont++;
          }
        }

        // Extract latitude and longitude
        for (int i = 2; i < 5; i++)
        {
          if(i == 2 || i == 4)
          {
            if(i == 4) Serial.print(",");
            for (int j = indices[i]; j < (indices[i+1]-1); j++)
            {
              Serial.print(linea[j+1]);  
            }
          }
        }
      }

      // Reset GPS buffer
      conta = 0;                    
      for (int i = 0; i < 300; i++){     
        linea[i] = ' ';             
      }  
    }
  }
}