// https://github.com/lidacity/falling_drops
// https://wokwi.com/projects/392864539998877697

#include <Adafruit_NeoPixel.h>
 
// ленту подключать через резистор 470 Ом
// питание лент от отдельного стабилизированного источника постоянного напряжения 5V
// Земля общая с блоком питания Ардуино
// стандарт потребления одного цвета светодиода 0.012 А (12 мА), три светодиода (белый цвет) – 0.036 А (36 мА)

#define COUNT_STRIP 12 // количество лент
#define COUNT_PIXELS 32 // количество светодиодов в каждой ленте (максимальное значение при оперативной памяти 2к - около 500)
// использования памяти для NeoPixel = COUNT_STRIP * (40 + 3 * COUNT_PIXELS), не превышайте использование памяти вашего микроконтроллера

// перечисление пинов, куда подключены ленты
const byte DATA_PINS[COUNT_STRIP] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

#define PIN_HUE A0 // пин потенциометра задания тона
#define PIN_HUE_RAND A1  // пин переключателя увеличения или задания тона
#define PIN_LENGTH A2  // пин потенциометра задания длины метеора
#define PIN_LENGTH_RAND A3  // пин переключателя случайной или регулировки длины метеора
#define PIN_DELAY A4 // пин потенциометра задания скорости движения метеора
#define PIN_DELAY_RAND A5  // пин переключателя случайной или задания скорости движения метеора
#define PIN_PAUSE A6  // пин потенциометра задания паузы между вылетами метеора
#define PIN_PAUSE_RAND A7  // пин переключателя случайной или регулировки паузы межды вылетами метеора
#define PIN_METEOR 0  // пин переключателя внешнего вида метеора
#define PIN_DIRECTION 1  // пин переключателя направления движения метеора


struct Params
{
  bool Enable; // включена ли лента
  byte Phase; // позиция метеора на ленте
  byte Tick; // задержка скорости падения (loop) метеора
  //
  word Hue; // тон метеора
  byte Length; // длина метеора
  byte Delay; // скорость падения (loop) метеора
  byte Pause; // задержка (loop) метеора до следующего
  bool Meteor; // тип метеора
  bool Direction; // направление движения метеора
};

struct Strips
{
  Adafruit_NeoPixel Strip; // адресная лента
  Params Parameter; // настройки метеора
};

Strips Led[COUNT_STRIP]; // массив лент


void setup()
{
#if defined(DEBUG)
  Serial.begin(9600);
  Serial.println(COUNT_STRIP * (40 + 3 * COUNT_PIXELS));
  Serial.println(sizeof(Led));
#endif
  // включение дополнительных входов
  pinMode(PIN_HUE, INPUT);
  pinMode(PIN_HUE_RAND, INPUT_PULLUP);
  pinMode(PIN_LENGTH, INPUT);
  pinMode(PIN_LENGTH_RAND, INPUT_PULLUP);
  pinMode(PIN_DELAY, INPUT);
  pinMode(PIN_DELAY_RAND, INPUT_PULLUP);
  pinMode(PIN_PAUSE, INPUT);
  pinMode(PIN_PAUSE_RAND, INPUT_PULLUP);
  pinMode(PIN_METEOR, INPUT_PULLUP);
  pinMode(PIN_DIRECTION, INPUT_PULLUP);
  // стартовая инициализация лент
  for (byte i=0; i<=COUNT_STRIP-1; i++)
  {
    Params Parameter = Init(true);
    Led[i] = { Adafruit_NeoPixel(0, DATA_PINS[i], NEO_GRB + NEO_KHZ800), Parameter };
    Led[i].Strip.begin();
  }
  //
}
 
 
void loop()
{
  for (byte i=0; i<COUNT_STRIP; i++) // перебор лент
  {
    if (Led[i].Parameter.Tick == 0)
    {
      Led[i].Parameter.Tick = Led[i].Parameter.Delay;
      if (Led[i].Parameter.Enable) // если на ленте включён метеор
      {   
        Led[i].Strip.updateLength(COUNT_PIXELS);
        //Led[i].Strip.clear();
        if (Led[i].Parameter.Phase <= COUNT_PIXELS + Led[i].Parameter.Length)  // если метеор летит
        {
          byte Phase = Led[i].Parameter.Phase;
          word Hue = Led[i].Parameter.Hue;
          word Len = 255 << !Led[i].Parameter.Meteor;
          byte Decrease = Len / (Led[i].Parameter.Length + 1);
          //
          for (byte j=0; j<=Led[i].Parameter.Length+1; j++)
          {
            if ((0 <= (Phase - j)) and ((Phase - j) <= COUNT_PIXELS)) // метеор считается только в видимой части ленты
            {
              byte Index = Phase - j;
              if (!Led[i].Parameter.Direction)
                Index = COUNT_PIXELS - Index;
              int Val = Len - j * Decrease;
              if (!Led[i].Parameter.Meteor)
                Val = 255 - abs(Val - 255);
              Val = Adafruit_NeoPixel::gamma8(Val);
              uint32_t Color = Adafruit_NeoPixel::ColorHSV(Hue, 255, Val);
              Led[i].Strip.setPixelColor(Index, Color);
            }
          }
          Led[i].Parameter.Phase++;
        }
        else         
          Led[i].Parameter = Init(false); // если метеор вылетел за пределы
        //
        Led[i].Strip.show();
        Led[i].Strip.updateLength(0);
      }
      else // иначе на ленте метеор выключён
      {
        if (Led[i].Parameter.Pause > 0)
          Led[i].Parameter.Pause--; // то считаем длительность паузы между метеорами
        else
          Led[i].Parameter.Enable = true; // и по окончании включаем ленту снова
      }
    }
    else
      Led[i].Parameter.Tick--; // если случайная скорость падения
  }
  delay(1);
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
    return map(analogRead(PIN_LENGTH), 0, 1023, 5, 16);
  else
    return random(5, 32);
}


// вернуть скорость падения метеора
int GetDelay()
{
  bool IsDelay = digitalRead(PIN_DELAY_RAND) == LOW;
  if (IsDelay)
    return map(analogRead(PIN_DELAY), 0, 1023, 1, 95);
  else
    return random(3, 50);
}


// вернуть задержку между вылетами метеора
int GetPause()
{
  bool IsPause = digitalRead(PIN_PAUSE_RAND) == LOW;
  if (IsPause)
  {
    uint16_t R = map(analogRead(PIN_PAUSE), 0, 1023, 16, 1023);
    return random(0, R);
  }
  else
    return random(16, 128);
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
Params Init(bool Enable)
{
  Params Result;
  Result.Enable = Enable;
  Result.Phase = 0;
  Result.Tick = 0;
  //
  Result.Hue = GetHue(Result.Hue);
  Result.Length = GetLength();
  Result.Delay = GetDelay();
  Result.Pause =  GetPause();
  Result.Meteor = IsMeteor();
  Result.Direction = IsDirection();
  return Result;
}
