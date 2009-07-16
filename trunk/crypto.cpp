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

#include "crypto.h"

#include <QtCrypto>

QByteArray Crypto::generateSalt()
{
	const int salt_bytes_count = 5;
	return QCA::Random::randomArray(salt_bytes_count).toByteArray();
}

int Crypto::randInt(int min, int max)
{
	//in [0, UINT_MAX]
	const unsigned int r = QCA::Random::randomInt();
	//in [0, 1]
	double x = double(r)/UINT_MAX;

	//TODO not exactly uniform yet
	const double result = min + (max-min)*x;
	Q_ASSERT(result >= min);
	Q_ASSERT(result <= max);
	return result;
}
