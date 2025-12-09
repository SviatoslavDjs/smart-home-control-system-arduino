#include <SoftwareSerial.h> //підключення бібліотеки для роботи з Rx Tx
SoftwareSerial mySerial(3, 2); // RX, TX

#include <LiquidCrystal.h> //підключення бібліотеки для роботи з Екраном
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#include "DHT.h"          //підключення бібліотеки для роботи з давачом DHT11
const int DHTPIN = 17;     
DHT dht(DHTPIN, DHT11);

int x=0;               // Об'являємо змінну для збереження стану кнопок
int stat = 0;          //Об'являмо змінну стану (спрацювання)
int mute1=0;
int mute2=0;
const int rfid = 13;   // Об'являємо змінну для  та прийняття значень з rfid вказуємо пін
int rfid_value = 0;    // Створюємо змінну для зберігання значень з порта
const int rele1 = 11; // Об'являємо змінну для реле1 та вказуємо пін (вентилятор)
const int rele2 = 12; // Об'являємо змінну для реле2 та вказуємо пін (обігрівач (симулюєм світлодіодом))
char Incoming_value = 0;
const int D_gas_dym = A1; // Об'являємо змінну для давача газу та диму та вказуємо пін
int D_gas_dym_value = 0; // Створюємо змінну для зберігання значень з давача газу та диму і прирівнюємо до 0
int flag_gas_dym=0;
const int D_zatoplennia = A2; // Об'являємо змінну для давача рівня води (затоплення) та вказуємо пін
int D_zatoplennia_value = 0; // Створюємо змінну для зберігання значень з давача рівня води і прирівнюємо до 0
int flag_zatoplennia=0;
const int D_dveri = 18; // Об'являємо змінну для давача відкриття дверей та вказуємо пін
int D_dveri_value = 0; // Створюємо змінну для зберігання значень з давача відкриття дверей і прирівнюємо до 0
int flag_dveri=0;
const int buzzer = 19; // Об'являємо змінну для buzzer (п'єзовипромінювач) та вказуємо пін


void setup() {
  pinMode(D_dveri, INPUT); // Налаштовуємо пін давача відкриття дверей на вхід
  pinMode(rfid, INPUT);   // Налаштовуємо пін rfid на вхід
  pinMode(rele1, OUTPUT); // Налаштовуємо пін реле1 на вхід
  pinMode(rele2, OUTPUT); // Налаштовуємо пін реле2 на вхід
  digitalWrite (rele1, HIGH); // Записуємо на вхід 1, щоб реле включалось на значенні LOW
  digitalWrite (rele2, HIGH); // Записуємо на вхід 1, щоб реле включалось на значенні LOW
  delay(2000);         //час на ініціалізацію модулів
  Serial.begin(9600); //швидкість порта
  lcd.begin(16, 2);  //ініціалізуємо LCD та запускаємо його
  dht.begin();      //ініціалізуємо та запускаємо давач DHT11
lcd.setCursor(0, 0); // Встановлюємо курсор на 1 символ 1 рядка
  lcd.print ("Hello! Wait...");   //Друкуємо повідомлення
  mySerial.begin(9600); //ініціалізуємо та налаштовуємо швидкість порта
  Serial.println("Initializing...");     // Печать текста
  mySerial.println("AT"); // команда на перевірку роботи GSM
   updateSerial();
   delay(20000); 
   lcd.setCursor(0, 1); // Встановлюємо курсор на 1 символ 1 рядка
  lcd.print ("Ready");   //Друкуємо повідомлення
   delay(2000); 
  mySerial.println("AT+CMGF=1"); // Режим кодування СМС (для лат. букв)
  updateSerial();
 
}

void loop() { 
 if(Serial.available() > 0)  
  {
    Incoming_value = Serial.read();      
    Serial.print(Incoming_value);        
    Serial.print("\n");        
    if(Incoming_value == '1')             
      digitalWrite(rele1, LOW);  
    else if(Incoming_value == '0')       
      digitalWrite(rele1, HIGH);   
          else if(Incoming_value == '4')       
      digitalWrite(rele2, LOW);   
          else if(Incoming_value == '3')       
      digitalWrite(rele2, HIGH);   
  }                            
updateSerial();
rfid_value = digitalRead(rfid); // Зчитуємо значення з піна RFID та записуєємо в змінну
x = analogRead (A0); //Зчитуємо значення з аналогово піна А0 та записуємо в змінну
if (x < 60)   //Умова спрацювання кнопки
{ 
  lcd.clear();   // Очищуємо екран від символів
  lcd.setCursor(0, 0); // Встановлюємо курсор на 1 символ 1 рядка
  lcd.print ("Obigriv: OFF");   //Друкуємо повідомлення
  digitalWrite(rele2, HIGH);    //Команда на виключення реле  

  delay(2000);             // Затримка 2 сек.
}
else if (x < 200) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print ("Vent: ON");
  digitalWrite(rele1, LOW);
  
  delay(2000);
}
else if (x <= 400){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print ("Vent: OFF");
  digitalWrite(rele1, HIGH);
  delay(2000);
}
else if (x < 600) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print ("Obigriv: ON");
  digitalWrite(rele2, LOW);
  delay(2000);
}


