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


#ifndef __MATH_VECTOR_H__
#define __MATH_VECTOR_H__
#include <memory.h>
#define nullptr 0
#define ALIGN16( x )					__declspec(align(16)) x

/*
===============================================================================

  Vector classes

===============================================================================
*/

#define VECTOR_EPSILON		0.001f

class idAngles;
class idPolar3;
class idMat3;

// RAVEN BEGIN
class rvAngles;
// RAVEN END

//===============================================================
//
//	idVec2 - 2D vector
//
//===============================================================

class idVec2 {
public:
	float			x;
	float			y;

					idVec2( void );
					explicit idVec2( const float x, const float y );

	void 			Set( const float x, const float y );
	void			Zero( void );

	float			operator[]( int index ) const;
	float &			operator[]( int index );
	idVec2			operator-() const;
	float			operator*( const idVec2 &a ) const;
	idVec2			operator*( const float a ) const;
	idVec2			operator/( const float a ) const;
	idVec2			operator+( const idVec2 &a ) const;
	idVec2			operator-( const idVec2 &a ) const;
	idVec2 &		operator+=( const idVec2 &a );
	idVec2 &		operator-=( const idVec2 &a );
	idVec2 &		operator/=( const idVec2 &a );
	idVec2 &		operator/=( const float a );
	idVec2 &		operator*=( const float a );

	friend idVec2	operator*( const float a, const idVec2& b );

	bool			Compare( const idVec2 &a ) const;							// exact compare, no epsilon
	bool			Compare( const idVec2 &a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const idVec2 &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const idVec2 &a ) const;						// exact compare, no epsilon

// RAVEN BEGIN
// ddynerman: vector normalization operator
	idVec2 &		operator~( void );
// RAVEN END

	float			Length( void ) const;
	float			LengthFast( void ) const;
	float			LengthSqr( void ) const;
	float			Normalize( void );			// returns length
	float			NormalizeFast( void );		// returns length
	idVec2 &		Truncate( float length );	// cap length
	void			Clamp( const idVec2 &min, const idVec2 &max );
	void			Snap( void );				// snap to closest integer value
	void			SnapInt( void );			// snap towards integer (floor)

	int				GetDimension( void ) const;

	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	const char *	ToString( int precision = 2 ) const;

	void			Lerp( const idVec2 &v1, const idVec2 &v2, const float l );

// RAVEN BEGIN
// jscott: Ensures second element greater than first
	void			EnsureIncremental( void );
// RAVEN END
};

extern idVec2 vec2_origin;
#define vec2_zero vec2_origin

__forceinline idVec2::idVec2( void ) {
}

__forceinline idVec2::idVec2( const float x, const float y ) {
	this->x = x;
	this->y = y;
}

__forceinline void idVec2::Set( const float x, const float y ) {
	this->x = x;
	this->y = y;
}

__forceinline void idVec2::Zero( void ) {
	x = y = 0.0f;
}

__forceinline bool idVec2::Compare( const idVec2 &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) );
}

__forceinline bool idVec2::Compare( const idVec2 &a, const float epsilon ) const {
	if ( idMath::Fabs( x - a.x ) > epsilon ) {
		return false;
	}
			
	if ( idMath::Fabs( y - a.y ) > epsilon ) {
		return false;
	}

	return true;
}

__forceinline bool idVec2::operator==( const idVec2 &a ) const {
	return Compare( a );
}

__forceinline bool idVec2::operator!=( const idVec2 &a ) const {
	return !Compare( a );
}

__forceinline float idVec2::operator[]( int index ) const {
	return ( &x )[ index ];
}

__forceinline float& idVec2::operator[]( int index ) {
	return ( &x )[ index ];
}

// RAVEN BEGIN
// ddynerman: vector normalization
__forceinline idVec2& idVec2::operator~( void ) {
	Normalize();
	return *this;
}
// RAVEN END

__forceinline float idVec2::Length( void ) const {
	return ( float )idMath::Sqrt( x * x + y * y );
}

__forceinline float idVec2::LengthFast( void ) const {
	float sqrLength;

	sqrLength = x * x + y * y;
	return sqrLength * idMath::RSqrt( sqrLength );
}

__forceinline float idVec2::LengthSqr( void ) const {
	return ( x * x + y * y );
}

__forceinline float idVec2::Normalize( void ) {
	float sqrLength, invLength;

	sqrLength = x * x + y * y;
	invLength = idMath::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	return invLength * sqrLength;
}

__forceinline float idVec2::NormalizeFast( void ) {
	float lengthSqr, invLength;

	lengthSqr = x * x + y * y;
	invLength = idMath::RSqrt( lengthSqr );
	x *= invLength;
	y *= invLength;
	return invLength * lengthSqr;
}

__forceinline idVec2 &idVec2::Truncate( float length ) {
	float length2;
	float ilength;

	if ( !length ) {
		Zero();
	}
	else {
		length2 = LengthSqr();
		if ( length2 > length * length ) {
			ilength = length * idMath::InvSqrt( length2 );
			x *= ilength;
			y *= ilength;
		}
	}

	return *this;
}

__forceinline void idVec2::Clamp( const idVec2 &min, const idVec2 &max ) {
	if ( x < min.x ) {
		x = min.x;
	} else if ( x > max.x ) {
		x = max.x;
	}
	if ( y < min.y ) {
		y = min.y;
	} else if ( y > max.y ) {
		y = max.y;
	}
}

