/*************************************************** 
  This is an example sketch for our optical Fingerprint sensor


   enrolling bugs cleared
  enroll,punch and all deleting functionality added
  id re alloting bug cleared 
  individual delete functionality added
  display name functionality added
  time functionality
  punch in out added
  Authority functionality added
  punch in out bug cleared
  upgraded to Arduino mega
  excel output created! :)
  date checking function added
  LCD Added
  BUTTONS AND LED Added
  file name bugs cleared

 ****************************************************/


#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>
#include <DS3231.h>
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>


#define mySerial Serial1

DS3231  rtc(SDA, SCL);                  // Init the DS3231 using the hardware interface
//DS3231  rtc;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int enroll=45,punch=47,delet=49;
const int red=39,yellow=41,green=43;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
File myFile;
String option;
int bs=0;
const int chipSelect = 53;
uint8_t main_menu=1,id;
void punch_Mode();
void delete_Mode();
int getFingerprintIDez();
void SD_avilable();
void file_name();
void mont_check();
int fin=1;// Fingerprint id no
char fname[15],mo[9];
char pre_mo[9];              // after firt run uncommentout
//char pre_mo[9]="August";                                                                                 // commentout after firt run 

                                              /*********************/

struct employ
{
  int id_no;
  char inout;
  char name[21];                         // 21 is asigned bcs 21+2+1+6 ,30 so we can store 110 data
  char intim[6];
}emp[100];                                // 110 max


                                              /********************/


