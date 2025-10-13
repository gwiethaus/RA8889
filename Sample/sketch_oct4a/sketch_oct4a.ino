void setup() {
  Serial.begin(115200);
  while (!Serial);

  float valor = 0.12;
  char buffer[16];
  dtostrf(valor, 0, 2, buffer);  // largura automática, 2 casas decimais
  Serial.print("Buffer: ");
  Serial.println(buffer);  // deve imprimir "0.12"


  float valor1 = 0.12;
  char buffer1[32];
dtostrf((float)valor1, 0, 2, buffer1); // duas casas decimais
Serial.print("buffer 1 ");               // deve imprimir "0.12" por exemplo
Serial.println(buffer1);               // deve imprimir "0.12" por exemplo
  

  float valor2 = 0.22;
  char buffer2[32];
dtostrf(valor2, 0, 2, buffer2); // duas casas decimais
Serial.println("buffer 2: ");               // deve imprimir "0.12" por exemplo
Serial.println(buffer2);               // deve imprimir "0.12" por exemplo
}

void loop() {}