__forceinline void idVec2::Snap( void ) {
	x = floor( x + 0.5f );
	y = floor( y + 0.5f );
}

__forceinline void idVec2::SnapInt( void ) {
	x = float( int( x ) );
	y = float( int( y ) );
}

__forceinline idVec2 idVec2::operator-() const {
	return idVec2( -x, -y );
}
	
__forceinline idVec2 idVec2::operator-( const idVec2 &a ) const {
	return idVec2( x - a.x, y - a.y );
}

__forceinline float idVec2::operator*( const idVec2 &a ) const {
	return x * a.x + y * a.y;
}

__forceinline idVec2 idVec2::operator*( const float a ) const {
	return idVec2( x * a, y * a );
}

__forceinline idVec2 idVec2::operator/( const float a ) const {
	float inva = 1.0f / a;
	return idVec2( x * inva, y * inva );
}

__forceinline idVec2 operator*( const float a, const idVec2& b ) {
	return idVec2( b.x * a, b.y * a );
}

__forceinline idVec2 idVec2::operator+( const idVec2 &a ) const {
	return idVec2( x + a.x, y + a.y );
}

__forceinline idVec2 &idVec2::operator+=( const idVec2 &a ) {
	x += a.x;
	y += a.y;

	return *this;
}

__forceinline idVec2 &idVec2::operator/=( const idVec2 &a ) {
	x /= a.x;
	y /= a.y;

	return *this;
}

__forceinline idVec2 &idVec2::operator/=( const float a ) {
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;

	return *this;
}

__forceinline idVec2 &idVec2::operator-=( const idVec2 &a ) {
	x -= a.x;
	y -= a.y;

	return *this;
}

__forceinline idVec2 &idVec2::operator*=( const float a ) {
	x *= a;
	y *= a;

	return *this;
}

__forceinline int idVec2::GetDimension( void ) const {
	return 2;
}

__forceinline const float *idVec2::ToFloatPtr( void ) const {
	return &x;
}

__forceinline float *idVec2::ToFloatPtr( void ) {
	return &x;
}

// RAVEN BEGIN
// jscott: ensures the second element is greater than the first
__forceinline void idVec2::EnsureIncremental( void )
{
	float	temp;

	if( x < y )
	{
		return;
	}
	temp = x;
	x = y;
	y = temp;
}
// RAVEN END

//===============================================================
//
//	idVec3 - 3D vector
//
//===============================================================

class idVec3 {
public:	
	float			x;
	float			y;
	float			z;

					idVec3( void );
					idVec3( float ang[3] );
					explicit idVec3( const float x, const float y, const float z );

	void 			Set( const float x, const float y, const float z );
	void			Zero( void );

	float			operator[]( const int index ) const;
	float &			operator[]( const int index );
	idVec3			operator-() const;
	idVec3 &		operator=( const idVec3 &a );		// required because of a msvc 6 & 7 bug
// RAVEN BEGIN
// bdube: added vec2 equal
	idVec3 &		operator=( const idVec2 &a );
	idVec3 &		operator*=( const idVec3 &a );
// RAVEN END
	float			operator*( const idVec3 &a ) const;
	idVec3			operator*( const float a ) const;
	idVec3			operator/( const float a ) const;
	idVec3			operator+( const idVec3 &a ) const;
	idVec3			operator-( const idVec3 &a ) const;
	idVec3 &		operator+=( const idVec3 &a );
	idVec3 &		operator-=( const idVec3 &a );
	idVec3 &		operator/=( const idVec3 &a );
	idVec3 &		operator/=( const float a );
	idVec3 &		operator*=( const float a );

	friend idVec3	operator*( const float a, const idVec3& b );

	bool			Compare( const idVec3 &a ) const;							// exact compare, no epsilon
	bool			Compare( const idVec3 &a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const idVec3 &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const idVec3 &a ) const;						// exact compare, no epsilon

	bool			FixDegenerateNormal( void );	// fix degenerate axial cases
	bool			FixDenormals( void );			// change tiny numbers to zero

	idVec3			Cross( const idVec3 &a ) const;
	idVec3 &		Cross( const idVec3 &a, const idVec3 &b );

// RAVEN BEGIN
// ddynerman: vector normalization operator
	idVec3 &		operator~( void );
// RAVEN END

	float			Length( void ) const;
	float			LengthSqr( void ) const;
	float			LengthFast( void ) const;
	float			Normalize( void );				// returns length
	float			NormalizeFast( void );			// returns length
	idVec3 &		Truncate( float length );		// cap length
	void			Clamp( const idVec3 &min, const idVec3 &max );
	void			ClampMin( const float &minx, const float &miny, const float &minz );
	void			Snap( void );					// snap to closest integer value
	void			SnapInt( void );				// snap towards integer (floor)

	int				GetDimension( void ) const;

	float			ToYaw( void ) const;
	float			ToPitch( void ) const;
	idAngles		ToAngles( void ) const;
	idPolar3		ToPolar( void ) const;
	idMat3			ToMat3( void ) const;		// vector should be normalized
	const idVec2 &	ToVec2( void ) const;
	idVec2 &		ToVec2( void );
	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	const char *	ToString( int precision = 2 ) const;

// RAVEN BEGIN
// abahr: added axis so we can create matrix with non-x vector
	idMat3			ToMat3( int axis ) const;		// vector should be normalized
// abahr:
	idVec3			ToNormal() const { idVec3 v( *this ); v.Normalize(); return v; }
// RAVEN END

