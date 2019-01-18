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

#endif /* SETPINS_H */
