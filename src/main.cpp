/*
Copyright © 2020 Dmytro Korniienko (kDn)
JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov

    This file is part of FireLamp_JeeUI.

    FireLamp_JeeUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireLamp_JeeUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireLamp_JeeUI.  If not, see <https://www.gnu.org/licenses/>.

  (Этот файл — часть FireLamp_JeeUI.

   FireLamp_JeeUI - свободная программа: вы можете перераспространять ее и/или
   изменять ее на условиях Стандартной общественной лицензии GNU в том виде,
   в каком она была опубликована Фондом свободного программного обеспечения;
   либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
   версии.

   FireLamp_JeeUI распространяется в надежде, что она будет полезной,
   но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
   или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной
   общественной лицензии GNU.

   Вы должны были получить копию Стандартной общественной лицензии GNU
   вместе с этой программой. Если это не так, см.
   <https://www.gnu.org/licenses/>.)
*/


#include <Arduino.h>
#include "JeeUI2.h"
#include "config.h"
#include "lamp.h"
#include "main.h"
#ifdef LAMP_DEBUG
#include "ftpServer.h"
#endif

// глобальные переменные для работы с ними в программе
int mqtt_int; // интервал отправки данных по MQTT в секундах 
jeeui2 jee; // Создаем объект класса для работы с JeeUI2 фреймворком
LAMP myLamp;
#ifdef ESP_USE_BUTTON
GButton touch(BTN_PIN, PULL_MODE, NORM_OPEN);               
#endif

void setup() {
    Serial.begin(115200);

    //jee.mqtt("m21.cloudmqtt.com", 15486, "iukuegvk", "gwo8tlzvGJrR", mqttCallback, true);
    jee.mqtt(F(""), 15486, F("iukuegvk"), F("gwo8tlzvGJrR"), mqttCallback, true);
    jee.udp(jee.mc); // Ответ на UDP запрс. в качестве аргуиена - переменная, содержащая id по умолчанию
    jee.led(2, false); // назначаем пин на светодиод, который нам будет говорит о состоянии устройства. (быстро мигает - пытается подключиться к точке доступа, просто горит (или не горит) - подключен к точке доступа, мигает нормально - запущена своя точка доступа)
    jee.ap(20000); // если в течении 20 секунд не удастся подключиться к Точке доступа - запускаем свою (параметр "wifi" сменится с AP на STA)
    parameters(); // создаем параметры

    myLamp.effects.loadConfig();

    jee.ui(interface); // обратный вызов - интерфейс
    jee.update(update); // обратный вызов - вызывается при введении данных в веб интерфейс, нужна для сравнения значений пременных с параметрами
#ifdef LAMP_DEBUG
    jee.begin(true); // Инициализируем JeeUI2 фреймворк. Параметр bool определяет, показывать ли логи работы JeeUI2 (дебаг)
#else
    jee.begin(false); // Инициализируем JeeUI2 фреймворк. Параметр bool определяет, показывать ли логи работы JeeUI2 (дебаг)
#endif
    //update(); // 'этой функцией получаем значения параметров в переменные
#ifdef USE_FTP
    ftp_setup(); // запуск ftp-сервера
#endif
}

void loop() {
    myLamp.handle(); // цикл, обработка лампы

    jee.handle(); // цикл, необходимый фреймворку
    
    sendData(); // цикл отправки данных по MQTT
#ifdef USE_FTP
    ftp_loop(); // цикл обработки событий фтп-сервера
#endif
    ESP.wdtFeed(); // пнуть собаку
}

void mqttCallback(String topic, String payload){ // функция вызывается, когда приходят данные MQTT
  LOG.printf_P(PSTR("Message [%s - %s]"), topic.c_str() , payload.c_str());

}

void sendData(){
  static unsigned long i;
  static unsigned int in;
  if(i + (in * 1000) > millis() || mqtt_int == 0) return; // если не пришло время, или интервал = 0 - выходим из функции
  i = millis();
  in = mqtt_int;
  // всё, что ниже будет выполняться через интервалы

  LOG.println(F("MQTT send data"));
  //jee.publish("test","30");
  //jee.publish("hum", String(hum));
  //jee.publish("ds18b20", String(ds_tem));
}