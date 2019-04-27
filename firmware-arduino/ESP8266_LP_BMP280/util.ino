double temperature, pressure;

void gpio_init()
{
  pinMode(2, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  led_none();
  Serial.begin(115200);
  
}

void led_red()
{
  digitalWrite(2, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(14, HIGH);
}

void led_green()
{
  digitalWrite(2, HIGH);
  digitalWrite(13, HIGH);
  digitalWrite(14, LOW);
}

void led_blue()
{
  digitalWrite(2, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(14, HIGH);
}

void led_none()
{
  digitalWrite(2, HIGH);
  digitalWrite(13, HIGH);
  digitalWrite(14, HIGH);
}

bool startMeasurement()
{
  uint8_t result = bmp.startMeasurment();
  if (result != 0)
  {
    delay(result);
    result = bmp.getTemperatureAndPressure(temperature, pressure);
    if (result != 0)
      return 1;
    else
      return 0;
  }
  else
    return 0;
}

float getTemperature()
{
  return (temperature);
}

float getHumidity()
{
  return random(0, 100);
}

uint16_t getPressure()
{
  return pressure;
}
