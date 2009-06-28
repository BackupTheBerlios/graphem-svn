#ifndef STROKE_H
#define STROKE_H

//currently stores an improved version of Freeman chain codes
class Stroke {
public:
	Stroke(unsigned short direction, int duration, double length, bool up = false) :
		direction(direction),
		duration(duration),
		length(length),
		//pressure(pressure),
		up(up),
		removed(false)
	{ }
	double weight() const { return length; }

	short direction; // in [0,7]; start at 0 for a stroke in x-direction, continue counter-clockwise
	int duration; //in msecs
	double length; 
	//double pressure; //how to do this right?
	bool up; //no actual stroke, just moving the pen
	bool removed;
};
#endif
