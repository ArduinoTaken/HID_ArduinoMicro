/*
  KeyboardAndMouseControl
  For w1a auto test project
  2019.2.20

  Pin2 -LOW Trigger-- for test mouse move (100,200)
  Pin3 -LOW Trigger-- for test keyboard input "Hello,world!"
  Command:
  //help
  //ENTER->OK
  //ESC->OK
  //C:L/R
  //M:100,200
  //K:Hello,world!
  //Ready ->OK
  //DC:L
*/

#include "Keyboard.h"
#include "Mouse.h"
//status -> LED = 13
const int _led_pin = 13;
// set pin numbers for test buttons:
//2 -LOW Trigger-- for test mouse move (100,200)
const int _Mouse_Test_Btn = 2;
//3 -LOW Trigger-- for test keyboard input "Hello,world!"
const int _Keyboard_Test_Btn = 3;

//receive from COM datas
String comdata="";

//busy flag
bool _is_busy = false;

//status count
int _loop_count = 0;
bool _led_status = false;

void setup() { // initialize the buttons' inputs:
  //set key pin to pullup mode
  pinMode(_Mouse_Test_Btn, INPUT_PULLUP);
  pinMode(_Keyboard_Test_Btn,INPUT_PULLUP);
  pinMode(_led_pin,OUTPUT);
  digitalWrite(_led_pin,LOW);
  Serial.begin(9600);
  // initialize mouse control:
  Mouse.begin();
  Keyboard.begin();
  delay(500);
}

void loop() {
  // use serial input to control the mouse:
  if (Serial.available() > 0) {
    comdata = "";
    while (Serial.available() > 0)
    {
      comdata += char(Serial.read());
      delay(2);
    }
    if (comdata != "" && !_is_busy)
    {
      respondAction();
    }
  }
  delay(10);
  _loop_count +=1;
  
  //status print
  if(_loop_count > 100){
    _loop_count = 0;
    _led_status = !_led_status;
    digitalWrite(_led_pin,_led_status);
  }
  
}
void respondAction(){
  //C:L/R
  //M:100,200
  //K:Hello,world!
  //Ready
  _is_busy = true;
  //trim "\r\n"
  comdata.replace("\r","");
  comdata.replace("\n","");
  comdata.trim();
  
  //check data complete
  if(comdata.length() < 3) {
    Serial.println("ERROR");
    _is_busy = false;
    return;
  }
  
  //mouse move
  if(comdata.startsWith("M:")){
    //M:100,200\r\n ritht:100 down:200
    String cmd = comdata.substring(2);
    int location = cmd.indexOf(',');
    int move_x = cmd.substring(0,location).toInt();
    int move_y = cmd.substring(location+1).toInt();
    Mouse.move(move_x,0,0);
    delay(10);
    Mouse.move(0,move_y,0);
    delay(2);
    Serial.println("OK,"+comdata);
  }
  //mouse single lick
  else if(comdata.startsWith("C:")){
    String cmd = comdata.substring(2);
    if(cmd == "L"){
      Mouse.click(MOUSE_LEFT);
    }else{
      Mouse.click(MOUSE_RIGHT);
    }
    delay(2);
    Serial.println("OK,"+comdata);
  }
  //mouse double lick L button
  else if(comdata.startsWith("DC:")){
    String cmd = comdata.substring(3);
    if(cmd == "L"){
      Mouse.click(MOUSE_LEFT);
      delay(10);
      Mouse.click(MOUSE_LEFT);
    }else{
      Mouse.click(MOUSE_RIGHT);
      delay(10);
      Mouse.click(MOUSE_RIGHT);
    }
    delay(2);
    Serial.println("OK,"+comdata);
  }
  //keyboard input
  else if(comdata.startsWith("K:")){
    //K:Hello,world!
    String cmd = comdata.substring(2);
    Keyboard.print(cmd);
    delay(2);
    Serial.println("OK,"+comdata);
  }
  //press 'ESC'
  else if(comdata == "ESC"){
    Keyboard.press(KEY_ESC);
    delay(5);
    Keyboard.release(KEY_ESC);
    delay(2);
    Serial.println("OK,"+comdata);
  }
  //press 'ENTER'
  else if(comdata == "ENTER"){
    Keyboard.press(KEY_RETURN);
    delay(5);
    Keyboard.release(KEY_RETURN);
    delay(2);
    Serial.println("OK,"+comdata);
  }
  else if(comdata == "Ready"){
    delay(5);
    Serial.println("OK,"+comdata);
  }
  else if(comdata == "help"){
    Serial.println("ENTER->OK");
    Serial.println("ESC->OK");
    Serial.println("Ready->OK");
    Serial.println("M:100,20->OK");
    Serial.println("C:L->OK");
    Serial.println("K:Hello,world!");
    Serial.println("DC:L->OK");
  }
  
  _is_busy = false;
}