	void			NormalVectors( idVec3 &left, idVec3 &down ) const;	// vector should be normalized
	void			OrthogonalBasis( idVec3 &left, idVec3 &up ) const;

	void			ProjectOntoPlane( const idVec3 &normal, const float overBounce = 1.0f );
	bool			ProjectAlongPlane( const idVec3 &normal, const float epsilon, const float overBounce = 1.0f );
	void			ProjectSelfOntoSphere( const float radius );

// RAVEN BEGIN
	float			ProjectOntoVector(const idVec3 &U);
// RAVEN END


// RAVEN BEGIN
// cdr - Extremely useful Vector opterations for Computational Geometry
	////////////////////////////////////////////////////////////////////////////////////
	// Area Of The Parallel Pipid (2D)
	//
	// Given two more points, this function calculates the area of the parallel pipid
	// formed.
	//
	// Note: This function CAN return a negative "area" if (this) is above or right of
	// (A) and (B)...  We do not take the abs because the sign of the "area" is needed
	// for the left right test
	//
	//
	//               ___---( ... )
	//        (A)---/        /
	//        /             /
	//       /             /
	//      /             /
	//     /      ___---(B)
	//  (this)---/
	//
	////////////////////////////////////////////////////////////////////////////////////
	float			AreaParallelPipid(const idVec3 &A, const idVec3 &B) const	{
		return ((A.x*B.y - A.y*B.x) + (B.x*y - x*B.y) + (x*A.y - A.x*y));
	}

