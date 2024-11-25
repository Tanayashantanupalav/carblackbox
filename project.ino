#include <LiquidCrystal.h>
#include <stdio.h>

#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 9);

LiquidCrystal lcd(6, 7, 5, 4, 3, 2);

#include <Wire.h>

/*********************** DS18B20 ***************************/
#include <OneWire.h> 
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 10 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
/***********************************************************/
int ADXL345 = 0x53;
float X_out, Y_out, Z_out;


int buzzer       = 13;

float tempc=0;
int memsx=0,memsy=0;
int vehicle_speed=0;
int engine_rpm=0;

char memss='x';
int sts1=0,sts2=0,sts3=0,sts4=0;
int cntl1=0; 
unsigned char rcv;
 
char pastnumber[11];
 

int i=0,k=0,lop=0;
int  gps_status=0;
float latitude=0; 
float logitude=0;                       
String Speed="";
String gpsString="";
char *test="$GPRMC";
unsigned char gv=0,msg1[10],msg2[11];
float lati=0,longi=0;
unsigned int lati1=0,longi1=0;
unsigned char flat[5],flong[5];
unsigned char finallat[8]="17.4309",finallong[9]="078.4419";
int ii=0,rchkr=0;
//17.4309,78.4419
 
int sti=0;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void beep()
{
  digitalWrite(buzzer, LOW);delay(2500);digitalWrite(buzzer, HIGH);
}

void adxl_345_init()
 {
   Wire.begin(); // Initiate the Wire library
   // Set ADXL345 in measuring mode
   Wire.beginTransmission(ADXL345); // Start communicating with the device 
   Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
   // Enable measurement
   Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
   Wire.endTransmission();
   delay(10);
 }
 void adxl_345_read()
 {
   // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
  //X_out = X_out/256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
  //Y_out = Y_out/256;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  //Z_out = Z_out/256;

/*
  
  Serial.print("Xa= ");
  Serial.print(X_out);
  Serial.print("   Ya= ");
  Serial.print(Y_out);
  Serial.print("   Za= ");
  Serial.println(Z_out);
*/ 
 }
void okcheck()
{
  unsigned char rcr;
  do{
      rcr = Serial.read();
    }while(rcr != 'K');
}


void send_link()
{
    Serial.write("AT+CMGS=\"");
    Serial.write(pastnumber);
    Serial.write("\"\r\n");  delay(2500);  
    Serial.write("https://www.google.co.in/search?client=opera&q=");
    for(ii=0;ii<=6;ii++){Serial.write(finallat[ii]);}
    Serial.write("%2C");
    for(ii=0;ii<=7;ii++){Serial.write(finallong[ii]);}
    Serial.write(0x1A);delay(4000);delay(4000);
}


 
void setup() 
{
 Serial.begin(1200);//serialEvent();
 mySerial.begin(9600);
 
 pinMode(buzzer, OUTPUT);
 
 digitalWrite(buzzer, HIGH);


   //25. Wireless Black Box for vehicles using GSM GPS
 lcd.begin(16, 2);lcd.cursor();
 lcd.print(" Wireless Black ");
 lcd.setCursor(0,1);
 lcd.print("Box For Vehicles"); 
  delay(2500);
 lcd.clear();
 lcd.print(" Using GSM GPS ");
  delay(2500);
  
 lcd.clear();
 lcd.print("Getting GPS Data");
 lcd.setCursor(0,1);
 lcd.print("Please Wait.....");
    delay(4000);
 
  adxl_345_init();
  
  sensors.begin(); 

     
  get_gps();
  gps_convert();
      
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Lat:");
     for(ii=0;ii<=6;ii++) lcd.write(finallat[ii]);
     
     lcd.setCursor(0,1);
    lcd.write("Long:");
     for(ii=0;ii<=7;ii++) lcd.write(finallong[ii]);   
   
  delay(4000); 
     lcd.clear();
     lcd.print("GSM Initialisation");
  
  gsminit();

  delay(1500); 
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("T:");  //2-3-4-5-6,0
   // lcd.setCursor(8,0);
   // lcd.print("M:");  //10,0
    
    lcd.setCursor(0,1);
    lcd.print("VS:");  //3-4-5-6,1
    lcd.setCursor(7,1);
    lcd.print("ER:");  //10-11-12-13,1
   
  //serialEvent();
}

