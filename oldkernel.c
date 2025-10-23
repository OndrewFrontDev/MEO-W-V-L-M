// kernel.c — spoločný C kernel pre WASM aj natív
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

static uint8_t *FB = NULL;
static int W = 0, H = 0;

static inline void put_px(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
    if ((unsigned)x >= (unsigned)W || (unsigned)y >= (unsigned)H) return;
    int i = (x + y * W) * 4;
    FB[i+0]=r; FB[i+1]=g; FB[i+2]=b; FB[i+3]=a;
}

static void fill_rect(int x0,int y0,int x1,int y1,uint8_t r,uint8_t g,uint8_t b,uint8_t a){
    if (x0<0) x0=0; if (y0<0) y0=0;
    if (x1>W) x1=W; if (y1>H) y1=H;
    for (int y=y0; y<y1; ++y){
        int i=(x0 + y*W)*4;
        for (int x=x0; x<x1; ++x){ FB[i]=r; FB[i+1]=g; FB[i+2]=b; FB[i+3]=a; i+=4; }
    }
}

static void fill_circle(int cx,int cy,int r,uint8_t R,uint8_t G,uint8_t B,uint8_t A){
    int r2=r*r;
    for (int y=-r; y<=r; ++y){
        int yy=cy+y;
        int dx=(int)floorf(sqrtf((float)(r2 - y*y)));
        int x0=cx-dx, x1=cx+dx;
        for (int x=x0; x<=x1; ++x) put_px(x,yy,R,G,B,A);
    }
}

#ifdef __EMSCRIPTEN__
__attribute__((export_name("init")))
#endif
void init(int w, int h){
    if (FB){ free(FB); FB=NULL; }
    W=w; H=h;
    FB=(uint8_t*)malloc((size_t)W*(size_t)H*4);
}

#ifdef __EMSCRIPTEN__
__attribute__((export_name("fb_ptr")))
#endif
int fb_ptr(void){ return (int)(uintptr_t)FB; }

#ifdef __EMSCRIPTEN__
__attribute__((export_name("width")))
#endif
int width(void){ return W; }

#ifdef __EMSCRIPTEN__
__attribute__((export_name("height")))
#endif
int height(void){ return H; }

#ifdef __EMSCRIPTEN__
__attribute__((export_name("tick")))
#endif
void tick(float t){
    // 1) modré pozadie
    fill_rect(0,0,W,H, 0x1e,0x66,0xf5, 0xff);

    // 2) jablko
    int r = (W < H ? W : H) * 22 / 100;
    int cx = W/2;
    int cy = (int)(H * 0.58f);

    // dve "gule"
    fill_circle(cx - r/2, cy, r, 0xe1,0x1d,0x48, 0xff);
    fill_circle(cx + r/2, cy, r, 0xe1,0x1d,0x48, 0xff);

    // spoj dolný pás
    int y0 = cy + (int)(0.45f*r), y1 = cy + (int)(0.95f*r);
    fill_rect(cx - r, y0, cx + r, y1, 0xe1,0x1d,0x48, 0xff);

    // stopka
    for (int y=0; y<r/2; ++y){
        int x = cx + (int)(0.15f*r);
        int yy = cy - (int)(1.2f*r) - y;
        for (int dx=-(r/20); dx<= (r/20); ++dx)
            put_px(x+dx, yy, 0x3f,0x2d,0x20, 0xff);
    }

    // list (elipsa)
    int rx=(int)(0.35f*r), ry=(int)(0.18f*r);
    int lx=cx+(int)(0.28f*r), ly=cy-(int)(1.5f*r);
    for (int y=-ry; y<=ry; ++y)
      for (int x=-rx; x<=rx; ++x){
        float X=(float)x/(float)rx, Y=(float)y/(float)ry;
        if (X*X + Y*Y <= 1.0f) put_px(lx+x, ly+y, 0x16,0xa3,0x4a, 0xff);
      }

    // highlight (jemná pulzácia)
    int rr = r/4 + (int)(sinf(t*2.0f)* (r*0.03f));
    fill_circle(cx - (int)(0.6f*r), cy - (int)(0.4f*r), rr, 255,255,255, 170);
}
