#ifndef CURRENTMONITOR_H
#define CURRENTMONITOR_H

#include <Arduino.h>

#define samplePeriod 20

double calcIrms()
{
  double filteredI, sumI, Irms, sampleI, lastLargeSampleI, lastSmallSampleI, offsetI;

  lastSmallSampleI = analogRead(A0);

  for (unsigned int n = 0; n < samplePeriod; n++)
  {
    sampleI = analogRead(A0);
  	if(sampleI > lastLargeSampleI)
    	{
    		lastLargeSampleI = sampleI;
    	}
    if(sampleI < lastSmallSampleI)
      {
        lastSmallSampleI = sampleI;
      } 
     //yield();
     delay(1);
  }
  offsetI = (((lastLargeSampleI - lastSmallSampleI)/2) + lastSmallSampleI);
  filteredI = lastLargeSampleI - offsetI;
  
  double Ipk = filteredI * (3.3/1024);
  Irms = Ipk * 0.707; //0.707 =  1/(sqrt(2) 

  return Irms;
}

#endif /* CURRENTMONITOR_H */
