/* 
 * Alauddin Ansari
 * 2018-11-20
 * General Functions
 */

void dropFood(int taskPin, int qty) {
  digitalWrite(taskPin, HIGH);
  delay(qty);
  digitalWrite(taskPin, LOW);
  delay(10);
  digitalWrite(taskPin, LOW); // Just for surity, in case of failure
  delay(100);
  digitalWrite(taskPin, LOW); // Just for surity, in case of failure
  delay(100);
}

int getOnOffStatus(int currentHour, int onTime, int offTime)
{
    if (onTime == offTime) {
        return 0;
    }

    if (onTime < offTime) {
        if (currentHour >= onTime && currentHour < offTime) {
            return 1;
        }
        else if (currentHour >= offTime) {
            return 0;
        }
        else {
            return 0;
        }
    }

    if (onTime > offTime) {
        if (currentHour >= onTime && currentHour <= 23) {
            return 1;
        }
        else if (currentHour < offTime) {
            return 1;
        }
        else if (currentHour >= offTime && currentHour < onTime) {
            return 0;
        }
    }
}


float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//read internal voltage
long readVcc()
{
  long result;
  
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  
  delay(2); // Wait for Vref to settle
  
  ADCSRA |= _BV(ADSC); // Convert
  
  while (bit_is_set(ADCSRA, ADSC));
  
  result = ADCL;
  result |= ADCH << 8;
  
  result = 1126400L / result; // Back-calculate AVcc in mV
  
  return result;
}
