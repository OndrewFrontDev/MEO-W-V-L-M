// kernel.c — vykreslí zabudovaný obrázok (0xRRGGBB) do RGBA framebufferu (WASM/native)
#include <stdint.h>
#include <stdlib.h>

static uint8_t *FB = NULL;   // RGBA framebuffer (W*H*4)
static int W = 0, H = 0;

// ====== ROZMERY VÁŠHO OBRÁZKA (UPRAV PODĽA REALITY) =========================
#define MEOWL_WIDTH   720
#define MEOWL_HEIGHT  981
// ============================================================================

// png.txt MUSÍ obsahovať len čiarkou oddelené 32-bitové hodnoty 0xRRGGBB
// napr.: 0x112233, 0xaabbcc, ...
static const uint32_t meowl_rgb[MEOWL_WIDTH * MEOWL_HEIGHT] = {
    #include "png.txt"
};

#ifdef __EMSCRIPTEN__
__attribute__((export_name("init")))
#endif
void init(int w, int h) {
    if (FB) { free(FB); FB = NULL; }
    W = w; H = h;
    FB = (uint8_t*)malloc((size_t)W * (size_t)H * 4);
    // voliteľne vymaž na čiernu
    if (FB) {
        for (int i = 0; i < W*H*4; ++i) FB[i] = (i%4==3) ? 255 : 0; // A=255
    }
}

#ifdef __EMSCRIPTEN__
__attribute__((export_name("fb_ptr")))
#endif
int fb_ptr(void) { return (int)(uintptr_t)FB; }

#ifdef __EMSCRIPTEN__
__attribute__((export_name("width")))
#endif
int width(void) { return W; }

#ifdef __EMSCRIPTEN__
__attribute__((export_name("height")))
#endif
int height(void) { return H; }

// Jednorazový render (t nerešíme, nechávam pre kompatibilitu s volaním z JS)
#ifdef __EMSCRIPTEN__
__attribute__((export_name("tick")))
#endif
void tick(float t) {
    if (!FB) return;

    // Centrovanie obrázka na plátno; ak je väčší než plátno, orežeme
    int startX = (W - MEOWL_WIDTH)  / 2;
    int startY = (H - MEOWL_HEIGHT) / 2;

    // Základné orezanie (clipping) do rozsahu framebufferu
    int srcX0 = 0, srcY0 = 0;
    int dstX0 = startX, dstY0 = startY;
    int drawW = MEOWL_WIDTH, drawH = MEOWL_HEIGHT;

    if (dstX0 < 0) { srcX0 = -dstX0; drawW -= srcX0; dstX0 = 0; }
    if (dstY0 < 0) { srcY0 = -dstY0; drawH -= srcY0; dstY0 = 0; }
    if (dstX0 + drawW > W) drawW = W - dstX0;
    if (dstY0 + drawH > H) drawH = H - dstY0;
    if (drawW <= 0 || drawH <= 0) return;

    // Kopírovanie: 0xRRGGBB -> RGBA8888 (A=255)
    for (int y = 0; y < drawH; ++y) {
        int srcRow = (srcY0 + y) * MEOWL_WIDTH;
        int dstRow = (dstY0 + y) * W;
        for (int x = 0; x < drawW; ++x) {
            uint32_t px = meowl_rgb[srcX0 + x + srcRow];
            int dst = (dstX0 + x + dstRow) * 4;
            FB[dst + 0] = (px >> 16) & 0xFF; // R
            FB[dst + 1] = (px >>  8) & 0xFF; // G
            FB[dst + 2] = (px      ) & 0xFF; // B
            FB[dst + 3] = 255;               // A
        }
    }
}