void loop() 
{ 
  /*
  tempc = analogRead(A0);
  tempc = (tempc*0.48828125);
  lcd.setCursor(2,0);convertl(tempc);    delay(100);
  */

  sensors.requestTemperatures();
  tempc = sensors.getTempCByIndex(0);
  //tempf = ((tempc*1.8) + 32); 
  lcd.setCursor(2,0);convertl(tempc);   delay(10);
  
  //lcd.write('F');    delay(1);//6,7,8,9

  
  if(tempc > 80)
    {
      sts1++;
      if(sts1 >= 2){sts1=2;}
      if(sts1 == 1)
        {beep();
          lcd.setCursor(15,0);lcd.print("S");
             delay(4000);delay(4000);delay(4000);
          Serial.write("AT+CMGS=\"");
          Serial.write(pastnumber);
          Serial.write("\"\r\n");  delay(3000);  
          Serial.write("High Engine Temp:");Serial.print(tempc);
          Serial.write("C  https://www.google.co.in/search?client=opera&q=");
          for(ii=0;ii<=6;ii++){Serial.write(finallat[ii]);}
          Serial.write("%2C");
          for(ii=0;ii<=7;ii++){Serial.write(finallong[ii]);}
          Serial.write(0x1A);delay(4000);delay(4000);delay(4000);
          lcd.setCursor(15,0);lcd.print(" ");
        }
    }
  else
    {
      sts1=0;  
    }
  
  
  //memsx = analogRead(A1);delay(100);
  //memsy = analogRead(A2);delay(100);
  /*
  if((memsx > 300 && memsx < 400) && (memsy > 300 && memsy < 400))
     {
       lcd.setCursor(10,0);lcd.print("Stab ");
       memss='s';
       sts2=0;
     }
  */
     adxl_345_read();

     if((X_out > -200 && X_out < 200) && (Y_out > -200 && Y_out < 200)) 
       {
        lcd.setCursor(10,0);lcd.print("Stab ");
        memss='s';
        sts2=0;
       }
    else
     {
       lcd.setCursor(10,0);lcd.print("Tilt  ");
       memss='m';   
       sts2++;
       if(sts2 >= 2){sts2=2;}
    
       if(sts2 == 1)
         {beep();
          lcd.setCursor(15,0);lcd.print("S");
             delay(4000);delay(4000);delay(4000);
          Serial.write("AT+CMGS=\"");
          Serial.write(pastnumber);
          Serial.write("\"\r\n");  delay(3000);  
          Serial.write("MEMS:Tilt-Vehicle Position Tilt");
          Serial.write("  https://www.google.co.in/search?client=opera&q=");
          for(ii=0;ii<=6;ii++){Serial.write(finallat[ii]);}
          Serial.write("%2C");
          for(ii=0;ii<=7;ii++){Serial.write(finallong[ii]);}
          Serial.write(0x1A);delay(4000);delay(4000);delay(4000);
          lcd.setCursor(15,0);lcd.print(" ");
         }
     }
     

  vehicle_speed = analogRead(A0);//delay(10);
  vehicle_speed = (vehicle_speed/5);
  lcd.setCursor(3,1);convertl(vehicle_speed);
   
  if(vehicle_speed > 100)
    {
     sts3++;
     if(sts3 >= 2){sts3=2;}
     if(sts3 == 1)
       {beep();
          lcd.setCursor(15,0);lcd.print("S");
             delay(3000);delay(4000);delay(4000);
          Serial.write("AT+CMGS=\"");
          Serial.write(pastnumber);
          Serial.write("\"\r\n");  delay(3000);  
          Serial.write("High Speed:");Serial.print(vehicle_speed);
          Serial.write("K/hr  https://www.google.co.in/search?client=opera&q=");
          for(ii=0;ii<=6;ii++){Serial.write(finallat[ii]);}
          Serial.write("%2C");
          for(ii=0;ii<=7;ii++){Serial.write(finallong[ii]);}
          Serial.write(0x1A);delay(4000);delay(4000);delay(4000);
          lcd.setCursor(15,0);lcd.print(" ");
       }
    }
  else
    {
     sts3=0;    
    }
    
  engine_rpm    = analogRead(A1);//delay(10);
  engine_rpm    = (engine_rpm * 4); 
  lcd.setCursor(10,1);convertl(engine_rpm);
  
  if(engine_rpm > 3000)
    {
     sts4++;
     if(sts4 >= 2){sts4=2;}
     if(sts4 == 1)
       {beep();
          lcd.setCursor(15,0);lcd.print("S");
             delay(4000);delay(4000);delay(4000);
          Serial.write("AT+CMGS=\"");
          Serial.write(pastnumber);
          Serial.write("\"\r\n");  delay(3000);  
          Serial.write("High RPM:");Serial.print(engine_rpm);
          Serial.write("R.P.M  https://www.google.co.in/search?client=opera&q=");
          for(ii=0;ii<=6;ii++){Serial.write(finallat[ii]);}
          Serial.write("%2C");
          for(ii=0;ii<=7;ii++){Serial.write(finallong[ii]);}
          Serial.write(0x1A);delay(4000);delay(4000);delay(4000);
          lcd.setCursor(15,0);lcd.print(" ");
       }
    }
  else
    {
     sts4=0;    
    }
  
   while(Serial.available()) 
        {         
         char inChar = (char)Serial.read();
          if(inChar == '*')
            {sti=1;
              //inputString += inChar;
            }
          if(sti == 1)
            {
                inputString += inChar;
            }
          if(inChar == '#')
            {sti=0;
              stringComplete = true;      
            }
        }
        
 if(stringComplete)
     {
      //lcd.clear();lcd.print(inputString);
      //if(inputString[1] == 's' || inputString[1] == 'S') 
      if(inputString[0] == '*')  
        {           
          lcd.setCursor(15,0);lcd.print("S");
             delay(3000);delay(4000);delay(4000);
          Serial.write("AT+CMGS=\"");
          Serial.write(pastnumber);
          Serial.write("\"\r\n");  delay(3000);  
          Serial.write("Vehicle Parameters: ");
          Serial.write("Temp:");Serial.print(tempc);
          Serial.write(" Vehicle Position:");
          if(memss == 's'){Serial.write("Stable ");}
          if(memss == 'm'){Serial.write("Tilt ");}
          Serial.write(" Vehicle Speed:");Serial.print(vehicle_speed);
          Serial.write("K/hr Engine RPM:");Serial.print(engine_rpm);
          Serial.write("  https://www.google.co.in/search?client=opera&q=");
          for(ii=0;ii<=6;ii++){Serial.write(finallat[ii]);}
          Serial.write("%2C");
          for(ii=0;ii<=7;ii++){Serial.write(finallong[ii]);}
          Serial.write(0x1A);delay(4000);delay(4000);delay(4000);
          lcd.setCursor(15,0);lcd.print(" ");
        }  
      inputString = "";
      stringComplete = false;
     }  
}



