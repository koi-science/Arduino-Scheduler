/**
 * @file SchedulerDemo.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2015
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * @section Description
 * This Arduino sketch shows how to use the Scheduler library.
 * Two threads are scheduled together with the main thread.
 * 1. The main thread prints an iteration number and delays.
 * 2. The second thread blinks the built-in LED and prints the
 *    current status.
 * 3. The third thread reads a line from Serial input. The line is
 *    printed with number of yields during the input wait.
 */

#include <Scheduler.h>

void setup()
{
  Serial.begin(57600);
  Serial.println(F("SchedulerDemo: started"));

  void* buf = malloc(64);
  Serial.print(millis());
  Serial.print(F(":setup:alloc:buf=0x"));
  Serial.println((int) buf, HEX);
  free(buf);

  Scheduler.begin();
  setup2();
  Scheduler.start(loop2);
  setup3();
  Scheduler.start(loop3);
}

void loop()
{
  static int i = 0;
  Serial.print(millis());
  Serial.print(F(":loop::i="));
  Serial.println(i++);
  delay(500);

  void* buf = malloc(64);
  Serial.print(millis());
  Serial.print(F(":loop:alloc:buf=0x"));
  Serial.println((int) buf, HEX);
  delay(500);
  Serial.print(millis());
  Serial.print(F(":loop:free:buf=0x"));
  Serial.println((int) buf, HEX);
  free(buf);
  buf = NULL;
}

const int LED = 13;

void setup2()
{
  Serial.print(millis());
  Serial.println(F(":setup2"));
  pinMode(LED, OUTPUT);
}

void loop2()
{
  Serial.print(millis());
  Serial.println(F(":loop2::led off"));
  digitalWrite(LED, LOW);
  delay(1000);
  Serial.print(millis());
  Serial.println(F(":loop2::led on"));
  digitalWrite(LED, HIGH);
  delay(1000);
}

void setup3()
{
  Serial.print(millis());
  Serial.println(F(":setup3"));
}

void loop3()
{
  static char* buf = NULL;
  if (buf == NULL) {
    buf = (char*) malloc(64);
    Serial.print(millis());
    Serial.print(F(":loop3:alloc:buf=0x"));
    Serial.println((int) buf, HEX);
    if (buf == NULL) {
      delay(10);
      return;
    }
  }
  char* bp = buf;
  unsigned long nr = 0;
  int c;
  while ((c = Serial.read()) != '\n') {
    if (c > 0)
      *bp++ = c;
    else {
      nr += 1;
      yield();
    }
  }
  *bp = 0;
  Serial.print(millis());
  Serial.print(F(":loop3:nr="));
  Serial.print(nr);
  Serial.print(F(",buf="));
  Serial.println(buf);
  if (!strcmp_P(buf, (const char*) F("free"))) {
    Serial.print(millis());
    Serial.print(F(":loop3:free:buf=0x"));
    Serial.println((int) buf, HEX);
    free(buf);
    delay(500);
    buf = NULL;
  }
}

