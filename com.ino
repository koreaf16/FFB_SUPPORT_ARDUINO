void get_messages_from_serial()
{
  if (Serial.available() > 0)
  {
    String rxdata = Serial.readStringUntil('\n');

    if (rxdata.substring(0,1) == "P")      // 조이스틱 포지션 받기
    {
        int index1 = rxdata.indexOf(',');
        int index2 = rxdata.length();
        
        String a1 = rxdata.substring(1,index1);
        String a2 = rxdata.substring(index1+1,index2);
        
        pos[0] = a1.toInt();
        pos[1] = a2.toInt();
    
        pos_updated=true;
    }
    else if (rxdata.substring(0,1) == "S")  // 현재 셋팅값 요청에 대한 응답
    {
        Serial.print("A");
        Serial.print("|");
        Serial.print(setmode);
        Serial.print("|");
        Serial.print(multiply);
        Serial.print("|");
        Serial.print(min_vibration);
        Serial.print("|");
        Serial.println(max_vibration);  
    }
    else if (rxdata.substring(0,1) == "W")  // 셋팅값 적용
    {                
        int index1 = rxdata.indexOf(',');
        int index2 = rxdata.indexOf('.');
        int index3 = rxdata.indexOf(':');
        int index4 = rxdata.length();
        
        String a1 = rxdata.substring(1,index1);
        String a2 = rxdata.substring(index1+1,index2);
        String a3 = rxdata.substring(index2+1,index3);
        String a4 = rxdata.substring(index3+1,index4);

        setmode = a1.toInt();
        multiply = a2.toInt();
        min_vibration = a3.toInt();
        max_vibration = a4.toInt();          
        Serial.print("Y");
        Serial.print(min_vibration);
   
    }
    // NON FORCE FEEDBACK 처리
    else if (rxdata.substring(0,1) == "K")  // 기본 FFB모드
    {
         ffbsupport_mode = false;      
    }
    else if (rxdata.substring(0,1) == "L")  // 프로그램모드
    {
         ffbsupport_mode = true;      
    }
    // 유전원모드 처리
    else if (rxdata.substring(0,1) == "E")  // 유전원
    {
            int  wait_delay = 90;
            int  pwr_delay = 10;
            Serial.println("유전원모드");  
    }
    else if (rxdata.substring(0,1) == "R")  // 무전원
    {
            int  wait_delay = 80;
            int  pwr_delay = 20;
            Serial.println("무전원모드");  
    }    
    else if (rxdata.substring(0,1) == "V")  // 바이브레이션 데이터
    {
         int index = rxdata.length();
         String a1 = rxdata.substring(1,index);
         client_vibration = a1.toInt();
    }
    else if (rxdata.substring(0,1) == "H")  // HELLO
    {
         int index = rxdata.length();
         String a1 = rxdata.substring(1,index);
         Serial.print("H");
         Serial.print("|");
         Serial.println(a1);
    }
  }
}


void set_switch_calibration()
{
  button = digitalRead(10);
  button1 = digitalRead(15);

  if (button == LOW || button1 == LOW) { // when the button is pressed
    if (clicks == 1) {
      current = millis();
      clicks = 2;
    }
    else {
      clicks = 1;
      current = millis();
      limits = current + 1000;
    }
    delay(200);
  }
  if (millis() > limits && clicks > 0) {

    if ( clicks == 1 ) {
      check_joyx();
    }
    else if ( clicks == 2 ) {
      check_joyy();
    }
    clicks = 0;
  }
}
