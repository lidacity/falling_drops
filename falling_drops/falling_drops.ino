// https://github.com/lidacity/falling_drops
// https://wokwi.com/projects/392864539998877697

#include <Adafruit_NeoPixel.h>


// класс, где на все ленты задан единый массив цветов Pixels
class NeoPixel : public Adafruit_NeoPixel
{
  public:
    NeoPixel(uint8_t *Pixels, uint16_t n, int16_t p, neoPixelType t) : Adafruit_NeoPixel(0, p, t)
    {
      numBytes = n * 3;
      numLEDs = n;
      pixels = Pixels;
    }

    NeoPixel() : Adafruit_NeoPixel() {}
};


// ленту подключать через резистор 470 Ом
// питание лент от отдельного стабилизированного источника постоянного напряжения 5V
// Земля общая с блоком питания Ардуино
// стандарт потребления одного цвета светодиода 0.012 А (12 мА), три светодиода (белый цвет) – 0.036 А (36 мА)

#define COUNT_STRIP 12 // количество лент
#define COUNT_PIXELS 32 // количество светодиодов в каждой ленте (максимальное значение при оперативной памяти 2к - около 500)
// стандартное использования памяти для NeoPixel = COUNT_STRIP * (40 + 3 * COUNT_PIXELS)

// перечисление пинов, куда подключены ленты
const byte DATA_PINS[COUNT_STRIP] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

#define PIN_HUE A0 // пин потенциометра задания тона
#define PIN_HUE_RAND A1  // пин переключателя увеличения или задания тона
#define PIN_LENGTH A2  // пин потенциометра задания длины метеора
#define PIN_LENGTH_RAND A3  // пин переключателя случайной или регулировки длины метеора
#define PIN_PAUSE A4  // пин потенциометра задания паузы между вылетами метеора
#define PIN_PAUSE_RAND A5  // пин переключателя случайной или регулировки паузы межды вылетами метеора
#define PIN_DELAY A6 // пин потенциометра задания скорости движения метеора
#define PIN_DELAY_RAND A7  // пин переключателя случайной или задания скорости движения метеора
#define PIN_METEOR 0  // пин переключателя внешнего вида метеора
#define PIN_DIRECTION 1  // пин переключателя направления движения метеора



struct Parameters
{
  byte Phase; // позиция метеора на ленте
  unsigned long Tick; // задержка скорости падения метеора
  byte Decrease; // уменьшение яркости каждого последующего пикселя
  //
  word Hue; // тон метеора
  byte Length; // длина метеора
  unsigned long Pause; // задержка метеора до следующего вылета
  unsigned long Delay; // скорость падения метеора
  bool Meteor; // тип метеора
  bool Direction; // направление движения метеора
};

Parameters Led[COUNT_STRIP]; // массив настроек лент
NeoPixel Strip[COUNT_STRIP]; // массив лент
uint8_t Pixels[3 * COUNT_PIXELS]; // единый массив цветов


void setup()
{
  //Serial.begin(9600);
  // включение дополнительных входов
  pinMode(PIN_HUE, INPUT);
  pinMode(PIN_HUE_RAND, INPUT_PULLUP);
  pinMode(PIN_LENGTH, INPUT);
  pinMode(PIN_LENGTH_RAND, INPUT_PULLUP);
  pinMode(PIN_PAUSE, INPUT);
  pinMode(PIN_PAUSE_RAND, INPUT_PULLUP);
  pinMode(PIN_DELAY, INPUT);
  pinMode(PIN_DELAY_RAND, INPUT_PULLUP);
  pinMode(PIN_METEOR, INPUT_PULLUP);
  pinMode(PIN_DIRECTION, INPUT_PULLUP);
  // стартовая инициализация лент
  for (byte i=0; i<COUNT_STRIP; i++)
  {
    Led[i] = Init(0, true);
    Strip[i] = NeoPixel(Pixels, COUNT_PIXELS, DATA_PINS[i], NEO_GRB + NEO_KHZ800);
    Strip[i].begin();
  }
}
 
 
void loop()
{
  unsigned long Millis = millis();
  for (byte i=0; i<COUNT_STRIP; i++) // перебор лент
  {
    if (Millis > Led[i].Pause and Millis > Led[i].Tick) // вышла задержка до следующего вылета метеора И скорость падения метеора
    {
      Led[i].Tick = Millis + Led[i].Delay;
      //
      Strip[i].clear();
      if (Led[i].Phase <= COUNT_PIXELS + Led[i].Length)  // если метеор летит
      {
        int Len = 255 << !Led[i].Meteor;
        for (byte j=0; j<=Led[i].Length+1; j++)
        {
          short Index = Led[i].Direction ? Led[i].Phase - j : COUNT_PIXELS - (Led[i].Phase - j);
          if ((0 <= Index) and (Index <= COUNT_PIXELS)) // метеор считается только в видимой части ленты
          {
            short Val = Adafruit_NeoPixel::gamma8(Led[i].Meteor ? Len : 255 - abs(Len - 255));
            uint32_t Color = Adafruit_NeoPixel::ColorHSV(Led[i].Hue, 255, Val);
            Strip[i].setPixelColor(Index, Color);
          }
          Len -= Led[i].Decrease;
        }
        Led[i].Phase++;
      }
      else         
        Led[i] = Init(Led[i].Hue, false); // если метеор вылетел за пределы
      //
      Strip[i].show();
    }
  }
}


// вернуть тон метеора
int32_t GetHue(int32_t Hue)
{
  bool IsHue = digitalRead(PIN_HUE_RAND) == LOW;
  if (IsHue)
  {
    word Result = map(analogRead(PIN_HUE), 0, 1023, 0, 65535);
    return random(Result - 1024, Result + 1024);
  }
  else
    return Hue + random(256, 1024);
}


// вернуть длину метеора
int GetLength()
{
  bool IsLength = digitalRead(PIN_LENGTH_RAND) == LOW;
  if (IsLength)
    return map(analogRead(PIN_LENGTH), 0, 1023, 4, 32);
  else
    return random(8, 16);
}


// вернуть задержку между вылетами метеора
int GetPause()
{
  bool IsPause = digitalRead(PIN_PAUSE_RAND) == LOW;
  if (IsPause)
  {
    word R = map(analogRead(PIN_PAUSE), 0, 1023, 16, 4096);
    return random(0, R);
  }
  else
    return random(4, 256) << 2;
}


// вернуть скорость падения метеора
int GetDelay()
{
  bool IsDelay = digitalRead(PIN_DELAY_RAND) == LOW;
  if (IsDelay)
    return map(analogRead(PIN_DELAY), 0, 1023, 0, 512);
  else
    return random(1, 32) << 2;
}


// вернуть внешний вид метеора
int IsMeteor()
{
  return random(2) or digitalRead(PIN_METEOR) == LOW;
}


// вернуть направление движения метеора
int IsDirection()
{
  return random(2) or digitalRead(PIN_DIRECTION) == LOW;
}


// инициализация параметров ленты с необходимыми настройками
Parameters Init(uint32_t Hue, bool Enable)
{
  Parameters Result;
  //
  Result.Hue = GetHue(Hue);
  Result.Length = GetLength();
  Result.Pause = millis() + GetPause();
  Result.Delay = GetDelay();
  Result.Meteor = IsMeteor();
  Result.Direction = IsDirection();
  //
  Result.Phase = 0;
  Result.Tick = Result.Delay;
  Result.Decrease = (255 << !Result.Meteor) / (Result.Length + 1);
  //
  return Result;
}
