//20512 66% 1861 90%
//4042 13% 455 22%

// Adafuit NeoPixel v1.12.0
#include <Adafruit_NeoPixel.h>
 
// ноги для каждой ленты подключать через резистор 470 Ом
// питание лент от отдельного стабилизированного источника постоянного напряжения 5V
// Земля общая с блоком питания Ардуино
// ВНИМАНИЕ: потребление каждого кусочка ленты из 20 светодиодов = 6 Ватт (ток 1,2А на канал)
// из 12 лент = 72 Ватт, что равно току почти 15А. если предполагается одновременное включение всех
// практика показала что максимальное потребление одного отрезка ленты в режиме сосульки не более 200мА

#define COUNT_STRIP 12 // количество лент (максимальное значение = 12 или 8 в зависимости от количества дискретных пинов, + можно использовать расширитель портов ввода-вывода, например при использовании PCF8574 максимальное значение += 64)
#define COUNT_PIXELS 20 // количество светодиодов в каждой ленте (максимальное значение = 255-LENGTH_METEORS)
// использования памяти для NeoPixel = COUNT_STRIP * (40 + 3 * COUNT_PIXELS), не превышайте использование памяти вашего микроконтроллера

#define LENGTH_METEORS 24 // длина метеора (максимальное значение = 255)
#define INCREASE_BRIGHTNESS 256/LENGTH_METEORS

const byte DATA_PINS[COUNT_STRIP] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }; // перечисление пинов, куда подключены ленты
#define PIN_COLOR A0 // пин резистора регулировки цвета
#define PIN_DELAY A1 // пин резистора регулировки скорости
#define PIN_RAND A2  // пин переключателя случайного или увеличения цвета


struct Strips
{
  Adafruit_NeoPixel Strip;
  bool Enable;
  int32_t Phase;
  int32_t Pause;
  int32_t Hue;
};

Strips Led[COUNT_STRIP]; // ленты


void setup()
{
  // В случае управление питанием через ключ нужен этот кусок кода, чтобы включить ленту
  // и дождаться окончания переходных процессов
  /*pinMode(POWER_PIN, OUTPUT);
  digitalprint(PIN_COLOR, HIGH);
  delay(2000);*/
  // инициализируем ленты
  // стартовая инициализация всех массивов
  int32_t Hue = GetHue();
  for (int i=0; i<COUNT_STRIP; i++)
  {
    Led[i] = { Adafruit_NeoPixel(COUNT_PIXELS, DATA_PINS[i], NEO_GRB + NEO_KHZ800), true, 0, 0, Hue };
    Led[i].Strip.begin();
  }
  // включение аналогового входа настройка диапазона цветов метеора
//  pinMode(PIN_COLOR, INPUT);
//  pinMode(PIN_DELAY, INPUT);
//  pinMode(PIN_RAND, INPUT_PULLUP);
}
 
 
void loop()
{
  for (int i=0; i<COUNT_STRIP; i++) // поочередно перебираем все ленты
  {
    if (Led[i].Enable) // если на ленте включён метеор
    {   
      Led[i].Strip.clear();
      if (Led[i].Phase > COUNT_PIXELS + LENGTH_METEORS) // если метеор вылетел то
      {
        Led[i].Enable = false; // выключение ленты  
        Led[i].Phase = 0; // сброс фазы ленты на НОЛЬ
        Led[i].Pause = random(10, 50); // задание паузы до следующего включения (в циклах)
        if (GetColor()) // если тумблер выключен
          Led[i].Hue = GetHue(); // то работаем с резистором цвета
        else
          Led[i].Hue += 128; // иначе плавно меняем цвет        
      }
      else // иначе засвечиваем Метеор на ленте
      {
        int Index = Led[i].Phase;
        int32_t Hue = Led[i].Hue;
        int Val = 255;
        while ((Index >= 0) && (Val >= 0))
        {
          if (Index < COUNT_PIXELS)
          {
            uint32_t Color = Led[i].Strip.ColorHSV(Hue, 255, Val);
            Led[i].Strip.setPixelColor(Index, Color);
          }
          Val -= INCREASE_BRIGHTNESS;
          Index--;
        }
        Led[i].Phase++; // сдвигаем фазу
      }
      Led[i].Strip.show(); // засветка ленты
    }
    else // если лента выключена
    {
      if (Led[i].Pause > 0)
        Led[i].Pause--; // то считаем длительность паузы
      else
        Led[i].Enable = true; // и по окончании включаем ленту снова
    }
  }
  // скорость метеора если число + то скорость -
  int StayTime = GetStay(); // снятие показателя задержки
  delay(StayTime); // задержка по уровню скорости МЕТЕОРА
}


bool GetColor()
{
  return digitalRead(PIN_RAND) == HIGH; // если тумблер выключен
}


int32_t GetHue()
{
  word Result = map(analogRead(PIN_COLOR), 0, 1023, 0, 65535); // в цветовом ряду HSV - hue описывается в диапазоне от 0 до 65535
  return random(Result - 1024, Result + 1024); // с коридором флюктуации цвета
}

int GetStay()
{
  return map(analogRead(PIN_DELAY), 0, 1023, 5, 95); // снятие показателя задержки
}
