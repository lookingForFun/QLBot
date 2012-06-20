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

#ifndef __MATH_ANGLES_H__
#define __MATH_ANGLES_H__

/*
===============================================================================

	Euler angles

===============================================================================
*/

// angle indexes
#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2		// fall over

#ifndef M_PI
#  define M_PI (3.1415926536f)
#endif

class idVec3;
class idQuat;
class idRotation;
class idMat3;
class idMat4;

class idAngles {
public:
	float			pitch;
	float			yaw;
	float			roll;

					idAngles( void );
					idAngles( float pitch, float yaw, float roll );
					explicit idAngles( const idVec3 &v );

	void 			Set( float pitch, float yaw, float roll );
	idAngles &		Zero( void );

	float			operator[]( int index ) const;
	float &			operator[]( int index );
	idAngles		operator-() const;			// negate angles, in general not the inverse rotation
	idAngles &		operator=( const idAngles &a );
	idAngles		operator+( const idAngles &a ) const;
	idAngles &		operator+=( const idAngles &a );
	idAngles		operator-( const idAngles &a ) const;
	idAngles &		operator-=( const idAngles &a );
	idAngles		operator*( const float a ) const;
	idAngles &		operator*=( const float a );
	idAngles		operator/( const float a ) const;
	idAngles &		operator/=( const float a );

	friend idAngles	operator*( const float a, const idAngles &b );

	bool			Compare( const idAngles &a ) const;							// exact compare, no epsilon
	bool			Compare( const idAngles &a, const float epsilon ) const;	// compare with epsilon
	bool			operator==(	const idAngles &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const idAngles &a ) const;						// exact compare, no epsilon

	idAngles &		Normalize360( void );	// normalizes 'this'
	idAngles &		Normalize180( void );	// normalizes 'this'

	float			Length( void ) const;
	float			LengthSqr( void ) const;
	void			Clamp( const idAngles &min, const idAngles &max );

	int				GetDimension( void ) const;

	void			ToVectors( idVec3 *forward, idVec3 *right = nullptr, idVec3 *up = nullptr ) const;
	idVec3			ToForward( void ) const;
	idQuat			ToQuat( void ) const;
	idRotation		ToRotation( void ) const;
	idMat3			ToMat3( void ) const;

// RAVEN BEGIN
	idMat3			&ToMat3( idMat3 &mat ) const;
// abahr
	idAngles&		Scale( const idAngles& scalar );
	idAngles&		Remap( const int map[], const int dirMap[] );
// RAVEN END

	idMat4			ToMat4( void ) const;
	idVec3			ToAngularVelocity( void ) const;
	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	const char *	ToString( int precision = 2 ) const;

	bool			FixDenormals( void );
};

extern idAngles ang_zero;

__forceinline idAngles::idAngles( void ) {
}

__forceinline idAngles::idAngles( float pitch, float yaw, float roll ) {
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

__forceinline idAngles::idAngles( const idVec3 &v ) {
	this->pitch = v[0];
	this->yaw	= v[1];
	this->roll	= v[2];
}

__forceinline void idAngles::Set( float pitch, float yaw, float roll ) {
	this->pitch = pitch;
	this->yaw	= yaw;
	this->roll	= roll;
}

__forceinline idAngles &idAngles::Zero( void ) {
	pitch = yaw = roll = 0.0f;
	return *this;
}

__forceinline float idAngles::operator[]( int index ) const {
	assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &pitch )[ index ];
}

__forceinline float &idAngles::operator[]( int index ) {
	assert( ( index >= 0 ) && ( index < 3 ) );
	return ( &pitch )[ index ];
}

__forceinline idAngles idAngles::operator-() const {
	return idAngles( -pitch, -yaw, -roll );
}

__forceinline idAngles &idAngles::operator=( const idAngles &a ) {
	pitch	= a.pitch;
	yaw		= a.yaw;
	roll	= a.roll;
	return *this;
}

__forceinline idAngles idAngles::operator+( const idAngles &a ) const {
	return idAngles( pitch + a.pitch, yaw + a.yaw, roll + a.roll );
}

__forceinline idAngles& idAngles::operator+=( const idAngles &a ) {
	pitch	+= a.pitch;
	yaw		+= a.yaw;
	roll	+= a.roll;

	return *this;
}

