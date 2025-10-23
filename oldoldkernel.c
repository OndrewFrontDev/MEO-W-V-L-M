// kernel.c — načíta a vykreslí zabudovaný obrázok meowl.png
#include <stdint.h>
#include <stdlib.h>

static uint8_t *FB = NULL;
static int W = 0, H = 0;

// ===============================================================
// obrázok vygenerovaný z meowl.png pomocou https://notisrac.github.io/FileToCArray/
// uprav rozmery na skutočnú veľkosť PNG
#define MEOWL_WIDTH 720
#define MEOWL_HEIGHT 981

// --- vlož sem svoj vygenerovaný RGBA array ---
static const uint8_t meowl_rgba[MEOWL_WIDTH * MEOWL_HEIGHT * 4] = {
    #include "png.txt"   // alebo sem priamo vlož celé pole {0x..,0x.., ...}
};
// ===============================================================

#ifdef __EMSCRIPTEN__
__attribute__((export_name("init")))
#endif
void init(int w, int h) {
    if (FB) { free(FB); FB = NULL; }
    W = w; H = h;
    FB = (uint8_t*)malloc((size_t)W * (size_t)H * 4);
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

#ifdef __EMSCRIPTEN__
__attribute__((export_name("tick")))
#endif
void tick(float t) {
    if (!FB) return;

    // ak máš iný pomer strán, prispôsob pozíciu obrázka
    int startX = (W - MEOWL_WIDTH) / 2;
    int startY = (H - MEOWL_HEIGHT) / 2;
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;

    for (int y = 0; y < MEOWL_HEIGHT && y + startY < H; ++y) {
        for (int x = 0; x < MEOWL_WIDTH && x + startX < W; ++x) {
            int dst = ((startX + x) + (startY + y) * W) * 4;
            int src = (x + y * MEOWL_WIDTH) * 4;
            FB[dst + 0] = meowl_rgba[src + 0];
            FB[dst + 1] = meowl_rgba[src + 1];
            FB[dst + 2] = meowl_rgba[src + 2];
            FB[dst + 3] = meowl_rgba[src + 3];
        }
    }
}
