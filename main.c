#include <gba_video.h>
#include <gba_input.h>
#include <gba_interrupt.h>

#include "main.h"
#include "draw.h"
#include "emath.h"
#include "global.h"
#include "blocks.h"
#include "lut.h"
#include "player.h"

int display_frame = 0;
int input;
u16* vid_addr;
int debug_val;

IWRAM_CODE void profile(int col) {
    int current_line = REG_VCOUNT;
    plot_pixel(vid_addr + (current_line >> 1), col);
}

IWRAM_CODE static void draw_crosshair() {
    plot_pixel(vid_addr + 120 * 160 / 2 + 60, 1);
}

IWRAM_CODE static void clear_screen() {
    for (int i = 0; i < 240 * 160 / 2; i++) {
        *(vid_addr + i) = 0x0000;
    }
}

IWRAM_CODE int main() {
    const int vidmode = MODE_4 | BG2_ON;
    REG_DISPCNT = vidmode;

    *(BG_PALETTE + 1) = RGB5(15, 5, 20);
    *(BG_PALETTE + 2) = RGB8(0x9b, 0xad, 0xb7);
    *(BG_PALETTE + 3) = RGB8(0x84, 0x7e, 0x87);

    init_palette();
    
    int x, y;
    int frame = 0;

    init_blockmap();

    // REG_IME = 1;
    // REG_IE |= IRQ_VBLANK;
    // REG_

    while (1) {
        while(REG_VCOUNT != 0);

        REG_DISPCNT = vidmode | (display_frame ? BIT(4) : 0);
        vid_addr = (u16*) (0x06000000 + (display_frame ? 0 : 0xA000));

        input = REG_KEYINPUT;

        int next = !(input & KEY_SELECT);
        static int holding_sel = 0;
        if (!holding_sel && next) {
            freeze_traversal ^= 1;
        }
        holding_sel = next;

        // cam_x_sin = fastsin(cam_rot.x);
        // cam_x_cos = fastcos(cam_rot.x);

        // cam_y_sin = fastsin(cam_rot.y);
        // cam_y_cos = fastcos(cam_rot.y);

        player_update();

        //clear_screen(); // cpuopt
        //profile(3);
        traverse_blocks();
        //profile(4);
        draw_blocks();
        //profile(5);
        
        if (freeze_traversal)
            plot_pixel(vid_addr + 120 * 16, 1);

        draw_crosshair();

        frame++;
        display_frame ^= 1;
        
    }
}

int fastrandom() {
    static int x = 21938473;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 15;
    return x;
}
