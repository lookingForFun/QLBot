#pragma once

class Colors
{ 
public:
	static float White[4];
	static float TransGreen[4];
	static float Red[4];
	static float Blue[4];
	static float Green[4];
	static float Yellow[4];
	static float TransBlack[4];
};

class DrawUtils
{
	static void Draw3dBoxFace(idVec3 a,idVec3 b,idVec3 c,idVec3 d, float color[4]);
public:
	static void AdjustFrom640( float *x, float *y, float *w, float *h );
	static void DrawChar( int x, int y, int width, int height, int ch );
	static void DrawStringExt( int x, int y, const char *string, float Color[4], bool shadow, bool center, int charWidth = 10, int charHeight = 10, int maxChars = 0);
	static bool WorldToScreen(const idVec3& in, float* out);
	static void FillRectangle(float x, float y, float w, float h, const float color[4]);
	static void DrawRectangle(float x, float y, float w, float h, const float color[4]);
	static void DrawPic( float x, float y, float width, float height, unsigned long hShader );
	static void Draw3dBox(idVec3 mins, idVec3 maxs, float color[4]);
};