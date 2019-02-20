#ifndef SETPINS_H
#define SETPINS_H

#include <Arduino.h>

struct ESPPins {
	bool D3;
	bool D4;
	bool D5;
	unsigned int A6;
};


void SetPins(struct ESPPins pin);

struct PinStatus {
  bool D0Status;
  bool D1Status;
  bool D2Status;
  bool A0Status;
};

#endif /* SETPINS_H */
