// https://github.com/lidacity/falling_drops
// https://wokwi.com/projects/392864539998877697

#include <Adafruit_NeoPixel.h>
 
// ноги для каждой ленты подключать через резистор 470 Ом
// питание лент от отдельного стабилизированного источника постоянного напряжения 5V
// Земля общая с блоком питания Ардуино
// Внимание: потребление одного цвета светодиода 0.012 А (12 мА), соответственно три светодиода (белый цвет) – 0.036 А (36 мА)

#define COUNT_STRIP 12 // количество лент (максимальное значение = 12 или 8 в зависимости от количества дискретных пинов, + можно использовать расширитель портов ввода-вывода, например при использовании PCF8574 максимальное значение += 64)
#define COUNT_PIXELS 32 // количество светодиодов в каждой ленте (максимальное значение = 255-LENGTH_METEORS)
// использования памяти для NeoPixel = COUNT_STRIP * (40 + 3 * COUNT_PIXELS), не превышайте использование памяти вашего микроконтроллера

// перечисление пинов, куда подключены ленты
const byte DATA_PINS[COUNT_STRIP] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

// если некоторые пины управления не задействованы - их можно отключить
// для этого необходимо их закоментировать (или указать #undef)
#define HUE // тон метеора
#define DELAY // скорость движения метеора
#define LENGTH // длина метеора
#define PAUSE // пауза между вылетами метеора
#define METEOR  // внешний вид метеора

#if defined(HUE)
  #define PIN_HUE A0 // пин потенциометра задания тона
  #define PIN_RAND_HUE A1  // пин переключателя увеличения или задания тона
#endif
#if defined(DELAY)
  #define PIN_DELAY A2 // пин потенциометра задания скорости движения метеора
  #define PIN_RAND_DELAY A3  // пин переключателя случайной или задания скорости движения метеора
#endif
#if defined(LENGTH)
  #define PIN_LENGTH A4  // пин потенциометра задания длины метеора
  #define PIN_RAND_LENGTH A5  // пин переключателя случайной или регулировки длины метеора
#endif
#if defined(PAUSE)
  #define PIN_PAUSE A6  // пин потенциометра задания паузы между вылетами метеора
  #define PIN_RAND_PAUSE A7  // пин переключателя случайной или регулировки паузы межды вылетами метеора
#endif
#if defined(METEOR)
  #define PIN_METEOR 0  // пин переключателя внешнего вида метеора
  #define PIN_DIRECTION 1  // пин переключателя направления движения метеора
#endif


struct Params
{
  bool Enable; // включена ли лента
  byte Phase; // позиция метеора на ленте
  byte Pause; // задержка (loop) метеора до следующего
  word Hue; // тон метеора
  byte Delay; // задержка скорости падения (loop) метеора
  byte DelayStart; // скорость падения (loop) метеора
  byte Length; // длина метеора
  bool Meteor; // тип метеора
  bool Direction; // направление движения метеора
};

struct Strips
{
  Adafruit_NeoPixel Strip; // адресная лента
  Params Parameter; // настройки метеора
};

Strips Led[COUNT_STRIP]; // массив лент


// Note:
// для метеоров COUNT_STRIP*COUNT_PIXELS задействовано памяти 1632
// для массива Led задействовано памяти 384


