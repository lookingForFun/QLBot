/*
	Copyright (C) 1999-2006 Id Software, Inc. and contributors.
	For a list of contributors, see the accompanying CONTRIBUTORS file.

	This file is part of GtkRadiant.

	GtkRadiant is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	GtkRadiant is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with GtkRadiant; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "../Common/Math.h"
#include "../Common/Vector.h"
#include "../Common/Angles.h"
#include "../Common/Matrix.h"
#include <float.h>

idAngles ang_zero( 0.0f, 0.0f, 0.0f );


/*
=================
idAngles::Normalize360

returns angles normalized to the range [0 <= angle < 360]
=================
*/
idAngles& idAngles::Normalize360( void ) {
	int i;

	for ( i = 0; i < 3; i++ ) {
		if ( ( (*this)[i] >= 360.0f ) || ( (*this)[i] < 0.0f ) ) {
			(*this)[i] -= floor( (*this)[i] / 360.0f ) * 360.0f;

			if ( (*this)[i] >= 360.0f ) {
				(*this)[i] -= 360.0f;
			}
			if ( (*this)[i] < 0.0f ) {
				(*this)[i] += 360.0f;
			}
		}
	}

	return *this;
}

/*
=================
idAngles::Normalize180

returns angles normalized to the range [-180 < angle <= 180]
=================
*/
idAngles& idAngles::Normalize180( void ) {
	Normalize360();

	if ( pitch > 180.0f ) {
		pitch -= 360.0f;
	}
	
	if ( yaw > 180.0f ) {
		yaw -= 360.0f;
	}

	if ( roll > 180.0f ) {
		roll -= 360.0f;
	}
	return *this;
}

/*
=================
idAngles::ToVectors
=================
*/
void idAngles::ToVectors( idVec3 *forward, idVec3 *right, idVec3 *up ) const {
	float sr, sp, sy, cr, cp, cy;
	
	idMath::SinCos( DEG2RAD( yaw ), sy, cy );
	idMath::SinCos( DEG2RAD( pitch ), sp, cp );
	idMath::SinCos( DEG2RAD( roll ), sr, cr );
	if ( forward ) {
		forward->Set( cp * cy, cp * sy, -sp );
	}

	if ( right ) {
		right->Set( -sr * sp * cy + cr * sy, -sr * sp * sy + -cr * cy, -sr * cp );
	}

	if ( up ) {
		up->Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );
	}
}

/*
=================
idAngles::ToForward
=================
*/
idVec3 idAngles::ToForward( void ) const {
	float sp, sy, cp, cy;
	
	idMath::SinCos( DEG2RAD( yaw ), sy, cy );
	idMath::SinCos( DEG2RAD( pitch ), sp, cp );

	return idVec3( cp * cy, cp * sy, -sp );
}

/*
=================
idAngles::ToMat3
=================
*/
idMat3 idAngles::ToMat3( void ) const {
	idMat3 mat;
	float sr, sp, sy, cr, cp, cy;

	idMath::SinCos( DEG2RAD( yaw ), sy, cy );
	idMath::SinCos( DEG2RAD( pitch ), sp, cp );
	idMath::SinCos( DEG2RAD( roll ), sr, cr );

	mat[ 0 ].Set( cp * cy, cp * sy, -sp );
	mat[ 1 ].Set( sr * sp * cy + cr * -sy, sr * sp * sy + cr * cy, sr * cp );
	mat[ 2 ].Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );

	return mat;
}

// RAVEN BEGIN
// jscott: slightly quicker version without the copy
idMat3 &idAngles::ToMat3( idMat3 &mat ) const {
	float	sr, sp, sy, cr, cp, cy;
		
	idMath::SinCos( DEG2RAD( yaw ), sy, cy );
	idMath::SinCos( DEG2RAD( pitch ), sp, cp );
	idMath::SinCos( DEG2RAD( roll ), sr, cr );

	mat[0].Set( cp * cy, cp * sy, -sp );
	mat[1].Set( sr * sp * cy + cr * -sy, sr * sp * sy + cr * cy, sr * cp );
	mat[2].Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );

	return mat;
}
// RAVEN END


/*
=============
idAngles::ToString
=============
*/
const char *idAngles::ToString( int precision ) const {
	return nullptr;
}
