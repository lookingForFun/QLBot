#include "../Common/ImportInterfaces.h"
#include "../Common/ImportExport.h"
#include "../Common/Angles.h"
#include "hook.h"
#include "DrawUtils.h"

typedef struct {
	idVec3 xyz;
	float st[2];
	byte modulate[4];
} polyVert_t;

#define veccpy(b,a)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define setcolor(b, a) ((b)[0]=(byte)((a)[0]*255),(b)[1]=(byte)((a)[1]*255),(b)[2]=(byte)((a)[2]*255),(b)[3]=(byte)((a)[3]*255))

void DrawUtils::Draw3dBoxFace(idVec3 a,idVec3 b,idVec3 c,idVec3 d, float color[4])
{
	polyVert_t	verts[4];

	verts[0].xyz = a;
	verts[0].st[0] = 0;
	verts[0].st[1] = 0;
	setcolor(verts[0].modulate, color);

	verts[1].xyz = b;
	verts[1].st[0] = 0;
	verts[1].st[1] = 1;
	setcolor(verts[1].modulate, color);

	verts[2].xyz = c;
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	setcolor(verts[2].modulate, color);

	verts[3].xyz = d;
	verts[3].st[0] = 1;
	verts[3].st[1] = 0;
	setcolor(verts[3].modulate, color);

	Original_Syscall( CG_R_ADDPOLYTOSCENE, Shaders[XORSTR("white")], 4, verts );
} 


void DrawUtils::Draw3dBox(idVec3 mins, idVec3 maxs, float color[4])
{
	idVec3 a,b,c,d,e,f,g,h,temp,orig;

	float x = (maxs[0] - mins[0])/2;
	float y = (maxs[1] - mins[1])/2;
	float z = (maxs[2] - mins[2])/2;

	orig[0] = mins[0] + x;
	orig[1] = mins[1] + y;
	orig[2] = mins[2] + z;


	a=b=c=d=e=f=g=h= orig;

	temp = idVec3(-x,-y,-z);
	a += temp;

	temp = idVec3(-x,-y,z);
	b += temp;

	temp = idVec3(x,-y,z);
	c += temp;

	temp = idVec3(x,-y,-z);
	d += temp;


	temp = idVec3(-x,y,-z);
	e += temp;

	temp = idVec3(-x,y,z);
	f += temp;

	temp = idVec3(x,y,z);
	g += temp;

	temp = idVec3(x,y,-z);
	h += temp;

	//we need to trace ABCD, CGHD, EFGH, ABFE, BFGC, AEHD

	Draw3dBoxFace(a,b,c,d,color);
	Draw3dBoxFace(c,g,h,d,color);
	Draw3dBoxFace(e,f,g,h,color);
	Draw3dBoxFace(a,b,f,e,color);
	Draw3dBoxFace(b,f,g,c,color);
	Draw3dBoxFace(a,e,h,d,color);

	Draw3dBoxFace(d,c,b,a,color); //avoid the culling effect
	Draw3dBoxFace(d,h,g,c,color);
	Draw3dBoxFace(h,g,f,e,color);
	Draw3dBoxFace(e,f,b,a,color);
	Draw3dBoxFace(c,g,f,b,color);
	Draw3dBoxFace(d,h,e,a,color);

}

