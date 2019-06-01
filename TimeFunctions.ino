/* 
 * Alauddin Ansari
 * 2018-12-15
 * Time and DS1307 related Functions
 * Dependencies:
 * 1. Time Lib
 * 2. DS1307RTC Lib
 */

void connectDS1307() {
  
  while(timeStatus() != timeSet) {
    setSyncProvider(RTC.get);
    delay(1000);
  }
  // setSyncInterval(3600);
}

int getDriftedTime() {
  int mmm = month();

  switch(mmm) {
    case 11:
      return 1;
    case 12:
    case 1:
      return 2;
    case 2:
      return 1;
    default:
      return 0;
  }
}

void syncDriftedTime(int sec) {
  int yy, mmm, dd, hh, mm, ss;

  if (sec <= 0 && sec >= 60) {
    return;
  }
  
  yy = year();
  mmm = month();
  dd = day();
  hh = hour();
  mm = minute();
  ss = second();

  // Increased 1 sec only to avoid callback loop.
  sec++;
  ss++;

  delay(sec * 1000);

  // correct time
  if (ss >= 60) {
    ss = ss - 60;
    mm++;
    if (mm == 60) {
      mm = 0;
      hh++;
      if (hh == 24) {
        hh = 0;
        dd++;
      }
    }
  }

  setTime(hh, mm, ss, dd, mmm, yy);
}

boolean validateTime() {
  int yy = year();
  int mon = month();
  int dd = day();
  int hh = hour();
  
//  if (yy < 2000 || yy > 2069) {
//    return false;
//  }
  if (mon <= 0 || mon > 12) {
    return false;
  }
  if (dd <= 0 || dd > 31) {
    return false;
  }
  if (hh < 0 || hh > 23) {
    return false;
  }
  return true;
}
