// https://alexgyver.ru/lessons/parsing/

struct myStruct {
  float val_f;
  int val_i;
  long val_l;
  byte val_b;
};

myStruct buf;
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
}

void loop() {
 // заполняем
  buf.val_b = 123;
  buf.val_i = 12345;
  buf.val_l = 123456;
  buf.val_f = 123.456;

// Serial.println(564765);
// Serial.println (buf.val_i);
  // указываем ему буфер-структуру, но приводим тип к byte*
  // размер можно указать через sizeof()
  Serial.write((byte*)&buf, sizeof(buf));
  delay(2000);
}
