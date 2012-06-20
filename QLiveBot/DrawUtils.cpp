#include "hook.h"
#include "../Common/Math.h"
#include "../Common/Vector.h"
#include "../Common/Angles.h"
#include "../Common/Matrix.h"
#include "DrawUtils.h"
#include <windows.h>
#include <math.h>

float getFovX();
float getFovY();
idAngles getRefDefViewAngles();
idVec3 getRefDefViewOrg();


int PASSFLOAT( float x )
{
    float	floatTemp;
    floatTemp = x;
    return *(int *)&floatTemp;
}

void trap_R_DrawStretchPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, DWORD hShader )
{
    Original_Syscall( CG_R_DRAWSTRETCHPIC, PASSFLOAT(x), PASSFLOAT(y), PASSFLOAT(w), PASSFLOAT(h), PASSFLOAT(s1), PASSFLOAT(t1), PASSFLOAT(s2), PASSFLOAT(t2), hShader );
}

void trap_R_SetColor( const float rgba[4] )
{
    Original_Syscall(CG_R_SETCOLOR, rgba);
}

void DrawUtils::DrawPic( float x, float y, float width, float height, DWORD hShader )
{
    float	s0;
    float	s1;
    float	t0;
    float	t1;

    if( width < 0 ) {	// flip about vertical
        width  = -width;
        s0 = 1;
        s1 = 0;
    }
    else {
        s0 = 0;
        s1 = 1;
    }

    if( height < 0 ) {	// flip about horizontal
        height= -height;
        t0 = 1;
        t1 = 0;
    }
    else {
        t0 = 0;
        t1 = 1;
    }
    
    AdjustFrom640( &x, &y, &width, &height );
    trap_R_DrawStretchPic( x, y, width, height, s0, t0, s1, t1, hShader );
}

void DrawUtils::AdjustFrom640( float *x, float *y, float *w, float *h )
{
    float cgs_screenYScale = *(cgs_screenXScale+1);
    float vidWidth = *cgs_screenXScale * 640.0f;
    float vidHeight = *cgs_screenXScale * 480.0f;
    // adjust for wide screens
    if ( vidWidth * 480.0f > vidHeight * 640.0f )
        *x += 0.5f * ( vidWidth - ( vidHeight * 640.0f / 480.0f ) );
    
    // scale for screen sizes
    *x *= *cgs_screenXScale;
    *y *= cgs_screenYScale;
    *w *= *cgs_screenXScale;
    *h *= *(cgs_screenXScale+1);
}

void DrawUtils::DrawChar( int x, int y, int width, int height, int ch )
{
    int row, col;
    float frow, fcol;
    float size;
    float	ax, ay, aw, ah;

    ch &= 255;

    if ( ch == ' ' )
        return;

    ax = x;
    ay = y;
    aw = width;
    ah = height;
    AdjustFrom640( &ax, &ay, &aw, &ah );

    row = ch>>4;
    col = ch&15;

    frow = row*0.0625;
    fcol = col*0.0625;
    size = 0.0625;

    trap_R_DrawStretchPic( ax, ay, aw, ah,
        fcol, frow, 
        fcol + size, frow + size, 
        Shaders[XORSTR("charset")] );
}

#define Q_COLOR_ESCAPE	'^'
#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE )

void DrawUtils::DrawStringExt( int x, int y, const char *string, float Color[4], bool shadow, bool center, int charWidth, int charHeight, int maxChars )
{
    const char	*s;
    int			xx;
    int			cnt;
    float		color[4];
    
    if (maxChars <= 0)
        maxChars = 32767; // do them all!

    if(center)
    {
        int len = 0;
        s = string;
        while ( *s && len < maxChars)
        {
            if ( Q_IsColorString( s ) )
            {
                s += 2;
                continue;
            }
            len++;
            s++;
        }
        if(len > maxChars)
            len = maxChars;
        x -= (len*(charWidth-charWidth/4))/2;
    }

    // draw the drop shadow
    if (shadow)
    {
        color[0] = color[1] = color[2] = 0;
        color[3] = Color[3];
        trap_R_SetColor( color );
        s = string;
        xx = x;
        cnt = 0;
        while ( *s && cnt < maxChars) {
            if ( Q_IsColorString( s ) ) {
                s += 2;
                continue;
            }
            DrawChar( xx + 1, y + 1, charWidth, charHeight, *s );
            cnt++;
            xx += charWidth-charWidth/4;
            s++;
        }
    }

    // draw the colored text
    s = string;
    xx = x;
    cnt = 0;
    trap_R_SetColor( Color );
    while ( *s && cnt < maxChars)
    {
        if ( Q_IsColorString( s ) )
        {
            s += 2;
            continue;
        }
        DrawChar( xx, y, charWidth, charHeight, *s );
        xx += charWidth-charWidth/4;
        cnt++;
        s++;
    }
    trap_R_SetColor( nullptr );
}

bool DrawUtils::WorldToScreen( const idVec3& point, float *out )
{
    float   xc, yc;
    float   px, py;
    float   z;

    px = tan( getFovX() * M_PI / 360.0 );
    py = tan( getFovY() * M_PI / 360.0 );

    idVec3 trans = point - getRefDefViewOrg();

    xc = 640.0f / 2.0f;
    yc = 480.0f / 2.0f;
    
    auto viewAxis = getRefDefViewAngles().ToMat3();

    z = trans * viewAxis[ 0 ];
    if( z <= 0.001f )
            return false;

    out[0] = xc - trans * viewAxis[ 1 ] * xc / ( z * px );

    out[1] = yc - trans * viewAxis[ 2 ] * yc / ( z * py );

    return true;
}

void DrawUtils::FillRectangle(float x, float y, float w, float h, const float color[4])
{
    trap_R_SetColor(color);
    AdjustFrom640(&x, &y, &w, &h);
    trap_R_DrawStretchPic(x, y, w, h, 0.0f, 1.0f, 0.0f, 1.0f, Shaders[XORSTR("white")]);
}

void DrawUtils::DrawRectangle(float x, float y, float w, float h, const float color[4])
{
    FillRectangle(x, y, w, 1.0, color);
    FillRectangle(x, y+h, w, 1.0, color);
    FillRectangle(x, y, 1.0, h, color);
    FillRectangle(x+w-1.0, y, 1.0, h, color);
}

/*idVec4 DrawUtils::ColorFromHexString(const char * s)
{
    auto color = strtoul(s, nullptr, 16);
    auto r = (color >> 24) / 255.0;
    auto g = ((color & 0x00FFFFFF) >> 16) / 255.0;
    auto b = ((color & 0x0000FFFF) >> 8) / 255.0;
    auto a = (color & 0x000000FF) / 255.0;
    idVec4(r, g, b, a);
}*/

float Colors::White[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float Colors::Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float Colors::Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
float Colors::Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
float Colors::Yellow[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
float Colors::TransGreen[4] = { 0.0f, 1.0f, 0.0f, 0.35f };
float Colors::TransBlack[4] = { 0.0f, 0.0f, 0.0f, 0.3f };