__forceinline idAngles idAngles::operator-( const idAngles &a ) const {
	return idAngles( pitch - a.pitch, yaw - a.yaw, roll - a.roll );
}

__forceinline idAngles& idAngles::operator-=( const idAngles &a ) {
	pitch	-= a.pitch;
	yaw		-= a.yaw;
	roll	-= a.roll;

	return *this;
}

__forceinline idAngles idAngles::operator*( const float a ) const {
	return idAngles( pitch * a, yaw * a, roll * a );
}

__forceinline idAngles& idAngles::operator*=( float a ) {
	pitch	*= a;
	yaw		*= a;
	roll	*= a;
	return *this;
}

__forceinline idAngles idAngles::operator/( const float a ) const {
	float inva = 1.0f / a;
	return idAngles( pitch * inva, yaw * inva, roll * inva );
}

__forceinline idAngles& idAngles::operator/=( float a ) {
	float inva = 1.0f / a;
	pitch	*= inva;
	yaw		*= inva;
	roll	*= inva;
	return *this;
}

__forceinline idAngles operator*( const float a, const idAngles &b ) {
	return idAngles( a * b.pitch, a * b.yaw, a * b.roll );
}

__forceinline bool idAngles::Compare( const idAngles &a ) const {
	return ( ( a.pitch == pitch ) && ( a.yaw == yaw ) && ( a.roll == roll ) );
}

__forceinline bool idAngles::Compare( const idAngles &a, const float epsilon ) const {
	if ( idMath::Fabs( pitch - a.pitch ) > epsilon ) {
		return false;
	}
			
	if ( idMath::Fabs( yaw - a.yaw ) > epsilon ) {
		return false;
	}

	if ( idMath::Fabs( roll - a.roll ) > epsilon ) {
		return false;
	}

	return true;
}

__forceinline bool idAngles::operator==( const idAngles &a ) const {
	return Compare( a );
}

__forceinline bool idAngles::operator!=( const idAngles &a ) const {
	return !Compare( a );
}

__forceinline float idAngles::Length( void ) const {
	return idMath::Sqrt( yaw * yaw + pitch * pitch + roll * roll );
}

__forceinline float idAngles::LengthSqr( void ) const {
	return ( yaw * yaw + pitch * pitch + roll * roll );
}

__forceinline void idAngles::Clamp( const idAngles &min, const idAngles &max ) {
	if ( pitch < min.pitch ) {
		pitch = min.pitch;
	} else if ( pitch > max.pitch ) {
		pitch = max.pitch;
	}
	if ( yaw < min.yaw ) {
		yaw = min.yaw;
	} else if ( yaw > max.yaw ) {
		yaw = max.yaw;
	}
	if ( roll < min.roll ) {
		roll = min.roll;
	} else if ( roll > max.roll ) {
		roll = max.roll;
	}
}

__forceinline int idAngles::GetDimension( void ) const {
	return 3;
}

__forceinline const float *idAngles::ToFloatPtr( void ) const {
	return &pitch;
}

__forceinline float *idAngles::ToFloatPtr( void ) {
	return &pitch;
}

// RAVEN BEGIN
__forceinline idAngles& idAngles::Scale( const idAngles& scalar ) {
	for( int ix = 0; ix < GetDimension(); ++ix ) {
		(*this)[ix] *= scalar[ix];
	}

	return *this;
}

__forceinline idAngles& idAngles::Remap( const int map[], const int dirMap[] ) {
	idAngles ang( *this );
	for( int ix = 0; ix < GetDimension(); ++ix ) {
		(*this)[ ix ] = SignZero(dirMap[ix]) * ang[ abs(map[ix]) ];
	}

	return *this;
}
// RAVEN END

__forceinline bool idAngles::FixDenormals( void ) {
	bool denormal = false;
	if ( fabs( yaw ) < 1e-30f ) {
		yaw = 0.0f;
		denormal = true;
	}
	if ( fabs( pitch ) < 1e-30f ) {
		pitch = 0.0f;
		denormal = true;
	}
	if ( fabs( roll ) < 1e-30f ) {
		roll = 0.0f;
		denormal = true;
	}
	return denormal;

}

#endif /* !__MATH_ANGLES_H__ */