/*
void serialEvent() 
{
  while (Serial.available()) 
        {         
         char inChar = (char)Serial.read();
          if(inChar == '*')
            {sti=1;
              //inputString += inChar;
            }
          if(sti == 1)
            {
                inputString += inChar;
            }
          if(inChar == '#')
            {sti=0;
              stringComplete = true;      
            }
        }
}
*/


int readSerial(char result[]) 
{
  int i = 0;
  while (1) 
  {
    while (Serial.available() > 0) 
    {
      char inChar = Serial.read();
      if (inChar == '\n') 
         {
          result[i] = '\0';
          Serial.flush();
          return 0;
         }
      if (inChar != '\r') 
         {
          result[i] = inChar;
          i++;
         }
    }
  }
}


int readSerial1(char result[]) 
{
  int i = 0;
  while (1) 
  {
    while (Serial.available() > 0) 
    {
      char inChar = Serial.read();
      if (inChar == '*') 
         {
          result[i] = '\0';
          Serial.flush();
          return 0;
         }
      if (inChar != '*') 
         {
          result[i] = inChar;
          i++;
         }
    }
  }
}




void gpsEvent()
{
  gpsString="";
  while(1)
  {
   //while (gps.available()>0)            //Serial incoming data from GPS

    while (mySerial.available() > 0)
   {
    //char inChar = (char)gps.read();
    char inChar = (char)mySerial.read();
     gpsString+= inChar;                    //store incoming data from GPS to temparary string str[]
     i++;
    // Serial.print(inChar);
     if (i < 7)                      
     {
      if(gpsString[i-1] != test[i-1])         //check for right string
      {
        i=0;
        gpsString="";
      }
     }
    if(inChar=='\r')
    {
     if(i>60)
     {
       gps_status=1;
       break;
     }
     else
     {
       i=0;
     }
    }
  }
   if(gps_status)
    break;
  }
}