	////////////////////////////////////////////////////////////////////////////////////
	// The Left Right Test (2D)
	//
	// Given a line segment (Start->End) and a tolerance for *right on*, this function
	// evaluates which side the point is of the line.
	//
	//
	//
	//          (this)        ___---/(End)
	//                 ___---/
	//          ___---/
	//  (Start)/
	//  
	////////////////////////////////////////////////////////////////////////////////////
	bool			IsLeftOf(const idVec3 &Start, const idVec3 &End) const {
		return (AreaParallelPipid(Start, End)>0.0f);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Distance To Line Segment
	//
	// Given a line segment (Start->End) this function will return the distance of the
	// given point (this) to the segment.  You must also pass in a Scale argument which
	// will tell you:
	//  Scale (-INF, 0.0f) => (this) projected before (Start)
	//  Scale [0.0f, 1.0f] => (this) projected inside the segment (Start->End)
	//  Scale (1.0f, +INF) => (this) projected beyond (End)
	//
	//
	//
	//          (this)        ___---/(End)
	//                 ___---/
	//          ___---/
	//  (Start)/
	//  
	////////////////////////////////////////////////////////////////////////////////////
	float			DistToLineSeg(const idVec3 &Start, const idVec3 &End, float& Scale) const {
		static idVec3 U;
		static idVec3 V;
		static float ULen2;

		V		= (*this) - Start;				// Compute V
		U		= End	  - Start;				// Compute U
		ULen2	= U.LengthSqr();				// Normalize U
		Scale	= (V*U / ULen2);				// Find the scale of this vector on U
		if (Scale<0.0f)	{return Dist(Start);}	// If Negative Scale, Projected In Front Of Start
		if (Scale>1.0f)	{return Dist(End);}		// If Positive Scale, Projected In Beyond End
		return Dist(Start + (U*Scale));			// Otherwise, project U to new location and return dist
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Project On To Line Segment
	//
	// Given a line segment (Start->End) this function will project the point onto the
	// segment.
	//
	//          (this)        ___---(End)
	//			    \     ---/
	//          __ (Result)
	//  (Start)
	//  
	////////////////////////////////////////////////////////////////////////////////////
	bool			ProjectToLineSeg(const idVec3 &Start, const idVec3 &End) {
		static idVec3 U;
		static idVec3 V;
		static float ULen2;
		static float Scale;

		V		= (*this) - Start;				// Compute V
		U		= End	  - Start;				// Compute U
		ULen2	= U.LengthSqr();				// Normalize U
		Scale	= (V*U / ULen2);				// Find the scale of this vector on U

		if (Scale<0.0f) {
			(*this) = Start;					// If Negative Scale, Projected In Front Of Start
			return false;						// Off The End
		} else if (Scale>1.0f)	{
			(*this) = End;						// If Positive Scale, Projected In Beyond End
			return false;						// Off The End
		} else {			
			(*this) = Start + (U*Scale);		// Otherwise, project U to new location and return dist
		}
		return true;							// Perpendicular Intersection Is On The Segment
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Distance Squared (Much Faster than Dist)
	////////////////////////////////////////////////////////////////////////////////////
	float			Dist2(const idVec3& Pt) const
	{
		return ((Pt.x-x)*(Pt.x-x) + (Pt.y-y)*(Pt.y-y) + (Pt.z-z)*(Pt.z-z));
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Distance Squared OnlyXY
	////////////////////////////////////////////////////////////////////////////////////
	float			Dist2XY(const idVec3& Pt) const
	{
		return ((Pt.x-x)*(Pt.x-x) + (Pt.y-y)*(Pt.y-y));
	}
	////////////////////////////////////////////////////////////////////////////////////
	// Distance OnlyXY
	////////////////////////////////////////////////////////////////////////////////////
	float			DistXY(const idVec3& Pt) const
	{
		idVec3 delta(x,y,Pt.z);
		delta = delta - Pt;
		return delta.LengthFast();	
	}
// RAVEN END

	void			Lerp( const idVec3 &v1, const idVec3 &v2, const float l );
	void			SLerp( const idVec3 &v1, const idVec3 &v2, const float l );

// RAVEN BEGIN
// jscott: Ensures second element greater than first
	void			EnsureIncremental( void );
// jscott: for BSE
	int				GetLargestAxis( void ) const;
// jscott: for rvAngles
	rvAngles		ToRadians( void ) const;
	idMat3			&ToMat3( idMat3 &mat ) const;		// vector should be normalized
	float			Dist(const idVec3 &Pt) const 
	{
		idVec3 delta(x,y,z);
		delta = delta - Pt;
		return delta.LengthFast();	
	}

// RAVEN END

	bool			IsZero( void ) const;
};

/*
extern idVec3 vec3_origin;
#define vec3_zero vec3_origin
*/


__forceinline idVec3::idVec3( void ) {
}

__forceinline idVec3::idVec3( const float x, const float y, const float z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

__forceinline float idVec3::operator[]( const int index ) const {
	return ( &x )[ index ];
}

__forceinline float &idVec3::operator[]( const int index ) {
	return ( &x )[ index ];
}

__forceinline void idVec3::Set( const float x, const float y, const float z ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

__forceinline void idVec3::Zero( void ) {
	x = y = z = 0.0f;
}

__forceinline idVec3 idVec3::operator-() const {
	return idVec3( -x, -y, -z );
}

__forceinline idVec3 &idVec3::operator=( const idVec3 &a ) {
	x = a.x;
	y = a.y;
	z = a.z;
	return *this;
}

// RAVEN BEGIN
// bdube: added vec3 from vec2 assignment
__forceinline idVec3 &idVec3::operator=( const idVec2 &a ) {
	x = a.x;
	y = a.y;
	return *this;
}
__forceinline idVec3 &idVec3::operator*=( const idVec3 &a )
{
	x *= a.x;
	y *= a.y;
	z *= a.z;
	return *this;
}
// RAVEN END

__forceinline idVec3 idVec3::operator-( const idVec3 &a ) const {
	return idVec3( x - a.x, y - a.y, z - a.z );
}

__forceinline float idVec3::operator*( const idVec3 &a ) const {
	return x * a.x + y * a.y + z * a.z;
}

__forceinline idVec3 idVec3::operator*( const float a ) const {
	return idVec3( x * a, y * a, z * a );
}

__forceinline idVec3 idVec3::operator/( const float a ) const {
	float inva = 1.0f / a;
	return idVec3( x * inva, y * inva, z * inva );
}

__forceinline idVec3 operator*( const float a, const idVec3& b ) {
	return idVec3( b.x * a, b.y * a, b.z * a );
}

__forceinline idVec3 idVec3::operator+( const idVec3 &a ) const {
	return idVec3( x + a.x, y + a.y, z + a.z );
}

__forceinline idVec3 &idVec3::operator+=( const idVec3 &a ) {
	x += a.x;
	y += a.y;
	z += a.z;

	return *this;
}

__forceinline idVec3 &idVec3::operator/=( const idVec3 &a ) {
	x /= a.x;
	y /= a.y;
	z /= a.z;

	return *this;
}

__forceinline idVec3 &idVec3::operator/=( const float a ) {
	float inva = 1.0f / a;
	x *= inva;
	y *= inva;
	z *= inva;

	return *this;
}

__forceinline idVec3 &idVec3::operator-=( const idVec3 &a ) {
	x -= a.x;
	y -= a.y;
	z -= a.z;

	return *this;
}

__forceinline idVec3 &idVec3::operator*=( const float a ) {
	x *= a;
	y *= a;
	z *= a;

	return *this;
}

__forceinline bool idVec3::Compare( const idVec3 &a ) const {
	return ( ( x == a.x ) && ( y == a.y ) && ( z == a.z ) );
}

__forceinline bool idVec3::Compare( const idVec3 &a, const float epsilon ) const {
	if ( idMath::Fabs( x - a.x ) > epsilon ) {
		return false;
	}
			
	if ( idMath::Fabs( y - a.y ) > epsilon ) {
		return false;
	}

	if ( idMath::Fabs( z - a.z ) > epsilon ) {
		return false;
	}

	return true;
}

__forceinline bool idVec3::operator==( const idVec3 &a ) const {
	return Compare( a );
}

__forceinline bool idVec3::operator!=( const idVec3 &a ) const {
	return !Compare( a );
}

__forceinline float idVec3::NormalizeFast( void ) {
	float sqrLength, invLength;

	sqrLength = x * x + y * y + z * z;
	invLength = idMath::RSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

__forceinline bool idVec3::FixDegenerateNormal( void ) {
	if ( x == 0.0f ) {
		if ( y == 0.0f ) {
			if ( z > 0.0f ) {
				if ( z != 1.0f ) {
					z = 1.0f;
					return true;
				}
			} else {
				if ( z != -1.0f ) {
					z = -1.0f;
					return true;
				}
			}
			return false;
		} else if ( z == 0.0f ) {
			if ( y > 0.0f ) {
				if ( y != 1.0f ) {
					y = 1.0f;
					return true;
				}
			} else {
				if ( y != -1.0f ) {
					y = -1.0f;
					return true;
				}
			}
			return false;
		}
	} else if ( y == 0.0f ) {
		if ( z == 0.0f ) {
			if ( x > 0.0f ) {
				if ( x != 1.0f ) {
					x = 1.0f;
					return true;
				}
			} else {
				if ( x != -1.0f ) {
					x = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	if ( idMath::Fabs( x ) == 1.0f ) {
		if ( y != 0.0f || z != 0.0f ) {
			y = z = 0.0f;
			return true;
		}
		return false;
	} else if ( idMath::Fabs( y ) == 1.0f ) {
		if ( x != 0.0f || z != 0.0f ) {
			x = z = 0.0f;
			return true;
		}
		return false;
	} else if ( idMath::Fabs( z ) == 1.0f ) {
		if ( x != 0.0f || y != 0.0f ) {
			x = y = 0.0f;
			return true;
		}
		return false;
	}
	return false;
}

__forceinline bool idVec3::FixDenormals( void ) {
	bool denormal = false;
	if ( fabs( x ) < 1e-30f ) {
		x = 0.0f;
		denormal = true;
	}
	if ( fabs( y ) < 1e-30f ) {
		y = 0.0f;
		denormal = true;
	}
	if ( fabs( z ) < 1e-30f ) {
		z = 0.0f;
		denormal = true;
	}
	return denormal;
}

__forceinline idVec3 idVec3::Cross( const idVec3 &a ) const {
	return idVec3( y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x );
}

__forceinline idVec3 &idVec3::Cross( const idVec3 &a, const idVec3 &b ) {
	x = a.y * b.z - a.z * b.y;
	y = a.z * b.x - a.x * b.z;
	z = a.x * b.y - a.y * b.x;

	return *this;
}

// RAVEN BEGIN
// ddynerman: vector normalization operator
__forceinline idVec3& idVec3::operator~( void ) {
	Normalize();
	return *this;
}
// RAVEN END

__forceinline float idVec3::Length( void ) const {
	return ( float )idMath::Sqrt( x * x + y * y + z * z );
}

__forceinline float idVec3::LengthSqr( void ) const {
	return ( x * x + y * y + z * z );
}

__forceinline float idVec3::LengthFast( void ) const {
	float sqrLength;

	sqrLength = x * x + y * y + z * z;
	return sqrLength * idMath::RSqrt( sqrLength );
}

__forceinline float idVec3::Normalize( void ) {
	float sqrLength, invLength;

	sqrLength = x * x + y * y + z * z;
// RAVEN BEGIN
// jscott: fixed degenerate case
	if ( !sqrLength ) {		
		return 0.0f;
	}
// RAVEN END
	invLength = idMath::InvSqrt( sqrLength );
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return invLength * sqrLength;
}

__forceinline idVec3 &idVec3::Truncate( float length ) {
	float length2;
	float ilength;

	if ( !length ) {
		Zero();
	}
	else {
		length2 = LengthSqr();
		if ( length2 > length * length ) {
			ilength = length * idMath::InvSqrt( length2 );
			x *= ilength;
			y *= ilength;
			z *= ilength;
		}
	}

	return *this;
}

__forceinline void idVec3::Clamp( const idVec3 &min, const idVec3 &max ) {
	if ( x < min.x ) {
		x = min.x;
	} else if ( x > max.x ) {
		x = max.x;
	}
	if ( y < min.y ) {
		y = min.y;
	} else if ( y > max.y ) {
		y = max.y;
	}
	if ( z < min.z ) {
		z = min.z;
	} else if ( z > max.z ) {
		z = max.z;
	}
}

__forceinline void idVec3::ClampMin( const float &minx, const float &miny, const float &minz ) {
	if ( x < minx ) {
		x = minx;
	}
	if ( y < miny ) {
		y = miny;
	}
	if ( z < minz ) {
		z = minz;
	}
}

__forceinline void idVec3::Snap( void ) {
	x = floor( x + 0.5f );
	y = floor( y + 0.5f );
	z = floor( z + 0.5f );
}

__forceinline void idVec3::SnapInt( void ) {
	x = float( int( x ) );
	y = float( int( y ) );
	z = float( int( z ) );
}

__forceinline int idVec3::GetDimension( void ) const {
	return 3;
}

__forceinline const idVec2 &idVec3::ToVec2( void ) const {
	return *reinterpret_cast<const idVec2 *>(this);
}

__forceinline idVec2 &idVec3::ToVec2( void ) {
	return *reinterpret_cast<idVec2 *>(this);
}

__forceinline const float *idVec3::ToFloatPtr( void ) const {
	return &x;
}

__forceinline float *idVec3::ToFloatPtr( void ) {
	return &x;
}

__forceinline void idVec3::NormalVectors( idVec3 &left, idVec3 &down ) const {
	float d;

	d = x * x + y * y;
	if ( !d ) {
		left[0] = 1;
		left[1] = 0;
		left[2] = 0;
	} else {
		d = idMath::InvSqrt( d );
		left[0] = -y * d;
		left[1] = x * d;
		left[2] = 0;
	}
	down = left.Cross( *this );
}

__forceinline void idVec3::OrthogonalBasis( idVec3 &left, idVec3 &up ) const {
	float l, s;

	if ( idMath::Fabs( z ) > 0.7f ) {
		l = y * y + z * z;
		s = idMath::InvSqrt( l );
		up[0] = 0;
		up[1] = z * s;
		up[2] = -y * s;
		left[0] = l * s;
		left[1] = -x * up[2];
		left[2] = x * up[1];
	}
	else {
		l = x * x + y * y;
		s = idMath::InvSqrt( l );
		left[0] = -y * s;
		left[1] = x * s;
		left[2] = 0;
		up[0] = -z * left[1];
		up[1] = z * left[0];
		up[2] = l * s;
	}
}

__forceinline void idVec3::ProjectOntoPlane( const idVec3 &normal, const float overBounce ) {
	float backoff;
	
	backoff = *this * normal;
	
	if ( overBounce != 1.0 ) {
		if ( backoff < 0 ) {
			backoff *= overBounce;
		} else {
			backoff /= overBounce;
		}
	}

	*this -= backoff * normal;
}

// RAVEN BEGIN
////////////////////////////////////////////////////////////////////////////////////
// Project
//
// Standard projection function.  Take the (this) and project it onto the vector
// (U).  Imagine drawing a line perpendicular to U from the endpoint of the (this)
// Vector.  That then becomes the new vector.
//
// The value returned is the scale of the new vector with respect to the one passed
// to the function.  If the scale is less than (1.0) then the new vector is shorter 
// than (U).  If the scale is negative, then the vector is going in the opposite 
// direction of (U).
//
//               _  (U)
//               /|
//             /                                        _ (this)
//           /                      RESULTS->           /|
//         /                                          /
//       /    __\ (this)                            /
//     /___---  /                                 /
//
////////////////////////////////////////////////////////////////////////////////////
__forceinline float	idVec3::ProjectOntoVector(const idVec3 &U)
{
	float	Scale = ((*this)*(U) / U.LengthSqr());	// Find the scale of this vector on U
	(*this)=U;								// Copy U onto this vector
	(*this)*=Scale;							// Use the previously calculated scale to get the right length.
	return Scale;
}
// RAVEN END


__forceinline bool idVec3::ProjectAlongPlane( const idVec3 &normal, const float epsilon, const float overBounce ) {
	idVec3 cross;
	float len;

	cross = this->Cross( normal ).Cross( (*this) );
	// normalize so a fixed epsilon can be used
	cross.Normalize();
	len = normal * cross;
	if ( idMath::Fabs( len ) < epsilon ) {
		return false;
	}
	cross *= overBounce * ( normal * (*this) ) / len;
	(*this) -= cross;
	return true;
}

// RAVEN BEGIN
// jscott: ensures the second element is greater than the first and that the third is greater than the 2nd
__forceinline void idVec3::EnsureIncremental( void )
{
	float temp;

	if ( y < x )
	{
		temp = x;
		x = y;
		y = temp;
	}
	
	if ( z < y )
	{
		temp = y;
		y = z;
		z = temp;
	}
	
	if ( y < x )
	{
		temp = x;
		x = y;
		y = temp;
	}
}

__forceinline int idVec3::GetLargestAxis( void ) const
{
	float a = fabs( x );
	float b = fabs( y );
	float c = fabs( z );

	if( a >= b && a >= c )
	{
		return( 0 );
	}
	if( b >= a && b >= c )
	{
		return( 1 );
	}
	if( c >= a && c >= b )
	{
		return( 2 );
	}
	return( 0 );
}

__forceinline bool idVec3::IsZero( void ) const {
	return ( ( ( *( const unsigned long * ) &( x ) ) | ( *( const unsigned long * ) &( y ) ) | ( *( const unsigned long * ) &( z ) ) ) & ~( 1<<31 ) ) == 0;
}

//===============================================================
//
//	idVec5 - 5D vector
//
//===============================================================

class idVec5 {
public:
	float			x;
	float			y;
	float			z;
	float			s;
	float			t;

					idVec5( void );
					explicit idVec5( const idVec3 &xyz, const idVec2 &st );
					explicit idVec5( const float x, const float y, const float z, const float s, const float t );

	float			operator[]( int index ) const;
	float &			operator[]( int index );
	idVec5 &		operator=( const idVec3 &a );

	int				GetDimension( void ) const;

	const idVec3 &	ToVec3( void ) const;
	idVec3 &		ToVec3( void );
	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	const char *	ToString( int precision = 2 ) const;

	void			Lerp( const idVec5 &v1, const idVec5 &v2, const float l );
};

extern idVec5 vec5_origin;
#define vec5_zero vec5_origin

__forceinline idVec5::idVec5( void ) {
}

__forceinline idVec5::idVec5( const idVec3 &xyz, const idVec2 &st ) {
	x = xyz.x;
	y = xyz.y;
	z = xyz.z;
	s = st[0];
	t = st[1];
}

__forceinline idVec5::idVec5( const float x, const float y, const float z, const float s, const float t ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->s = s;
	this->t = t;
}

__forceinline float idVec5::operator[]( int index ) const {
	return ( &x )[ index ];
}

__forceinline float& idVec5::operator[]( int index ) {
	return ( &x )[ index ];
}

__forceinline idVec5 &idVec5::operator=( const idVec3 &a ) { 
	x = a.x;
	y = a.y;
	z = a.z;
	s = t = 0;
	return *this;
}

__forceinline int idVec5::GetDimension( void ) const {
	return 5;
}

__forceinline const idVec3 &idVec5::ToVec3( void ) const {
	return *reinterpret_cast<const idVec3 *>(this);
}

__forceinline idVec3 &idVec5::ToVec3( void ) {
	return *reinterpret_cast<idVec3 *>(this);
}

__forceinline const float *idVec5::ToFloatPtr( void ) const {
	return &x;
}

__forceinline float *idVec5::ToFloatPtr( void ) {
	return &x;
}


//===============================================================
//
//	idVec6 - 6D vector
//
//===============================================================

class idVec6 {
public:	
					idVec6( void );
					explicit idVec6( const float *a );
					explicit idVec6( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 );

	void 			Set( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 );
	void			Zero( void );

	float			operator[]( const int index ) const;
	float &			operator[]( const int index );
	idVec6			operator-() const;
	idVec6			operator*( const float a ) const;
	idVec6			operator/( const float a ) const;
	float			operator*( const idVec6 &a ) const;
	idVec6			operator-( const idVec6 &a ) const;
	idVec6			operator+( const idVec6 &a ) const;
	idVec6 &		operator*=( const float a );
	idVec6 &		operator/=( const float a );
	idVec6 &		operator+=( const idVec6 &a );
	idVec6 &		operator-=( const idVec6 &a );

	friend idVec6	operator*( const float a, const idVec6& b );

	bool			Compare( const idVec6 &a ) const;							// exact compare, no epsilon
	bool			Compare( const idVec6 &a, const float epsilon ) const;		// compare with epsilon
	bool			operator==(	const idVec6 &a ) const;						// exact compare, no epsilon
	bool			operator!=(	const idVec6 &a ) const;						// exact compare, no epsilon

	float			Length( void ) const;
	float			LengthSqr( void ) const;
	float			Normalize( void );			// returns length
	float			NormalizeFast( void );		// returns length

	int				GetDimension( void ) const;

	const idVec3 &	SubVec3( int index ) const;
	idVec3 &		SubVec3( int index );
	const float *	ToFloatPtr( void ) const;
	float *			ToFloatPtr( void );
	const char *	ToString( int precision = 2 ) const;

private:
	float			p[6];
};

extern idVec6 vec6_origin;
#define vec6_zero vec6_origin
extern idVec6 vec6_infinity;

__forceinline idVec6::idVec6( void ) {
}

__forceinline idVec6::idVec6( const float *a ) {
	memcpy( p, a, 6 * sizeof( float ) );
}

__forceinline idVec6::idVec6( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 ) {
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

__forceinline idVec6 idVec6::operator-() const {
	return idVec6( -p[0], -p[1], -p[2], -p[3], -p[4], -p[5] );
}

__forceinline float idVec6::operator[]( const int index ) const {
	return p[index];
}

__forceinline float &idVec6::operator[]( const int index ) {
	return p[index];
}

__forceinline idVec6 idVec6::operator*( const float a ) const {
	return idVec6( p[0]*a, p[1]*a, p[2]*a, p[3]*a, p[4]*a, p[5]*a );
}

__forceinline float idVec6::operator*( const idVec6 &a ) const {
	return p[0] * a[0] + p[1] * a[1] + p[2] * a[2] + p[3] * a[3] + p[4] * a[4] + p[5] * a[5];
}

__forceinline idVec6 idVec6::operator/( const float a ) const {
	float inva;

	assert( a != 0.0f );
	inva = 1.0f / a;
	return idVec6( p[0]*inva, p[1]*inva, p[2]*inva, p[3]*inva, p[4]*inva, p[5]*inva );
}

__forceinline idVec6 idVec6::operator+( const idVec6 &a ) const {
	return idVec6( p[0] + a[0], p[1] + a[1], p[2] + a[2], p[3] + a[3], p[4] + a[4], p[5] + a[5] );
}

__forceinline idVec6 idVec6::operator-( const idVec6 &a ) const {
	return idVec6( p[0] - a[0], p[1] - a[1], p[2] - a[2], p[3] - a[3], p[4] - a[4], p[5] - a[5] );
}

__forceinline idVec6 &idVec6::operator*=( const float a ) {
	p[0] *= a;
	p[1] *= a;
	p[2] *= a;
	p[3] *= a;
	p[4] *= a;
	p[5] *= a;
	return *this;
}

__forceinline idVec6 &idVec6::operator/=( const float a ) {
	float inva;

	assert( a != 0.0f );
	inva = 1.0f / a;
	p[0] *= inva;
	p[1] *= inva;
	p[2] *= inva;
	p[3] *= inva;
	p[4] *= inva;
	p[5] *= inva;
	return *this;
}

__forceinline idVec6 &idVec6::operator+=( const idVec6 &a ) {
	p[0] += a[0];
	p[1] += a[1];
	p[2] += a[2];
	p[3] += a[3];
	p[4] += a[4];
	p[5] += a[5];
	return *this;
}

__forceinline idVec6 &idVec6::operator-=( const idVec6 &a ) {
	p[0] -= a[0];
	p[1] -= a[1];
	p[2] -= a[2];
	p[3] -= a[3];
	p[4] -= a[4];
	p[5] -= a[5];
	return *this;
}

__forceinline idVec6 operator*( const float a, const idVec6& b ) {
	return b * a;
}

__forceinline bool idVec6::Compare( const idVec6 &a ) const {
	return ( ( p[0] == a[0] ) && ( p[1] == a[1] ) && ( p[2] == a[2] ) &&
			( p[3] == a[3] ) && ( p[4] == a[4] ) && ( p[5] == a[5] ) );
}

__forceinline bool idVec6::Compare( const idVec6 &a, const float epsilon ) const {
	if ( idMath::Fabs( p[0] - a[0] ) > epsilon ) {
		return false;
	}
			
	if ( idMath::Fabs( p[1] - a[1] ) > epsilon ) {
		return false;
	}

	if ( idMath::Fabs( p[2] - a[2] ) > epsilon ) {
		return false;
	}

	if ( idMath::Fabs( p[3] - a[3] ) > epsilon ) {
		return false;
	}

	if ( idMath::Fabs( p[4] - a[4] ) > epsilon ) {
		return false;
	}

	if ( idMath::Fabs( p[5] - a[5] ) > epsilon ) {
		return false;
	}

	return true;
}

__forceinline bool idVec6::operator==( const idVec6 &a ) const {
	return Compare( a );
}

__forceinline bool idVec6::operator!=( const idVec6 &a ) const {
	return !Compare( a );
}

__forceinline void idVec6::Set( const float a1, const float a2, const float a3, const float a4, const float a5, const float a6 ) {
	p[0] = a1;
	p[1] = a2;
	p[2] = a3;
	p[3] = a4;
	p[4] = a5;
	p[5] = a6;
}

__forceinline void idVec6::Zero( void ) {
	p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0.0f;
}

__forceinline float idVec6::Length( void ) const {
	return ( float )idMath::Sqrt( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5] );
}

__forceinline float idVec6::LengthSqr( void ) const {
	return ( p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5] );
}

__forceinline float idVec6::Normalize( void ) {
	float sqrLength, invLength;

	sqrLength = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5];
	invLength = idMath::InvSqrt( sqrLength );
	p[0] *= invLength;
	p[1] *= invLength;
	p[2] *= invLength;
	p[3] *= invLength;
	p[4] *= invLength;
	p[5] *= invLength;
	return invLength * sqrLength;
}

__forceinline float idVec6::NormalizeFast( void ) {
	float sqrLength, invLength;

	sqrLength = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3] + p[4] * p[4] + p[5] * p[5];
	invLength = idMath::RSqrt( sqrLength );
	p[0] *= invLength;
	p[1] *= invLength;
	p[2] *= invLength;
	p[3] *= invLength;
	p[4] *= invLength;
	p[5] *= invLength;
	return invLength * sqrLength;
}

__forceinline int idVec6::GetDimension( void ) const {
	return 6;
}

__forceinline const idVec3 &idVec6::SubVec3( int index ) const {
	return *reinterpret_cast<const idVec3 *>(p + index * 3);
}

__forceinline idVec3 &idVec6::SubVec3( int index ) {
	return *reinterpret_cast<idVec3 *>(p + index * 3);
}

__forceinline const float *idVec6::ToFloatPtr( void ) const {
	return p;
}

__forceinline float *idVec6::ToFloatPtr( void ) {
	return p;
}



//===============================================================
//
//	idPolar3
//
//===============================================================

class idPolar3 {
public:	
	float			radius, theta, phi;

					idPolar3( void );
					explicit idPolar3( const float radius, const float theta, const float phi );

	void 			Set( const float radius, const float theta, const float phi );

	float			operator[]( const int index ) const;
	float &			operator[]( const int index );
	idPolar3		operator-() const;
	idPolar3 &		operator=( const idPolar3 &a );

	idVec3			ToVec3( void ) const;
};

__forceinline idPolar3::idPolar3( void ) {
}

__forceinline idPolar3::idPolar3( const float radius, const float theta, const float phi ) {
	assert( radius > 0 );
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}
	
__forceinline void idPolar3::Set( const float radius, const float theta, const float phi ) {
	assert( radius > 0 );
	this->radius = radius;
	this->theta = theta;
	this->phi = phi;
}

__forceinline float idPolar3::operator[]( const int index ) const {
	return ( &radius )[ index ];
}

__forceinline float &idPolar3::operator[]( const int index ) {
	return ( &radius )[ index ];
}

__forceinline idPolar3 idPolar3::operator-() const {
	return idPolar3( radius, -theta, -phi );
}

__forceinline idPolar3 &idPolar3::operator=( const idPolar3 &a ) { 
	radius = a.radius;
	theta = a.theta;
	phi = a.phi;
	return *this;
}

__forceinline idVec3 idPolar3::ToVec3( void ) const {
	float sp, cp, st, ct;
	idMath::SinCos( phi, sp, cp );
	idMath::SinCos( theta, st, ct );
	return idVec3( cp * radius * ct, cp * radius * st, radius * sp );
}


/*
===============================================================================

	Old 3D vector macros, should no longer be used.

===============================================================================
*/

#ifndef DotProduct
#define DotProduct( a, b)			((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#endif
#define VectorSubtract( a, b, c )	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd( a, b, c )		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define	VectorScale( v, s, o )		((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA( v, s, b, o )		((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define VectorCopy( a, b )			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])


#endif /* !__MATH_VECTOR_H__ */
