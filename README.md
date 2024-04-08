# Led-гирлянда Падающие капли

Альтернативные названия:

* Падающие метеориты
* Сосульки
* Капающий дождь
* Тающие сосульки

и прочие.

Официальное название неизвестно.


# Технические данные

* работа на адресной ленте WS2812b на [NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
* дополнительные пины в #define в начале кода для управления отображением метеоров
* количество лент 12 шт (на Arduino Nano можно переделать на 19 шт)
* длина одной ленты 32 пикселя (ограничена размером памяти: при оперативной памяти 2к - максимальная длина ленты около 500 пикселей, и от количества лент не зависит)
* при большом количестве пикселей (>500) быстродействие зависит только от скорости процессора микроконтроллера
* эффект применяется не сразу, а на следующий вылет каждого метеора

Тестировал и отлаживал на [Wokwi](https://wokwi.com/projects/392864539998877697), а так же на lgt8f328p (10 лент на 120 пикселей каждая), на Wemos D1 mini (2 ленты по 32 и по 300), будет работать и на Arduino Pro mini.


# Возможности

(4 потенциометра с выключателем, 2 выключателя)

* задать тон метеора, или увеличение тона по кругу HSV
* задать скорость движения метеора, или случайная скорость каждого метеора
* задать длина метеора, или случайная длина каждого метеора
* задать задержку между вылетами метеора, или случайная задержка между вылетами каждого метеора
* задать внешний вид метеора
* задать направление вылета метеора


# Аналогичные проекты

* [117-12-ти канальный «Метеор» на светодиодных лентах WS2812B и Arduino](http://www.getchip.net/posts/117-12-ti-kanalnyjj-meteor-na-svetodiodnykh-lentakh-ws2812b-arduino/)
* [Модификации «Метеорита»](http://www.forum.getchip.net/viewtopic.php?f=24&t=610)