void setup()
{
#if defined(DEBUG)
  Serial.begin(9600);
  Serial.println(COUNT_STRIP * (40 + 3 * COUNT_PIXELS));
  Serial.println(sizeof(Led));
#endif
  // включение дополнительных входов
#if defined(HUE)
  pinMode(PIN_HUE, INPUT);
  pinMode(PIN_RAND_HUE, INPUT_PULLUP);
#endif
#if defined(DELAY)
  pinMode(PIN_DELAY, INPUT);
  pinMode(PIN_RAND_DELAY, INPUT_PULLUP);
#endif
#if defined(LENGTH)
  pinMode(PIN_LENGTH, INPUT);
  pinMode(PIN_RAND_LENGTH, INPUT_PULLUP);
#endif
#if defined(PAUSE)
  pinMode(PIN_PAUSE, INPUT);
  pinMode(PIN_RAND_PAUSE, INPUT_PULLUP);
#endif
#if defined(METEOR)
  pinMode(PIN_METEOR, INPUT_PULLUP);
  pinMode(PIN_DIRECTION, INPUT_PULLUP);
#endif
  // стартовая инициализация лент
  for (byte i=0; i<=COUNT_STRIP-1; i++)
  {
    Params Parameter = Init(true);
    Led[i] = { Adafruit_NeoPixel(COUNT_PIXELS, DATA_PINS[i], NEO_GRB + NEO_KHZ800), Parameter };
    Led[i].Strip.begin();
  }
}
 
 
void loop()
{
  for (byte i=0; i<COUNT_STRIP; i++) // перебор лент
  {
    if (!IsDelay() and (Led[i].Parameter.Delay > 0))
      Led[i].Parameter.Delay--; // если случайная скорость падения
    else
    {
      Led[i].Parameter.Delay = Led[i].Parameter.DelayStart;
      if (Led[i].Parameter.Enable) // если на ленте включён метеор
      {   
        Led[i].Strip.clear();
        if (Led[i].Parameter.Phase > COUNT_PIXELS + Led[i].Parameter.Length)
        {
          Led[i].Parameter = Init(false); // если метеор вылетел за пределы
        }
        else // иначе рассчитывается метеор на ленте
        {
          byte Phase = Led[i].Parameter.Phase;
          word Hue = Led[i].Parameter.Hue;
          word Len = 255 * (!Led[i].Parameter.Meteor + 1);
          byte Decrease = Len / (Led[i].Parameter.Length + 1);
          //
          for (byte j=0; j<=Led[i].Parameter.Length+1; j++)
          {
            if ((0 <= (Phase - j)) and ((Phase - j) < COUNT_PIXELS)) // метеор считается только в видимой части ленты
            {
              byte Index = Led[i].Parameter.Direction ? Phase - j : COUNT_PIXELS - Phase + j;
              int Val = Len - j * Decrease;
              Val = Led[i].Parameter.Meteor ? Val : 255 - abs(Val - 255);
              uint32_t Color = Led[i].Strip.ColorHSV(Hue, 255, Val);
              Led[i].Strip.setPixelColor(Index, Color);
            }
          }
          /*byte Index = Led[i].Parameter.Phase;
          int32_t Hue = Led[i].Parameter.Hue;
          int Val = 255 * (!Led[i].Parameter.Meteor + 1);
          while ((Index >= 0) && (Val >= 0))
          {
            if (Index < COUNT_PIXELS) // метеор считается только в видимой части ленты
            {
              byte Index1 = Led[i].Parameter.Direction ? Index : COUNT_PIXELS - Index;
              int Value = Led[i].Parameter.Meteor ? Val : 255 - abs(Val - 255);
              uint32_t Color = Led[i].Strip.ColorHSV(Hue, 255, Value);
              Led[i].Strip.setPixelColor(Index1, Color);
            }
            Val -= 255 * (!Led[i].Parameter.Meteor + 1) / (Led[i].Parameter.Length + 1);
            Index--;
          }*/
          Led[i].Parameter.Phase++;
        }
        Led[i].Strip.show();
      }
      else // иначе на ленте метеор выключён
      {
        if (Led[i].Parameter.Pause > 0)
          Led[i].Parameter.Pause--; // то считаем длительность паузы
        else
          Led[i].Parameter.Enable = true; // и по окончании включаем ленту снова
      }
    }
  }
  int DelayTime = GetDelay();
  delay(DelayTime);
}


// вернуть тон метеора
// в цветовом ряду HSV у NeoPixel тон hue описывается в диапазоне от 0 до 65535
// с коридором флюктуации тона
int32_t GetHue(int32_t Hue)
{
#if defined(HUE)
  bool IsHue = digitalRead(PIN_RAND_HUE) == LOW; // увеличение или задание тона
  if (IsHue)
  {
    word Result = map(analogRead(PIN_HUE), 0, 1023, 0, 65535);
    return random(Result - 1024, Result + 1024);
  }
  else
    return Hue + random(256, 1024);
#else
  return Hue + random(256, 1024);
#endif
}


// вернуть случайную или заданную длину метеора
int IsDelay()
{
#if defined(DELAY)
  return digitalRead(PIN_RAND_DELAY) == LOW;
#else
  return false;
#endif
}


// вернуть скорость падения метеора
int GetDelay()
{
#if defined(DELAY)
  bool IsDelay = digitalRead(PIN_RAND_DELAY) == LOW;
  if (IsDelay)
    return map(analogRead(PIN_DELAY), 0, 1023, 5, 95);
  else
    return 1;
#else
  return 1;
#endif
}


// вернуть длину метеора
int GetLength()
{
#if defined(LENGTH)
  bool IsLength = digitalRead(PIN_RAND_LENGTH) == LOW;
  if (IsLength)
    return map(analogRead(PIN_LENGTH), 0, 1023, 5, 16);
  else
#else
    return random(5, 16);
#endif
  return random(5, 16);
}


// вернуть задержку между вылетами метеора
int GetPause()
{
#if defined(PAUSE)
  bool IsPause = digitalRead(PIN_RAND_PAUSE) == LOW;
  if (IsPause)
  {
    uint16_t R = map(analogRead(PIN_PAUSE), 0, 1023, 16, 1023);
    return random(0, R);
  }
  else
    return random(16, 128);
#else
  return random(16, 128);
#endif
}


// вернуть внешний вид метеора
int IsMeteor()
{
#if defined(METEOR)
  return random(2) or digitalRead(PIN_METEOR) == LOW;
#else
  return true;
#endif
}


// вернуть направление движения метеора
int IsDirection()
{
#if defined(METEOR)
  return random(2) or digitalRead(PIN_DIRECTION) == LOW;
#else
  return true;
#endif
}


// инициализация параметров ленты с необходимыми настройками
Params Init(bool Enable)
{
  Params Result;
  Result.Enable = Enable;
  Result.Phase = 0;
  Result.Pause =  GetPause();
  Result.Hue = GetHue(Result.Hue);
  Result.DelayStart = random(1, 50);
  Result.Length = GetLength();
  Result.Meteor = IsMeteor();
  Result.Direction = IsDirection();
  return Result;
}
