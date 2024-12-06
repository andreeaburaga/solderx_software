float mean;
void calculateTemperature() {
  static int filterValue = 0;
  float analogReadValue = abs(analogRead(TEMP_OUT));
  if(filterValue == 0) filterValue = analogReadValue;
  
  mean = 0.9*filterValue + 0.1*analogReadValue;
  int U = (mean * 3300.0) / 1023.0; // Convert analog reading to millivolts
  double a0 = -54.1010714;
  double a1 = 0.507906896;
  double a2 = -0.00020687128;
  double a3 = 5.12766721e-08;
  
  // Compute the temperature T(U)
  float T1 = a0 + a1 * U + a2 * pow(U, 2) + a3 * pow(U, 3);
  //float T2 = 0.63*U + 10;
  
  
  filterValue = mean;
  currentTemperature = T1;
  if(currentTemperature < 0) currentTemperature = 0;
}