//клімат контроль
if (stat == 0)   //Умова, якщо ніодин давач не спрацював
{
  float h = dht.readHumidity(); // Вимірюємо вологість та записуємо в змінну
  float t = dht.readTemperature(); // Вимірюємо температуру та записуєм в змінну
  if (isnan(h) || isnan(t)) //Провірка чи працює давач і видає значення
  {  
    lcd.print("Error"); 

  }
  // Вивід на екран значень
  lcd.setCursor(0, 0);
  lcd.print("Volog:         %");
  lcd.setCursor(9, 0);
  lcd.print(h);
  lcd.setCursor(0, 1);
  lcd.print("Temp:          C");
  lcd.setCursor(9, 1);
  lcd.print(t);
  delay(2000);
}

// Давач газу та диму
D_gas_dym_value = analogRead(D_gas_dym); // Зчитуємо значення давача газу та диму та записуємо в змінну
if (D_gas_dym_value >= 150) // записуємо умову (ліміт спрацювання)
{
  stat = 1;    // записуємо в змінну стану 1
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print ("Dym/Gas!");
  if(mute1 == 0);
  {
  tone(buzzer, 1900); //Включаємо сигналізацію (подаємо сигнал на buzzer)
  }
  digitalWrite(rele1, LOW); // Включаємо реле з вентилятором
  delay(1000);
  if (flag_gas_dym == 0)
  {
  mySerial.println("AT+CMGS=\"+380501995829\""); // дає команду на відправку смс
  updateSerial();
   delay(1000);
  mySerial.print("Gas/Dym"); // відправка тексту
  updateSerial();
  mySerial.write(26);
  delay(1000);
  flag_gas_dym=1;
  }
  x = analogRead (A0); // Зчитуємо значення з піна кнопок та записуємо в змінну
  if (x < 750)    // Умова спрацювання кнопки 
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print ("Mute");
  mute1=1;
  noTone(buzzer); //Виключаємо сигналізацію
  delay(1000);
  }
  delay(100);
}
else
{
  flag_gas_dym=0;
  stat = 0;
  mute1=0;
  noTone(buzzer); //Виключаємо сигналізацію
}


// Давач рівня (затоплення)
D_zatoplennia_value = analogRead(D_zatoplennia); //Зчитуємо значення давача та записуємо в змінну
if (D_zatoplennia_value >= 230) //задаєм умову, порогове значення спрацьовування сигналізації
{
  stat = 1;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print ("Zafiksovana Voda");
  if(mute2 == 0);
  {
  tone(buzzer, 1900); //Включаємо сигналізацію (подаємо сигнал на buzzer)
  }
  delay(1000);
  if (flag_zatoplennia == 0)
  {
  mySerial.println("AT+CMGS=\"+380501995829\""); // дає команду на відправку смс
  updateSerial();
  delay(1000);
  mySerial.print("Zatoplennia!"); // відправка тексту
  updateSerial();
  mySerial.write(26);
  delay(1000);
  flag_zatoplennia=1;
  }
  delay(1000);
  x = analogRead (A0);
  if (x < 750) 
  {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print ("Mute");
  mute2=1;
  noTone(buzzer);
  delay(10000);
  }
  delay (5000);
  }
else
  {
  mute2=0;
  flag_zatoplennia=0;
  stat = 0;
  noTone(buzzer);
  }
  
if(rfid_value == 1) //перевірка чи включена сигналізація на rfid
{
//Давач відкриття дверей
D_dveri_value = digitalRead(D_dveri); //Зчитуємо значення з давача відкриття дверей
if (D_dveri_value == LOW) //Задаємо умову (Якщо двері відчинені)
{
  digitalWrite(D_dveri, LOW);
  {
  stat = 1;
  lcd.clear();
  lcd.setCursor(0, 0); // Первый символ первой строк
  lcd.print ("Dveri vidkryto");
  delay(100);
  if (flag_dveri == 0)
  {
  mySerial.println("AT+CMGS=\"+380501995829\""); // дає команду на відправку смс
  updateSerial();
  delay(1000);
  mySerial.print("Dveri!"); // відправка тексту
  updateSerial();
  mySerial.write(26);
  delay(1000);
  flag_dveri=1;
  }
  tone(buzzer, 1700);
  x = analogRead (A0);
  delay (5000);
  }
  }
else
  {
  flag_dveri=0;
  stat = 0;
  noTone(buzzer);
  }
}
}
//цикл для перенаправлення на серійний порт для перевірки роботи GSM
void updateSerial()
{
  delay(500);                           
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());       // Переадресація на серійний порт Ардуїно
  while(mySerial.available()) 
  { 
    Serial.write(mySerial.read());       // Переадресація на серійний порт SIM800
  }
}
}