void get_gps()
{
  
  lcd.clear();
  lcd.print("Getting GPS Data");
  lcd.setCursor(0,1);
  lcd.print("Please Wait.....");
 
   gps_status=0;
   int x=0;
   while(gps_status==0)
   {
    gpsEvent();
    int str_lenth=i;
    coordinate2dec();
    i=0;x=0;
    str_lenth=0;
   }
}

void coordinate2dec()
{
  String lat_degree="";
    for(i=17;i<=18;i++)         
      lat_degree+=gpsString[i];
      
  String lat_minut="";
     for(i=18;i<=19;i++)         
      lat_minut+=gpsString[i];
     for(i=21;i<=22;i++)         
      lat_minut+=gpsString[i];


       
  String log_degree="";
    for(i=29;i<=31;i++)
      log_degree+=gpsString[i];
  String log_minut="";
    for(i=32;i<=33;i++)
      log_minut+=gpsString[i];
    for(i=35;i<=36;i++)
      log_minut+=gpsString[i];

      
    Speed="";
    for(i=42;i<45;i++)          //extract longitude from string
      Speed+=gpsString[i];
      
     float minut= lat_minut.toFloat();
     minut=minut/60;
     float degree=lat_degree.toFloat();
     latitude=degree+minut;
     
     minut= log_minut.toFloat();
     minut=minut/60;
     degree=log_degree.toFloat();
     logitude=degree+minut;
}

/*
void coordinate2dec()
{
  String lat_degree="";
    for(i=19;i<=20;i++)         
      lat_degree+=gpsString[i];
      
  String lat_minut="";
     for(i=21;i<=22;i++)         
      lat_minut+=gpsString[i];
     for(i=24;i<=25;i++)         
      lat_minut+=gpsString[i];


       
  String log_degree="";
    for(i=32;i<=34;i++)
      log_degree+=gpsString[i];
  String log_minut="";
    for(i=35;i<=36;i++)
      log_minut+=gpsString[i];
    for(i=38;i<=39;i++)
      log_minut+=gpsString[i];

      
    Speed="";
    for(i=45;i<48;i++)          //extract longitude from string
      Speed+=gpsString[i];
      
     float minut= lat_minut.toFloat();
     minut=minut/60;
     float degree=lat_degree.toFloat();
     latitude=degree+minut;
     
     minut= log_minut.toFloat();
     minut=minut/60;
     degree=log_degree.toFloat();
     logitude=degree+minut;
}
*/
void gps_convert()
{
  if(gps_status)
  {
  //Serial.println(gpsString);

  if(gpsString[0] == '$' && gpsString[1] == 'G' && gpsString[2] == 'P' && gpsString[3] == 'R' && gpsString[4] == 'M' && gpsString[5] == 'C')
    {
     // Serial.println("Don11111111111111111111111111111111111111111111111111111\r\n");      
    //  Serial.write(gpsString[18]);Serial.write(gpsString[19]);Serial.write(gpsString[20]);Serial.write(gpsString[21]);Serial.write(gpsString[22]);
     //lcd.setCursor(0,0);
     for(ii=0;ii<9;ii++)
       {
        //lcd.write(gpsString[19+ii]);    
        msg1[ii] = gpsString[19+ii];
        //Serial.write(msg1[ii]);
       }
       //Serial.println("\r\n");
     //lcd.setCursor(0,1);
      for(ii=0;ii<10;ii++)
       {
        //lcd.write(gpsString[32+ii]);
        msg2[ii] = gpsString[32+ii];    
       // Serial.write(msg2[ii]);
       }

// Serial.println(msg1);
// Serial.println(msg2);

       //lati = (((msg1[2]-48)*100000) +((msg1[3]-48)*10000) + ((msg1[5]-48)*1000) + ((msg1[6]-48)*100) + ((msg1[7]-48)*10) + (msg1[8]-48)); 
       //longi = (((msg2[3]-48)*100000) + ((msg2[4]-48)*10000) + ((msg2[6]-48)*1000) + ((msg2[7]-48)*100) + ((msg2[8]-48)*10) + (msg2[9]-48));

       lati = (((msg1[2]-48)*1000) + ((msg1[3]-48)*100) + ((msg1[5]-48)*10) + (msg1[6]-48)); 
       longi = (((msg2[3]-48)*1000) + ((msg2[4]-48)*100) + ((msg2[6]-48)*10) + (msg2[7]-48));

      // converts(lati);Serial.write("-");
      // converts(longi);Serial.write("\r\n");
       
       lati = (lati/60);  longi = (longi/60);

       lati = (lati*100); longi = (longi*100);
       lati1 = lati;      longi1 = longi;
       
// Serial.write("After ");
  //      converts(lati1);Serial.write("-");
    //   converts(longi1);Serial.write("\r\n");
 

              convlat(lati); convlong(longi);
        finallat[0] = msg1[0];
        finallat[1] = msg1[1];
        finallat[2] = '.';
        finallat[3] = flat[0]; finallat[4] = flat[1];finallat[5] = flat[2];finallat[6] = flat[3];finallat[7] = '\0';


        finallong[0] = msg2[0];
        finallong[1] = msg2[1];
        finallong[2] = msg2[2];
        finallong[3] = '.';
        finallong[4] = flong[0];finallong[5] = flong[1];finallong[6] = flong[2];finallong[7] = flong[3];finallong[8] = '\0'; 

   
    }
  }  
}

 void convlat(unsigned int value)  
      {
             unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;              
      c=c|0x30;
      e=e|0x30; 
    g=g|0x30;              
      h=h|0x30;
   
  // dlcd(a);
//   dlcd(c);dlcd(e); dlcd(g);dlcd(h);//lcddata('A');//lcddata(' ');lcddata(' ');
    
         
               flat[0] = c;
               flat[1] = e;
               flat[2] = g;
             flat[3] = h;


          }

 void convlong(unsigned int value)  
      {
            unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;              
      c=c|0x30;
      e=e|0x30; 
    g=g|0x30;              
      h=h|0x30;
   
         
               flong[0] = c;
               flong[1] = e;
               flong[2] = g;
             flong[3] = h;


          }


