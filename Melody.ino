
/* Melody
 * (cleft) 2005 D. Cuartielles for K3
 *
 * Пример использования пьезоизлучателя для проигрывания 
 * мелодии.  
 * Для этого на пьезоизлучатель подаётся сигнал 
 * соответствующей частоты.
 *
 *   Расчет тонов производится следующим образом:
 *
 *       timeHigh = period / 2 = 1 / (2 * toneFrequency)
 *
 * таблица различных тонов:
 *
 * нота      частота     период timeHigh
 * c          261 Hz          3830  1915  
 * d          294 Hz          3400  1700  
 * e          329 Hz          3038  1519  
 * f          349 Hz          2864  1432  
 * g          392 Hz          2550  1275  
 * a          440 Hz          2272  1136  
 * b          493 Hz          2028  1014  
 * C          523 Hz          1912  956
 *
 * http://www.arduino.cc/en/Tutorial/Melody
 */

#define BUZZ_PIN  0
#define BUZZ_TYPE 1 //1-пассивный, 2-активный

byte length = 15;                  // число нот
char notes[] = "ccggaagffeeddc "; // пробел - пауза
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 200;

void playTone(int tone, int duration) { //1275, 400
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    PCF_20.write(BUZZ_PIN, HIGH);
    delayMicroseconds(tone);
    PCF_20.write(BUZZ_PIN, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  
  // проиграть тон, соответствующий ноте
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void playMelody() {
  for (int i = 0; i < length; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); // пауза
    } else {
      playNote(notes[i], beats[i] * tempo);
    }
    
    // пауза между нотами
    delay(tempo / 2); 
  }
}

void Beep(byte NumBeep, int Period) { 
  #if BUZZ_TYPE == 1 
    for (unsigned char i = 0; i < NumBeep; i++) { 
      //for (signed short int k = 0; k < 2400; k++) { 
      for (long i = 0; i < 3 * 200 * 1000L; i += 1275 * 2) {   
        PCF_20.write(BUZZ_PIN, HIGH); 
        delayMicroseconds(1275); 
        PCF_20.write(BUZZ_PIN, LOW); 
        delayMicroseconds(1275); 
      } 
      delay(Period); 
    } 
  
  #elif BUZZ_TYPE == 2 
    for (unsigned char i = 0; i < NumBeep; i++) { 
      PCF_20.write(BUZZ_PIN, HIGH); 
      delay(Period); 
      PCF_20.write(BUZZ_PIN, LOW); 
      delay(100); 
    } 
 
  #endif 
} 

