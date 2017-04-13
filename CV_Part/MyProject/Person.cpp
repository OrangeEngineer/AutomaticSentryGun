#include "Person.h"



Person::Person()
{
	//set values for default constructor
	setType("null");
	setColour(Scalar(0,0,0));

}

Person::Person(string name){

	setType(name);
	
	if(name=="enemy"){

		//TODO: use "calibration mode" to find HSV min
		//and HSV max values

		setHSVmin(Scalar(0,200,135));
		setHSVmax(Scalar(30,255,165));

		//BGR value for Red:
		setColour(Scalar(0,0,255));

	}
	if(name=="hostage"){

		//TODO: use "calibration mode" to find HSV min
		//and HSV max values

		setHSVmin(Scalar(95,90,110));
		setHSVmax(Scalar(105,170,255));

		//BGR value for Green:
		setColour(Scalar(0,255,0));

	}

}

Person::~Person(void)
{
}

int Person::getXPos(){

	return Person::xPos;

}

void Person::setXPos(int x){

	Person::xPos = x;

}

int Person::getYPos(){

	return Person::yPos;

}

void Person::setYPos(int y){

	Person::yPos = y;

}

Scalar Person::getHSVmin(){

	return Person::HSVmin;

}
Scalar Person::getHSVmax(){

	return Person::HSVmax;
}

void Person::setHSVmin(Scalar min){

	Person::HSVmin = min;
}


void Person::setHSVmax(Scalar max){

	Person::HSVmax = max;
}
