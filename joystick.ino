 void setupJoystick() {
  Joystick.setXAxisRange(minX, maxX);
  Joystick.setYAxisRange(minY, maxY);
  Joystick.begin();
  Gains gains[FFB_AXIS_COUNT];
  gains[0].frictionGain = friction_gain;
  gains[1].frictionGain = friction_gain;
  Joystick.setGains(gains);
}

void check_joyx () {
  Serial.println("X CAL");
  Joystick.setXAxis(0);
  delay(100);
  Joystick.setXAxis(10000);
  delay(100);
  Joystick.setXAxis(20000);
  delay(100);
  Joystick.setXAxis(30000);
  delay(100);
  Joystick.setXAxis(-10000);
  delay(100);
  Joystick.setXAxis(-20000);
  delay(100);
  Joystick.setXAxis(-30000);
  delay(100);
  Joystick.setXAxis(0);
}

void check_joyy () {
  Serial.println("Y CAL");
  Joystick.setYAxis(0);
  delay(100);
  Joystick.setYAxis(10000);
  delay(100);
  Joystick.setYAxis(20000);
  delay(100);
  Joystick.setYAxis(30000);
  delay(100);
  Joystick.setYAxis(-10000);
  delay(100);
  Joystick.setYAxis(-20000);
  delay(100);
  Joystick.setYAxis(-30000);
  delay(100);
  Joystick.setYAxis(0);
}

void updateJoystickPos() {
  Joystick.setXAxis(pos[0]);
  Joystick.setYAxis(pos[1]);
}

void updateEffects(bool recalculate) {
  for (int i = 0; i < 2; i++) {
    effects[i].frictionMaxPositionChange = 25; // TODO: find proper values for these automatically
    effects[i].inertiaMaxAcceleration = 10;
    effects[i].damperMaxVelocity = 150;
  }

  effects[0].springMaxPosition = maxX;
  effects[1].springMaxPosition = maxY;
  effects[0].springPosition = pos[0];
  effects[1].springPosition = pos[1];

  unsigned long currentMillis;
  currentMillis = millis();
  int16_t diffTime = currentMillis - lastEffectsUpdate;

  if (diffTime > 0 && recalculate) {
    lastEffectsUpdate = currentMillis;
    int16_t positionChangeX = pos[0] - lastX;
    int16_t positionChangeY = pos[1] - lastY;
    int16_t velX = positionChangeX / diffTime;
    int16_t velY = positionChangeY / diffTime;
    int16_t accelX = ((velX - lastVelX) * 10) / diffTime;
    int16_t accelY = ((velY - lastVelY) * 10) / diffTime;

    effects[0].frictionPositionChange = velX;
    effects[1].frictionPositionChange = velY;
    effects[0].inertiaAcceleration = accelX;
    effects[1].inertiaAcceleration = accelY;
    effects[0].damperVelocity = velX;
    effects[1].damperVelocity = velY;

    lastX = pos[0];
    lastY = pos[1];
    lastVelX = velX;
    lastVelY = velY;
    lastAccelX = accelX;
    lastAccelY = accelY;
  } else {
    effects[0].frictionPositionChange = lastVelX;
    effects[1].frictionPositionChange = lastVelY;
    effects[0].inertiaAcceleration = lastAccelX;
    effects[1].inertiaAcceleration = lastAccelY;
    effects[0].damperVelocity = lastVelX;
    effects[1].damperVelocity = lastVelY;
  }

  Joystick.setEffectParams(effects);
  Joystick.getForce(forces);

  if (diffTime > 0 && recalculate) {

    vibration1 = before_forces[0] - forces[0];
    if (vibration1 < 0) {
      vibration1 = (vibration1 * -1) ;
    }


    vibration2 = before_forces[1] - forces[1];
    if (vibration2 < 0) {
      vibration2 = (vibration2 * -1) ;
    }


    // DCS에서 바이브레이션 널뛰기 대응
    if ( before_vibration[0] > 100 && vibration1 == 0 )
    {
      vibration1 = round(before_vibration[0] / 2);
    }
    if ( before_vibration[1] > 100 && vibration2 == 0 )
    {
      vibration2 = round(before_vibration[1] / 2);
    }



    vibration1_m = vibration1 * multiply;
    vibration2_m = vibration2 * multiply;


    
    
    // 레인지범위 비율지정    
    if ( vibration1_m < min_vibration ) {
      vib_data1 = 0;       
    } else {
      vib_data1 = round(255 * ((vibration1_m  -  min_vibration)/(max_vibration - min_vibration))) ;  
    }
    

    if ( vibration2_m < min_vibration ) {
      vib_data2 = 0;       
    } else {
      vib_data2 = round(255 * ((vibration2_m  -  min_vibration)/(max_vibration - min_vibration)));
    }

    if ( vib_data1 > 255 ) vib_data1 = 255;
    if ( vib_data2 > 255 ) vib_data2 = 255;


    Serial.print("D");
    Serial.print("|");
    Serial.print(forces[0]);
    Serial.print("|");
    Serial.print(forces[1]);
    Serial.print("|");
    Serial.print(vibration1);
    Serial.print("|");
    Serial.print(vibration2);
    Serial.print("|");
    Serial.print(vibration1_m);
    Serial.print("|");
    Serial.print(vibration2_m);
    Serial.print("|");
    Serial.print(vib_data1);
    Serial.print("|");
    Serial.println(vib_data2);      


    before_forces[0] = forces[0];
    before_forces[1] = forces[1];
    before_vibration[0] = vibration1;
    before_vibration[1] = vibration2;
    lastTime = currentMillis;
  }
}

void Force_vibration()
{
    unsigned long vibrationMillis;
    
    if ( (before_vib_data1 == 0 && vib_data1 > 0) )
    {
        if (vibrationMillis_check == false) {
          vibrationMillis = millis() + pwr_delay;
          vibrationMillis_check = true;
        }       
    }

    if ( millis() < vibrationMillis && (vib_data1 != 0 || vib_data2 !=0 ))
    {
        analogWrite(6, 0);
        analogWrite(9, 255);
        
        Serial.println("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");      
    }
    else 
    {
      if ( setmode == 0 ) {
        if ( vib_data1 >= vib_data2 )
        {
          analogWrite(6, 0);
          analogWrite(9, vib_data1);       
        }
        else {
          analogWrite(6, 0);
          analogWrite(9, vib_data2);
        }
      }
      else if ( setmode == 1 ) {
          analogWrite(6, 0);
          analogWrite(9, vib_data1);         
      }
      else if ( setmode == 2 ) {
          analogWrite(6, 0);
          analogWrite(9, vib_data2);
      }

      before_vib_data1 = vib_data1;
      before_vib_data2 = vib_data2;
      vibrationMillis_check=false;
    }
}



void Vibration_mode()
{
   if ( client_vibration > 255 ) client_vibration = 255;
   analogWrite(6, 0);
   if ( client_vibration < 150 && client_vibration > 0) {
      analogWrite(9, 255);
      delay(pwr_delay);
   }
   else
   {
      delay(pwr_delay);
   }
    
   analogWrite(9, client_vibration);
}
