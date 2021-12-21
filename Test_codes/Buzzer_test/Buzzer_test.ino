const int ledChannel = 0;
const int resolution = 8;
const int buzPin = 23;
const int duty = 18;

int sVal;
int temp=0;
int nFrq[]={0, 277, 294, 311, 330, 349, 370, 392, 415,440, 466,  494};

void playNote(int note, int dur)
{
  ledcSetup(ledChannel, nFrq[note], resolution);
  ledcWrite(ledChannel, duty);
  delay(dur);
}

void setup()
{
  Serial.begin(115200);
  ledcAttachPin(buzPin, ledChannel);
}

void loop()
{
  if(Serial.available()>0)
  {
    sVal = Serial.read();
    Serial.println("sVal: " + sVal-0x30);
    temp = sVal-0x30;
  }
  if(temp==1)
    {
      for(int i=0; i<4; i++)
      {
        playNote(9, 250);
        playNote(5, 250);
        ledcSetup(ledChannel, 0, resolution);
        delay(250);
      }
    }
}
