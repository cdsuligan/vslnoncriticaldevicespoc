#include "setpins.h"



void SetPins(struct ESPPins pin)
{	  
	if(pin.D3){
	  pinMode(D3, OUTPUT);
      digitalWrite(D3, true);}
    else{
      pinMode(D3, OUTPUT);
      digitalWrite(D3, false);}
	  
	if(pin.D4){
	  pinMode(D4, OUTPUT);
      digitalWrite(D4, true);}
    else{
      pinMode(D4, OUTPUT);
      digitalWrite(D4, false);}
	  
	if(pin.D5){
	  pinMode(D5, OUTPUT);
      digitalWrite(D5, true);}
    else{
      pinMode(D5, OUTPUT);
      digitalWrite(D5, false);}
    if(pin.A6 != 0){
      pinMode(D6, OUTPUT);
      analogWrite(D6, pin.A6);}
    else{
      pinMode(D6, OUTPUT);
      analogWrite(D6, pin.A6);}
}
