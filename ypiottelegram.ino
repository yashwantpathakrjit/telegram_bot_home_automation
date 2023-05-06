

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define BOTtoken "57641122:AAGupTwVvq5bGlltWK6soZ09Zc"  
char ssid[] = "YPNPAPDP";         // Wifi SSID 
char password[] = "11378544";     // Wifi Password
String buttons[] = {"Light 1", "Light 2 "};  
int pin[] = {5, 4};                      

bool protection = 0;                      
int chatID_acces[] = {}; 
String on_symbol="✅ ";  
String off_symbol="☑ "; 


WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
int quantity;
int Bot_mtbs = 3000;
long Bot_lasttime;   
bool Start = false;
const int ledPin = 2;
int ledStatus = 0;
String keyboardJson = "";

int messageID;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));


  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String m_id = String(bot.messages[i].message_id);
    if (bot.messages[i].type == "callback_query") {
      String statusMessage;
      for (int i1=0; i1<quantity; i1++) {
       if (bot.messages[i].text == buttons[i1]) {
       digitalWrite (pin[i1], !digitalRead(pin[i1]));
       }
      digitalRead(pin[i1])?statusMessage+=on_symbol:statusMessage+=off_symbol;
      statusMessage+=buttons[i1]; 
      statusMessage+='\n';
      }
      //bot.deleteMessage(bot.messages[i].chat_id, bot.messages[i].message_id);
      bot.sendMessageWithInlineKeyboard(bot.messages[i].chat_id, statusMessage, "", keyboardJson);
      
    } else {
    String text = bot.messages[i].text;
    Serial.println(m_id);
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";
    int i2=0;
    do{
    if (!protection || String(chatID_acces[i2]) == chat_id) {
      if (text == "/switch") {
      String statusMessage;
      for (int i=0; i<quantity; i++) {
      digitalRead(pin[i])?statusMessage+=on_symbol:statusMessage+=off_symbol;
      statusMessage+=buttons[i];  
      statusMessage+='\n';
      }
      bot.sendMessageWithInlineKeyboard(chat_id, statusMessage, "", keyboardJson);
      }

      if (text == "/start") {
      String welcome = "Welcome " + from_name + ".\n";
      welcome += "This is  smart switch, controlled via Telegram.\n\n";
      welcome += "/switch : Click to open Switch Board.\n";
      String keyboardStart = "[[{ \"text\" : \"My YouTube Channel Click here \", \"url\" : \"https://www.youtube.com/c/YPTechDATIA\" }]]";
      bot.sendMessageWithInlineKeyboard(chat_id, welcome, "", keyboardStart);
      }
      break;
    }else{
      if (i2==((sizeof(chatID_acces)/sizeof(int))-1)&&text == "/start"||(sizeof(chatID_acces)/sizeof(int))==0&&text == "/start") {
      bot.sendMessage(chat_id, "No access, Chat ID: "+chat_id, "");
      }
    }

    i2++;
    }while (i2<(sizeof(chatID_acces)/sizeof(int)));
    }
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  quantity=sizeof(pin)/sizeof(int);
  for (int i=0; i<quantity; i++) {
  pinMode(pin[i], OUTPUT);
      }

  for (int i=0; i<quantity; i++) {
  if(i==0) keyboardJson += "[";
  keyboardJson += "[{ \"text\" : \"";
  keyboardJson += buttons[i];
  keyboardJson += "\", \"callback_data\" : \"";
  keyboardJson += buttons[i];
  keyboardJson += "\" }]";
  if(i==quantity-1){
    keyboardJson += "]";
    }else{
  keyboardJson += ",";  
  }
  }
  delay(10);
  client.setInsecure();
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}
