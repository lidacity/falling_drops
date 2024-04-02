# Led-гирлянда Падающие капли

Альтернативные названия:

* Падающие метеориты
* Сосульки
* Падающий дождь

и прочие.
Официальное название неизвестно.


Данный проект содержит модернизированный код проекта [Метеорит](http://www.getchip.net/posts/117-12-ti-kanalnyjj-meteor-na-svetodiodnykh-lentakh-ws2812b-arduino/)

# Изменения

* переход работы адресной ленты WS2812b от [FastLED](https://github.com/FastLED/FastLED/) к [NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel): появилась поддержка создавать лучи, при этом брать пины из массива, плюс огромная экономия памяти, так как FastLED избыточен для такого простого проекта
* причесал код (в оригинальном коде совсем непонятные работы с излишними массивам, нет динамического )
* вынес дополнительные пины в #define в начале кода, работа с дополнительными пинами происходит в отдельных функциях
* и да - мой код так же не идеален

Код по эффектам полностью совпадает с оригинальным проектом.

Тестировал и отлаживал на [Wokwi](https://wokwi.com/projects/392864539998877697), а так же Wemos D1 mini, но будет работать и на Arduino Pro mini.
Есть возможность увеличить количество лучей (при использовании расширителя портов ввода-вывода, например PCF8574).
Изначальное количество светодиодов в ленте мне показалось малым, хороший визуальный эффект возникает при размере 32 светодиода и длиной хвоста 24 пикселя (с расчётом на 2к оперативной памяти).

# Возможности

(4 потенциометра с выключателем, 2 выключателя)

* задать тон метеора, или увеличение тона по кругу HSV
* задать скорость движения метеора, или случайная скорость каждого метеора
* задать длина метеора, или случайная длина каждого метеора
* задать задержку между вылетами метеора, или случайная задержка между вылетами каждого метеора
* задать внешний вид метеора
* задать направление вылета метеора
* или отключить в коде все возможности

# Источник

* [117-12-ти канальный «Метеор» на светодиодных лентах WS2812B и Arduino](http://www.getchip.net/posts/117-12-ti-kanalnyjj-meteor-na-svetodiodnykh-lentakh-ws2812b-arduino/)
* [Модификации «Метеорита»](http://www.forum.getchip.net/viewtopic.php?f=24&t=610)

