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

const byte DATA_PINS[COUNT_STRIP] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }; // перечисление пинов, куда подключены ленты
#define PIN_HUE A0 // пин потенциометра задания тона
#define PIN_RAND_HUE A1  // пин переключателя увеличения или задания тона
#define PIN_DELAY A2 // пин потенциометра задания скорости движения метеора
#define PIN_RAND_DELAY A3  // пин переключателя случайной или задания скорости движения метеора
#define PIN_LENGTH A4  // пин потенциометра задания длины метеора
#define PIN_RAND_LENGTH A5  // пин переключателя случайной или регулировки длины метеора


struct Params
{
  bool Enable; // включена ли лента
  byte Phase; // позиция метеора на ленте
  byte Pause; // задержка (loop) метеора до следующего
  int32_t Hue; // тон метеора
  byte Delay; // задержка скорости падения (loop) метеора
  byte DelayStart; // скорость падения (loop) метеора
  byte Length; // длина метеора
};

struct Strips
{
  Adafruit_NeoPixel Strip; // адресная лента
  Params Parameter; // настройки метеора
};

Strips Led[COUNT_STRIP]; // массив лент


void setup()
{
  //Serial.begin(9600);
  // включение дополнительных входов
  pinMode(PIN_HUE, INPUT);
  pinMode(PIN_RAND_HUE, INPUT_PULLUP);
  pinMode(PIN_DELAY, INPUT);
  pinMode(PIN_RAND_DELAY, INPUT_PULLUP);
  pinMode(PIN_LENGTH, INPUT);
  pinMode(PIN_RAND_LENGTH, INPUT_PULLUP);
  // стартовая инициализация лент
  for (int i=0; i<COUNT_STRIP; i++)
  {
    Params Parameter = Init(true);
    Led[i] = { Adafruit_NeoPixel(COUNT_PIXELS, DATA_PINS[i], NEO_GRB + NEO_KHZ800), Parameter };
    Led[i].Strip.begin();
  }
}
 
 
void loop()
{
  for (int i=0; i<COUNT_STRIP; i++) // перебор лент
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
          int Index = Led[i].Parameter.Phase;
          int32_t Hue = Led[i].Parameter.Hue;
          int Val = 255;
          while ((Index >= 0) && (Val >= 0))
          {
            if (Index < COUNT_PIXELS) // метеор считается только в видимой части ленты
            {
              uint32_t Color = Led[i].Strip.ColorHSV(Hue, 255, Val);
              Led[i].Strip.setPixelColor(Index, Color);
            }
            Val -= 256 / Led[i].Parameter.Length;
            Index--;
          }
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
  bool IsHue = digitalRead(PIN_RAND_HUE) == LOW; // увеличение или задание тона
  if (IsHue)
  {
    word Result = map(analogRead(PIN_HUE), 0, 1023, 0, 65535);
    return random(Result - 1024, Result + 1024);
  }
  else
    return Hue + random(256, 1024);
}


// вернуть случайную или заданную длину метеора
int IsDelay()
{
  return digitalRead(PIN_RAND_DELAY) == LOW;
}


// вернуть скорость падения метеора
int GetDelay()
{
  if (IsDelay())
    return map(analogRead(PIN_DELAY), 0, 1023, 5, 95);
  else
    return 1;
}


// вернуть длину метеора
int GetLength()
{
  bool IsLength = digitalRead(PIN_RAND_LENGTH) == LOW;
  if (IsLength)
    return map(analogRead(PIN_LENGTH), 0, 1023, 5, 16);
  else
    return random(5, 16);
}


// инициализация параметров ленты с необходимыми настройками
Params Init(bool Enable)
{
  Params Result;
  Result.Enable = Enable;
  Result.Phase = 0;
  Result.Pause = random(25, 100);
  Result.Hue = GetHue(Result.Hue);
  Result.DelayStart = random(1, 50);
  Result.Length = GetLength();
  return Result;
}