void setup()  
{
  Serial.begin(9600);
  //rtc.begin();
  Wire.begin();
  lcd.begin(16, 2);        // lcd.print("hello, world!");   lcd.setCursor(0, 1);
  
  pinMode(red, OUTPUT);// initialize the LED pin as an output:
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(enroll, INPUT);// initialize the pushbutton pin as an input:
  pinMode(punch, INPUT);
  pinMode(delet, INPUT);

  SD_avilable();
  
 
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  
  Serial.println("\n\nSMEC ABCD");
  lcd.print("   SMEC ABCD");
  delay(1000);
  lcd.clear();

  // set the data rate for the sensor serial port
  finger.begin(9600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    lcd.print("Did not find fin");
    lcd.setCursor(0, 1);
    lcd.print("gerprint sensor");
    while (1) 
    {
      digitalWrite(red, HIGH);
      delay(500);
      digitalWrite(red, LOW);
      delay(500);
      }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  
  //EEPROM.put(4000, pre_mo);                                                                              // commentout after firt run

  
  file_name();
   
}

                                                 /********************/

uint8_t allocate_id(void) {
  
  while(fin<=100)                                                   // Fingerprint id no
  {
      uint8_t c = finger.loadModel(fin);
      if(c==FINGERPRINT_OK)
      {
        fin++;
      }
      else
      { 
        return fin;
      }
  }
}

                                                  /********************/


void loop()                                                      // run over and over again
{
  
  if(main_menu )
   {
     int Ac=0;                                                   //Authority check variable
     datecheck();
     lcd.setCursor(0, 1);
     lcd.print(" Press a Button ");
     main_menu =0;
     if(digitalRead(punch))
          {
            lcd.clear();
            punch_Mode();
          }
      else if(digitalRead(enroll))
      {
        lcd.clear();
        Ac=authority_check();                                     // Authority check
        if(Ac==4)
        {
         id = allocate_id();
         
          if (id == 0) {// ID #0 not allowed, try again!
             return;
          }
          else if(id<=100)
          {
              Serial.println("Ready to enroll a fingerprint!");
              lcd.print("Ready to enroll");
              lcd.setCursor(0, 1);
              Serial.print("Enrolling ID #");
              lcd.print("Enrolling ID #");
              Serial.println(id);
              lcd.print(id);
              getFingerprintEnroll();
              delay(1000);
              main_menu=1;
          }
          else
          {
              Serial.println("Memory full!!!!!! ");
              lcd.clear();
              lcd.print("Memory full!!!!!! ");
              delay(2000);
              lcd.clear();
              main_menu=1;
          }
      }
      else
      {
        main_menu=1;
      }
      }
      else if(digitalRead(delet))
      {
            lcd.clear();
            Ac=authority_check();                                    // Authority check
            if(Ac==4)
              {
              delete_Mode();
              finger.getTemplateCount();
              Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
              fin=1;
              main_menu=1;
              }
            else
            {
              main_menu=1;
            }
      }
      else if(option=="delall")
      {
              finger.emptyDatabase();
              Serial.println("Deleted!");
              fin=1;
              finger.getTemplateCount();
              Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
              main_menu=1;
      }
      else
      {
              main_menu=1;
      }
     
 }
}

/***********************************************************************************************/

                                        /* punch */

/************************************************************************************************/


uint8_t getFingerprintID() {   //unsigned integer of length 8 bits(uint8_t)
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  lcd.clear();
  lcd.print("ID #"); lcd.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}

void punch_Mode()
{
  int i=0;
  int8_t id=-1; // variable to store fingerprint id
  while(i<3)
  {
    Serial.print("Please place your finger in Sensor: ");
    lcd.clear();
    lcd.print("  Place Finger  ");
    digitalWrite(yellow, HIGH);
    delay(3000);
    lcd.clear();
    id=getFingerprintIDez();
    main_menu=1;
    if(id != -1)
    {
      digitalWrite(yellow, LOW);
      digitalWrite(green, HIGH);
      get_rom(id);                                                    // Read from EEPROM
      get_inout(id);                                                  // in out read and write / EEPROM
      delay(3000);
      lcd.clear();
      digitalWrite(green, LOW);
      return;
      i=3;
    }
  
    else
    {
      digitalWrite(yellow, LOW);
      digitalWrite(red, HIGH);
      Serial.print("Unknown Finger! \n");
      lcd.print(" Unknown Finger!");
      delay(1000);
      lcd.clear();
      digitalWrite(red, LOW);
      i++;
    }
    digitalWrite(yellow, LOW);
}
}

/***********************************************************************************************/

                                          /* Enroll */

/************************************************************************************************/


uint8_t getFingerprintEnroll() {

  int p = -1;
  digitalWrite(yellow, HIGH);
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  lcd.clear();
  lcd.print("Wating for valid");
  lcd.setCursor(0, 1);
  lcd.print("finger #");lcd.print(id);
  while (p != FINGERPRINT_OK){
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      lcd.clear();
      lcd.print("   Image taken  ");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      lcd.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.clear();
      lcd.print("Comuniaton error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.print("Imaging error");
      lcd.clear();
      lcd.print(" Imaging error  ");
      break;
    default:
      Serial.println("Unknown error");
      lcd.clear();
      lcd.print(" Unknown error  ");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.fingerFastSearch();                                                    //     extra added to avoid re enroll
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    lcd.clear();
    lcd.print("Fond prnt match!");
    digitalWrite(yellow, LOW);
      digitalWrite(red, HIGH);
    Serial.print("Found ID #"); Serial.println(finger.fingerID);
    lcd.setCursor(0, 1);
    lcd.print("Found ID #");lcd.print(finger.fingerID);
    delay(2000);
    lcd.clear();
    digitalWrite(red, LOW);
  }
  else{
  Serial.println("Remove finger");
  lcd.clear();
  lcd.print(" Remove finger ");
  delay(2000);
  p = 0;


  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  lcd.clear();
  lcd.print("ID ");lcd.print(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.setCursor(0, 1);
  lcd.print("Place same fingr");
  delay(1000);
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      lcd.setCursor(0, 1);
      lcd.print("  Image taken  ");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      lcd.clear();
      lcd.print("ID ");lcd.print(id);
      //lcd.setCursor(0, 1);
      
      lcd.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.setCursor(0, 1);
      lcd.print("Comuniaton error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      lcd.setCursor(0, 1);
      lcd.print(" Imaging error  ");
      break;
    default:
      Serial.println("Unknown error");
      lcd.setCursor(0, 1);
      lcd.print(" Unknown error  ");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    lcd.clear();
    digitalWrite(yellow, LOW);
    digitalWrite(green, HIGH);
    lcd.print("Prints matched!");
    delay(500);
    digitalWrite(green, LOW);
    digitalWrite(yellow, HIGH);
    lcd.clear();

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    lcd.clear();
    digitalWrite(yellow, LOW);
    digitalWrite(red, HIGH);
    lcd.print(" Did not match! ");
    delay(1000);
    digitalWrite(red, LOW);
    lcd.clear();
    return p;
  } else {
    Serial.println("Unknown error");
    digitalWrite(yellow, LOW);
    digitalWrite(red, HIGH);
    lcd.clear();
    lcd.print(" Unknown error  ");
    delay(1000);
    digitalWrite(red, LOW);
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  lcd.clear();
  lcd.print("ID ");lcd.print(id);
  put_rom(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {                                                                       // store to EEPROM
    Serial.println("Stored!");
    digitalWrite(yellow, LOW);
    digitalWrite(green, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("    Stored!    ");
    delay(1000);
    lcd.clear();
    digitalWrite(green, LOW);
    fin++;                                                                                        // store id no Fingerprint id no(fin)
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}
}



/***********************************************************************************************/

                                       /* delete */

/************************************************************************************************/


void delete_Mode()
{
  int i=0;
  int8_t id=-1; // variable to store fingerprint id
  while(i<3)
  {
    Serial.print("Please place your finger in Sensor: ");
    lcd.clear();
    lcd.print("place your fingr");
    digitalWrite(yellow, HIGH);
    delay(3000);
    lcd.clear();
    id=getFingerprintIDez();
    main_menu=1;
    if(id != -1)
    {
     digitalWrite(yellow, LOW); 
     digitalWrite(green, HIGH);
     Serial.print("ID #"); Serial.println(id);
     lcd.print("ID #"); lcd.print(id);
     get_rom(id); 
      deleteFingerprint(id);
      delay(2000);
      digitalWrite(green, LOW);
      lcd.clear();
      return;
      i=3;
    }
  
    else
    {
      digitalWrite(yellow, LOW);
      digitalWrite(red, HIGH);
      Serial.print("Unknown Finger! \n");
      lcd.print(" Unknown Finger!");
      delay(1000);
      digitalWrite(red, LOW);
      lcd.clear();
      i++;
    }
}
}


uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println(" Deleted!");
    lcd.clear();
    lcd.print("    Deleted!    ");
                                                     
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}


/***********************************************************************************************/

                                       /* EEPROM Functions */

/************************************************************************************************/



void put_rom(int no)           // no is the id no. reciving variable
{
  String inst;
  char str[21];
  int adrs,j;
  j = (no-1);
  adrs = (j*30);                                   // allocating the address wrt id no(30 byte allocating for each one)
    emp[j].id_no = no;
    Serial.print("Enter the name : ");
    lcd.clear();
    lcd.print("Enter the name :");
    while(!(Serial.available()));
    inst=Serial.readStringUntil('\n');
    inst.toCharArray(str,21);
    strcpy(emp[j].name,str);
    Serial.println(emp[j].name);
    lcd.setCursor(0, 1);
    lcd.print(emp[j].name);
    delay(1000);
    emp[j].inout = 'i';
    EEPROM.put(adrs, emp[j]);
}




void get_rom(int no)
{

   int adrs,j;
   j=(no-1);
   adrs = (j*30);
   EEPROM.get(adrs, emp[j]);
   Serial.print(emp[j].name);
   lcd.setCursor(0, 1);
   lcd.print(emp[j].name);
   delay(2000);
   lcd.clear();
   //Serial.println(sizeof(emp[j]));
   
}


void clr_rom()
{
  for (int c = 0 ; c < EEPROM.length() ; c++) {
    EEPROM.write(c, 255);
}
}

/***********************************************************************************************/

                                       /* Authority Functions */

/************************************************************************************************/



int authority_check()
{
  int i=0;
  int8_t id=-1; // variable to store fingerprint id
  while(i<3)
  {
    Serial.print("Place The finger of an Authorized Person: ");
    digitalWrite(yellow, HIGH);
    lcd.print("Place Authrized");
    lcd.setCursor(0, 1);
    lcd.print("Person's finger");
    delay(3000);
    lcd.clear();
    
    id=getAuthority();

    main_menu=1;
    if(id != -1)
    {
      if(id==1|id==2|id==3)                                     // id no 1 and 2 allocated for authority                                     
        {
          digitalWrite(yellow, LOW);
          digitalWrite(green, HIGH);
          Serial.println("Access Granted");
          lcd.print(" Access Granted ");
          delay(3000);
          digitalWrite(green, LOW);
          lcd.clear();
          return 4;                                         // return 1 makes some error so i select 4
                                       
        }
      else
        {
          digitalWrite(yellow, LOW);
          digitalWrite(red, HIGH);
          Serial.println("Access Denied");
          lcd.print(" Access Denied ");
          delay(1000);
          digitalWrite(red, LOW);
          lcd.clear();
          return 0;
        }
      
      delay(3000);
      return;
      i=3;
    }
  
    else
    {
      digitalWrite(yellow, LOW);
      digitalWrite(red, HIGH);
      Serial.print("Unknown Finger! \n");
      lcd.print(" Unknown Finger!");
      delay(1000);
      digitalWrite(red, LOW);
      lcd.clear();
      i++;
    }
}
}


int getAuthority()
{
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  return finger.fingerID; 
}



/***********************************************************************************************/

                                       /* in/out Functions */

/************************************************************************************************/



void get_inout(int no)
{
   String inst,outst;
   char intime[6],outime[6];
   int adrs,j;
   j=(no-1);
   adrs = (j*30);
   //file_name();
   EEPROM.get(adrs, emp[j]);
   switch(emp[j].inout)                                                      // Punch in out 
        {
          case 'o':
          Serial.println(" PUNCHED IN ");
          lcd.print("PUNCHED IN");
          inst=rtc.getTimeStr();
          inst.toCharArray(intime,6);
          strcpy(emp[j].intim,intime); 
          Serial.print("Time - ");Serial.println(emp[j].intim); 
          lcd.setCursor(0, 1);
          lcd.print("Time - ");lcd.print(emp[j].intim);      
          emp[j].inout = 'i';  
          EEPROM.put(adrs,emp[j]);
          break;   
                                 
          case 'i':
          Serial.println(" PUNCHED OUT ");
          Serial.print("PUNCH IN TIME : ");Serial.println(emp[j].intim);
          outst=rtc.getTimeStr();
          outst.toCharArray(outime,6);
          Serial.print("PUNCH OUT TIME : ");Serial.println(outime);
          myFile = SD.open(fname, FILE_WRITE);                                                            //.csv

                  if (myFile)
                  {
                   Serial.print("Writing...");
                    myFile.println(String(rtc.getDateStr())+","+String(emp[j].name)+","+String(emp[j].intim)+","+String(outime));
                    myFile.close();                                                             // close the file:
                    Serial.println("done.");
                  }
                  else 
                  {
                    Serial.println("error opening File ");      // if the file didn't open, print an error:
                    digitalWrite(yellow, LOW);
                    digitalWrite(green, LOW);
                    digitalWrite(red, HIGH);
                    lcd.clear();
                    lcd.print("   File Error   ");
                    delay(1000);
                    digitalWrite(red, LOW);
                  }
          emp[j].inout = 'o';
          lcd.print("PUNCHED OUT");
          lcd.setCursor(0, 1);
          lcd.print("Time - ");lcd.print(outime);
          EEPROM.put(adrs,emp[j]);
          break;

          default:
          Serial.println("Punching Error");
          break;
        }
   
}

/***********************************************************************************************/

                                     /* file name */

/************************************************************************************************/

void file_name()
{
  String mont;
  char str[5]=".csv";                  //%s- string
  mont=(rtc.getMonthStr());
  mont.toCharArray(mo,9);
  sprintf(fname,"%s%s",mo,str);        // sprintf function is to combine different datatypes
  myFile = SD.open(fname, FILE_WRITE); 
  if (myFile)
                  {  
                    EEPROM.get(4000, pre_mo); 
                    if(strcmp(pre_mo,mo))             // 0 if same
                    {
                      myFile.print("\nDate,Name,Time In,Time Out\n");
                      strcpy(pre_mo,mo);
                      Serial.println(pre_mo);
                      EEPROM.put(4000, pre_mo);
                      myFile.close();  
                      delay(1000); 
                    }                         
                  }
                  else 
                  {
                    Serial.println("error opening File1 ");      // if the file didn't open, print an error:
                    digitalWrite(yellow, LOW);
                    digitalWrite(green, LOW);
                    digitalWrite(red, HIGH);
                    lcd.clear();
                    lcd.print("   File Error   ");
                    delay(1000);
                    digitalWrite(red, LOW);
                  }
 
}   

 /***********************************************************************************************/
 
                                           /* date check */

 /***********************************************************************************************/
 void datecheck()
 {
  SD_avilable();
  String hou;
  int cnt1=1,cnt2=1,cnt3=1,adrs;
  char prst[9];                                         // if not punched in at the end of the day it automatically write leave
  lcd.setCursor(4, 0);
  lcd.print(rtc.getTimeStr());
  hou=rtc.getTimeStr();
  finger.getTemplateCount();
  hou.toCharArray(prst,9);
  cnt1 = strcmp(prst,"23:59:59");                                        // Updating time set here now set 12 AM
  cnt2 = strcmp(prst,"04:00:00"); 
  cnt3 = strcmp(prst,"08:00:00");
  
  if(cnt1==0|cnt2==0|cnt3==0)
    {
      Serial.println("Date Update");
      lcd.clear();
      lcd.print("Updating.......");
      digitalWrite(yellow, HIGH);
      for(int j=0;j<finger.templateCount;j++)
      {

        
        adrs = (j*30);
        EEPROM.get(adrs, emp[j]);
        myFile = SD.open(fname, FILE_WRITE);                                                            //.csv
                  if (myFile)
                  {
                    if(emp[j].inout == 'i')
                      {
                        myFile.println(String(rtc.getDateStr())+","+String(emp[j].name)+","+String(emp[j].intim)+","); 
                        //Serial.println(j);
                      }
                    myFile.close();                                                             // close the file:
                    emp[j].inout= 'o';
                    EEPROM.put(adrs,emp[j]);
                  }
                  else 
                  {
                    Serial.println("error opening File ");      // if the file didn't open, print an error:
                    digitalWrite(yellow, LOW);
                    digitalWrite(green, LOW);
                    digitalWrite(red, HIGH);
                    lcd.clear();
                    lcd.print("   File Error   ");
                    delay(1000);
                    digitalWrite(red, LOW);
                    lcd.clear();
                  }
                  
      }
      delay(1000);
      file_name();
      digitalWrite(yellow, LOW);
      lcd.clear();
    }
 }

/*****************************************************************************************/

                             /*  SD_avilable()  */

/*****************************************************************************************/

void SD_avilable()
{
if (!SD.begin(53)) {
    Serial.println("SD initialization failed!");
    lcd.clear();
    lcd.print("SD initializatin");
    lcd.setCursor(4, 1);
    lcd.print("failed!");
    
    while(1)
    {
      digitalWrite(red, HIGH);
      delay(500);
      digitalWrite(red, LOW);
      delay(500);
    }
  }
}


/********************************************************************************************/


/////////////////////////////////////////////////////////////////////////////
