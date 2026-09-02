unsigned long TimeOfHoldTrigger6 = 0;  // Czas ostatniego wykrycia stanu HIGH na pinie 6
unsigned long holdStartTime11 = 0;   // Czas rozpoczęcia podtrzymania na pinie 12
bool Holding11 = false;            // Czy trwa podtrzymanie stanu HIGH na pinie 12
bool Holding6 = false;


unsigned long TimeOfHoldTrigger7 = 0;  // Czas ostatniego wykrycia stanu HIGH na pinie 6
unsigned long holdStartTime12 = 0;   // Czas rozpoczęcia podtrzymania na pinie 12
bool Holding12 = false;            // Czy trwa podtrzymanie stanu HIGH na pinie 12
bool Holding7 = false;

bool Pause = false;         
unsigned long PauseTimer = 0;

// Funkcja sprawdzająca, czy któryś z dwóch pinów jest wciśnięty (LOW)
bool isPressed(int pinA, int pinB) {
  return (digitalRead(pinA) == LOW || digitalRead(pinB) == LOW);
}

void setup() { //
  pinMode(7, INPUT_PULLUP); //Przycisk jako wejście
  pinMode(6, INPUT_PULLUP); //Przycisk jako wejście
  pinMode(8, INPUT_PULLUP); //Przycisk jako wejście
  pinMode(9, INPUT_PULLUP); //Przycisk jako wejście
  // pinMode(12, OUTPUT); //Dioda jako wyjście
  // pinMode(11, OUTPUT); //Dioda jako wyjście
  // digitalWrite(12, LOW); //Wyłączenie przekaznika
  // digitalWrite(11, LOW); //Wyłączenie przekaznika
  Serial.begin(9600); //Ustawienie prędkości transmisji
  Serial.println("Witaj!"); //Jednorazowe wysłanie tekstu

  // Konfiguracja pinów 7 i 6 jako wejścia z podciągnięciem (INPUT_PULLUP)

  //Note: Moje testowe próby z językkiem niższego poziomu (coś nie działala jak źle podpiąłem przekaźniki myślałem że nie działa bo za wolno się uruchamiało. Zostawiam jako ciekawostkę.
  // DDRD &= ~(1 << DDD7); // Ustawienie pinu 7 portu D jako wejście
  // PORTD |= (1 << PORTD7); // Włączenie rezystora podciągającego dla pinu 7 portu D
  // DDRD &= ~(1 << DDD6); // Ustawienie pinu 6 portu D jako wejście
  // PORTD |= (1 << PORTD6); // Włączenie rezystora podciągającego dla pinu 6 portu D

  // Konfiguracja pinów 12 i 11 jako wyjścia (OUTPUT)
  DDRB |= (1 << DDB4); // Ustawienie pinu 12 portu B (PB4) jako wyjście
  DDRB |= (1 << DDB3); // Ustawienie pinu 11 portu B (PB3) jako wyjście
  // DDRB |= (1 << DDB1); // Ustawienie pinu 9 portu B (PB3) jako wyjście
  // Wyłączenie bramek (LOW)
  PORTB &= ~(1 << PORTB4); // Ustawienie pinu 12 portu B na LOW
  PORTB &= ~(1 << PORTB3); // Ustawienie pinu 11 portu B na LOW
  // PORTB |=(1 << PORTB1); // Ustawienie pinu 9 portu B na LOW
}



void loop() {
  unsigned long currentTime = millis();
  // Serial.println(currentTime); 
  if (!Pause) {
    if (isPressed(6, 8) && !isPressed(7, 9)){ //Góra
      digitalWrite(11, LOW);
      Holding6 = true;
      // Serial.println("Press button 6");
    } else{
      // Serial.println("Unpress button 6");
      TimeOfHoldTrigger6 = currentTime;
      digitalWrite(11, HIGH);
      Holding6 = false;
    }

    if (isPressed(6, 8) && !isPressed(7, 9){ //Dół
      digitalWrite(12, LOW);
      Holding7 = true;
      // Serial.println("Press button 7");
    } else{
      // Serial.println("Unpress button 7");
      TimeOfHoldTrigger7 = currentTime;
      digitalWrite(12, HIGH);
      Holding7 = false;
    }


    if (currentTime - TimeOfHoldTrigger6 >= 3000UL && Holding6 && !Holding11) { //Podtrzymanie góra
      TimeOfHoldTrigger6 = currentTime;  
      Serial.println("Hold ON");
      holdStartTime11 = currentTime;  
      Holding11 = true;
    }

    if (currentTime - TimeOfHoldTrigger7 >= 3000UL && Holding7 && !Holding12) { //Podtrzymanie dół
      TimeOfHoldTrigger7 = currentTime; 
      Serial.println("Hold ON");
      holdStartTime12 = currentTime;  
      Holding12= true;
    }

    if(Holding11){
      digitalWrite(11, LOW);
    }

    if(Holding12){
      digitalWrite(12, LOW);
    }

    if(currentTime - holdStartTime11 >= 15000UL && Holding11){ //Czas podtrzymania
      holdStartTime11 = currentTime;  
      digitalWrite(11, HIGH);
      digitalWrite(12, HIGH);
      Holding11 = false;
      Serial.println("Hold OFF");
    } else if (Holding11 && digitalRead(7) == LOW){
      digitalWrite(11, HIGH);
      digitalWrite(12, HIGH);
      Holding11 = false;
      Serial.println("Pause ON");
      PauseTimer = currentTime;
      Pause = true; // Pausa się załącza tylko jak zmienia się stan podczas podtrzymania
    }

    if(currentTime - holdStartTime12 >= 15000UL && Holding12){  //Czas podtrzymania
      holdStartTime12 = currentTime;  
      digitalWrite(11, HIGH);
      digitalWrite(12, HIGH);
      Holding12 = false;
      Serial.println("Hold OFF");
    } else if (Holding12 && digitalRead(6) == LOW){
      digitalWrite(11, HIGH);
      digitalWrite(12, HIGH);
      Holding12 = false;
      Serial.println("Pause ON");
      PauseTimer = currentTime;
      Pause = true; // Pausa się załącza tylko jak zmienia się stan podczas podtrzymania
    }
  }

  if(currentTime - PauseTimer >= 1000UL && Pause){ 
    PauseTimer = currentTime;
    Serial.println("Pause OFF");
    Pause = false; // Pausa się wyłącza, można swobodnie pracować dalej
  }
 
}