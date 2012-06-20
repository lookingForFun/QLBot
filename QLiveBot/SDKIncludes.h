/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
--------------------------------------------------------------

This program is in no way supported by Id Software.
*/

#ifndef SDKINCLUDESH
#define SDKINCLUDESH

typedef unsigned char byte;

//from the SDK
typedef float vec3_t[3];

#define	MAX_CLIENTS			64		// absolute limit
#define	GENTITYNUM_BITS		10		// don't need to send any more
#define	MAX_GENTITIES		(1<<GENTITYNUM_BITS)
#define ANIM_TOGGLEBIT		128
#define	EF_DEAD				0x00000001
#define	DEFAULT_VIEWHEIGHT	26
#define CROUCH_VIEWHEIGHT	12
typedef struct cplane_s {
	vec3_t	normal;
	float	dist;
	byte	type;			// for fast side tests: 0,1,2 = axial, 3 = nonaxial
	byte	signbits;		// signx + (signy<<1) + (signz<<2), used as lookup during collision
	byte	pad[2];
} cplane_t;
typedef struct {
	int	allsolid;	// if true, plane is not valid
	int startsolid;	// if true, the initial point was in a solid area
	float		fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t		endpos;		// final position
	cplane_t	plane;		// surface normal at impact, transformed to world space
	int			surfaceFlags;	// surface hit
	int			contents;	// contents on other side of surface hit
	int			entityNum;	// entity the contacted sirface is a part of
} trace_t;

#define	CONTENTS_SOLID			1		// an eye is never valid in a solid
#define	CONTENTS_LAVA			8
#define	CONTENTS_SLIME			16
#define	CONTENTS_WATER			32
#define	CONTENTS_FOG			64

#define CONTENTS_NOTTEAM1		0x0080
#define CONTENTS_NOTTEAM2		0x0100
#define CONTENTS_NOBOTCLIP		0x0200

#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000
//bot specific contents types
#define	CONTENTS_TELEPORTER		0x40000
#define	CONTENTS_JUMPPAD		0x80000
#define CONTENTS_CLUSTERPORTAL	0x100000
#define CONTENTS_DONOTENTER		0x200000
#define CONTENTS_BOTCLIP		0x400000
#define CONTENTS_MOVER			0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_BODY			0x2000000	// should never be on a brush, only in game
#define	CONTENTS_CORPSE			0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes not used for the bsp
#define	CONTENTS_STRUCTURAL		0x10000000	// brushes used for the bsp
#define	CONTENTS_TRANSLUCENT	0x20000000	// don't consume surface fragments inside
#define	CONTENTS_TRIGGER		0x40000000
#define	CONTENTS_NODROP			0x80000000	// don't leave bodies or items (death fog, lava)


#define	MASK_ALL				(-1)
#define	MASK_SOLID				(CONTENTS_SOLID)
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY)
#define	MASK_DEADSOLID			(CONTENTS_SOLID|CONTENTS_PLAYERCLIP)
#define	MASK_WATER				(CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME)
#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA)
#define	MASK_SHOT				(CONTENTS_SOLID|CONTENTS_BODY)

typedef struct {
	int		trType;
	int		trTime;
	int		trDuration;			// if non 0, trTime + trDuration = stop time
	vec3_t	trBase;
	vec3_t	trDelta;			// velocity, etc
} trajectory_t;

typedef struct usercmd_s {
	int				serverTime;
	int				angles[3];
	int 			buttons;
	byte			weapon;           // weapon 
	signed char	forwardmove, rightmove, upmove;
} usercmd_t;

typedef struct {
	int			down[2];		// key nums holding it down
	unsigned	downtime;		// msec timestamp
	unsigned	msec;			// msec down this frame if both a down and up happened
	int			active;			// current state
	int			wasPressed;		// set when down, not cleared when up
} kbutton_t;

#endif