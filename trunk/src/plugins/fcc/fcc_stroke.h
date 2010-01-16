/*
    Graphem
    Copyright (C) 2009-2010 Christian Pulvermacher

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef FCC_STROKE_H
#define FCC_STROKE_H

#include <QLineF>

/*
stores an improved version of Freeman chain codes:
directions are converted to integers in 0..7
 ----x--->
 | 3 2 1
 y 4   0
 | 5 6 7
 V

*/

class Stroke {
public:
	Stroke(QLineF l, int start_node, bool up = false);
	double getWeight() const { return weight; }
	Stroke operator+=(const Stroke &s);

	int start_node_id;
	short direction;
	bool up; //no actual stroke, just moving the pen
	bool removed;
	double length; 
private:
	double weight;

	//weight factors
	const static double w_angle = 10;
	const static double w_length = 1;
};
#endif
