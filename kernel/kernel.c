#include "../drivers/screen.h"
#include "../drivers/ports.h"
#include "../drivers/vga.h"

void main() {
    // Set the CPU into graphics mode. Width = 320, Height = 200, ColorDepth = 8
    set_vga_mode(320, 200, 8);

    // Print a graphic to the screen
    for(int x = 0; x < 320; x++)
        for(int y = 0; y < 200; y++){
            if(x % 2 == 0){
                put_pixel(x, y, 0, 50, 0);
            }else{
                put_pixel(x, y, 50, 0, 0);
            }
        }
}