void gsminit()
{
  delay(10000);
  delay(10000);
  delay(10000);
  
  Serial.write("AT\r\n");                   okcheck();
  Serial.write("ATE0\r\n");                 okcheck();
  Serial.write("AT+CMGF=1\r\n");            okcheck();
  Serial.write("AT+CNMI=1,2,0,0\r\n");      okcheck();
  Serial.write("AT+CSMP=17,167,0,0\r\n");   okcheck();
    
  lcd.clear();
  lcd.print("SEND MSG STORE");
  lcd.setCursor(0,1);
  lcd.print("MOBILE NUMBER");  
  do{
     rcv = Serial.read();
    }while(rcv != '*');
     readSerial(pastnumber);pastnumber[10] = '\0';
     
     /*
  pastnumber1[0] = pastnumber[0];pastnumber1[1] = pastnumber[1];pastnumber1[2] = pastnumber[2];pastnumber1[3] = pastnumber[3];pastnumber1[4] = pastnumber[4];pastnumber1[5] = pastnumber[5];
  pastnumber1[6] = pastnumber[6];pastnumber1[7] = pastnumber[7];pastnumber1[8] = pastnumber[8];pastnumber1[9] = pastnumber[9];pastnumber1[10] = '\0';
    */
/*
  pastnumber3[0] = pastnumber[20];pastnumber3[1] = pastnumber[21];pastnumber3[2] = pastnumber[22];pastnumber3[3] = pastnumber[23];pastnumber3[4] = pastnumber[24];pastnumber3[5] = pastnumber[25];
  pastnumber3[6] = pastnumber[26];pastnumber3[7] = pastnumber[27];pastnumber3[8] = pastnumber[28];pastnumber3[9] = pastnumber[29];pastnumber3[10] = '\0';
*/
  lcd.clear();
  lcd.print(pastnumber);

                             delay(3000);
    Serial.write("AT+CMGS=\"");
    Serial.write(pastnumber);
    Serial.write("\"\r\n"); delay(3000);
    Serial.write("Mobile Number Registerd\r\n");
    Serial.write(0x1A);    
    delay(4000);  delay(4000);  

}


void converts(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
   Serial.write(a);
   Serial.write(c);
   Serial.write(e); 
   Serial.write(g);
   Serial.write(h);
}

void convertl(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
   //lcd.write(a);
   lcd.write(c);
   lcd.write(e); 
   lcd.write(g);
   lcd.write(h);
}
