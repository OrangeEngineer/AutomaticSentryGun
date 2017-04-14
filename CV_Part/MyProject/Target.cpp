#include "Target.h"



Target::Target(){
	setType("null");
	setColour(Scalar(0,0,0));
}

Target::Target(string name){
	setType(name);
	if(name=="orange can"){
		setHSVmin(Scalar(0,200,135));
		setHSVmax(Scalar(30,255,255));
		setColour(Scalar(0,255,0));
	}
	if(name=="blue can"){
		setHSVmin(Scalar(95,90,110));
		setHSVmax(Scalar(105,170,255));
		setColour(Scalar(0,255,0));
	}

}

Target::~Target(void)
{
}

int Target::getXPos(){
	return Target::xPos;
}

void Target::setXPos(int x){
	Target::xPos = x;
}

int Target::getYPos(){
	return Target::yPos;
}

void Target::setYPos(int y){
	Target::yPos = y;
}

Scalar Target::getHSVmin(){
	return Target::HSVmin;
}

Scalar Target::getHSVmax(){
	return Target::HSVmax;
}

void Target::setHSVmin(Scalar min){
	Target::HSVmin = min;
}


void Target::setHSVmax(Scalar max){
	Target::HSVmax = max;
}
