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
