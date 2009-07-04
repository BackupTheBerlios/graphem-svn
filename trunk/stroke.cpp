/*
    Graphem
    Copyright (C) 2009 Christian Pulvermacher

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

#include "stroke.h"

#include <cmath>

#include <QLineF>


Stroke::Stroke(QLineF l, bool up):
	up(up),
	removed(false),
	length(l.length())
{
	//"inspired" by QLineF::angle() (Qt 4.4)
	const qreal theta = atan2(-l.dy(), l.dx()) * 180.0 / 3.141;
	double angle = theta < 0 ? theta + 360 : theta;

	if(angle < 0)
		angle = 360-angle;
	direction = qRound(angle/360*8) % 8;

	weight = (1- (angle/360*8 - qRound(angle/360*8))) * w_angle;
	weight += length * w_length;
}

//add another stroke
Stroke Stroke::operator+=(const Stroke &s)
{
	Q_ASSERT(direction == s.direction);

	length += s.length;
	weight += s.weight;

	return *this;
}
