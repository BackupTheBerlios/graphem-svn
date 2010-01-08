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

#ifndef NO_QCA
#include <QtCrypto>
#endif

#include <QtGlobal>

#include <climits>


QByteArray Crypto::generateSalt()
{
	const int salt_bytes_count = 5;

#ifndef NO_QCA
	return QCA::Random::randomArray(salt_bytes_count).toByteArray();
#else
	QByteArray salt("");
	for(int i = 0; i < salt_bytes_count; i++)
		salt.append(char(randInt(0, 255)));
	return salt;
#endif
}


int Crypto::randInt(int min, int max)
{
	//in [0, UINT_MAX]
#ifndef NO_QCA
	const unsigned int r = QCA::Random::randomInt();
#else
	const unsigned int 	r = qrand();
#endif

	//in [0, 1]
	double x = double(r)/UINT_MAX;

	//uniform in [min-0.5, max+0.5], then rounded
	const int result = qRound(min-0.5 + (max-min+1)*x);
	Q_ASSERT(result >= min);
	Q_ASSERT(result <= max);
	return result;
}
