// Adafuit NeoPixel v1.12.0
#include <Adafruit_NeoPixel.h>
 
// ноги для каждой ленты подключать через резистор 470 Ом
// питание лент от отдельного стабилизированного источника постоянного напряжения 5V
// Земля общая с блоком питания Ардуино
// Внимание: потребление одного цвета светодиода 0.012 А (12 мА), соответственно три светодиода (белый цвет) – 0.036 А (36 мА)

#define COUNT_STRIP 12 // количество лент (максимальное значение = 12 или 8 в зависимости от количества дискретных пинов, + можно использовать расширитель портов ввода-вывода, например при использовании PCF8574 максимальное значение += 64)
#define COUNT_PIXELS 25 // количество светодиодов в каждой ленте (максимальное значение = 255-LENGTH_METEORS)
// использования памяти для NeoPixel = COUNT_STRIP * (40 + 3 * COUNT_PIXELS), не превышайте использование памяти вашего микроконтроллера

#define LENGTH_METEORS 16 // длина метеора (максимальное значение = 255)
#define INCREASE_BRIGHTNESS 256/LENGTH_METEORS

const byte DATA_PINS[COUNT_STRIP] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }; // перечисление пинов, куда подключены ленты
#define PIN_COLOR A0 // пин резистора регулировки цвета
#define PIN_DELAY A1 // пин резистора регулировки скорости
#define PIN_RAND A2  // пин переключателя случайного или увеличения цвета


struct Strips
{
  Adafruit_NeoPixel Strip; // адресная лента
  bool Enable; // включение ленты
  int32_t Phase; // позиция метеора на ленте
  int32_t Pause; // задержка (в цикле loop) до следующей ленты
  int32_t Hue; // цвет (тон) метеора
};

Strips Led[COUNT_STRIP]; // массив лент


void setup()
{
  // стартовая инициализация лент
  int32_t Hue = GetHue();
  for (int i=0; i<COUNT_STRIP; i++)
  {
    Led[i] = { Adafruit_NeoPixel(COUNT_PIXELS, DATA_PINS[i], NEO_GRB + NEO_KHZ800), true, 0, 0, Hue };
    Led[i].Strip.begin();
  }
  // включение дополнительных входов
  pinMode(PIN_COLOR, INPUT);
  pinMode(PIN_DELAY, INPUT);
  pinMode(PIN_RAND, INPUT_PULLUP);
}
 
 
void loop()
{
  for (int i=0; i<COUNT_STRIP; i++) // перебор лент
  {
    if (Led[i].Enable) // если на ленте включён метеор
    {   
      Led[i].Strip.clear();
      if (Led[i].Phase > COUNT_PIXELS + LENGTH_METEORS) // если метеор вылетел за пределы
      {
        Led[i].Enable = false;
        Led[i].Phase = 0;
        Led[i].Pause = random(10, 50);
        if (IsHue())
          Led[i].Hue = GetHue(); // тон читается с потенциометра
        else
          Led[i].Hue += 128; // тон плавно увеличивается
      }
      else // иначе рассчитывается метеор на ленте
      {
        int Index = Led[i].Phase;
        int32_t Hue = Led[i].Hue;
        int Val = 255;
        while ((Index >= 0) && (Val >= 0))
        {
          if (Index < COUNT_PIXELS) // метеор считается только в видимой части ленты
          {
            uint32_t Color = Led[i].Strip.ColorHSV(Hue, 255, Val);
            Led[i].Strip.setPixelColor(Index, Color);
          }
          Val -= INCREASE_BRIGHTNESS;
          Index--;
        }
        Led[i].Phase++;
      }
      Led[i].Strip.show(); // отображение метеора на ленте
    }
    else // иначе на ленте метеор выключён
    {
      if (Led[i].Pause > 0)
        Led[i].Pause--; // то считаем длительность паузы
      else
        Led[i].Enable = true; // и по окончании включаем ленту снова
    }
  }
  int StayTime = GetStay(); // чтение с потенциометра задержки
  delay(StayTime);
}


bool IsHue()
{
  return digitalRead(PIN_RAND) == HIGH; // чтение выбора тона
}


int32_t GetHue()
{
  word Result = map(analogRead(PIN_COLOR), 0, 1023, 0, 65535); // в цветовом ряду HSV у NeoPixel - тон hue описывается в диапазоне от 0 до 65535
  return random(Result - 1024, Result + 1024); // с коридором флюктуации тона
}

int GetStay()
{
  return map(analogRead(PIN_DELAY), 0, 1023, 5, 95); // чтение показателя задержки
}
