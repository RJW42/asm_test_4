#include "../drivers/screen.h"
#include "../drivers/ports.h"

// Might be useful http://www.rohitab.com/discuss/topic/35103-switch-between-real-mode-and-protected-mode/
typedef struct VGAPorts{
    unsigned short mics;
    unsigned short crtc_index;   // Cathode ray tude controller Index 
    unsigned short crtc_data;    // Cathode ray tude controller Data 
    unsigned short sequencer_index;
    unsigned short sequencer_data;
    unsigned short graphics_controller_index;
    unsigned short graphics_controller_data;
    unsigned short attribute_controller_index;
    unsigned short attribute_controller_read;
    unsigned short attribute_controller_write;
    unsigned short attribute_controller_reset;
} VGAPorts;

int set_vga_mode(unsigned int width, unsigned int height, unsigned int color_depth, VGAPorts* ports);
int supports_mode(unsigned int width, unsigned int height, unsigned int color_depth);

void write_vga_registers(unsigned char * registers, VGAPorts* ports);
void put_pixel(unsigned int x, unsigned int y,  unsigned char color_value, VGAPorts* ports);
void init_pizel_values(int* values, VGAPorts* ports);

unsigned char* get_frame_buffer_segment(VGAPorts* ports);

void main() {
    VGAPorts ports = {
        0x3c2,
        0x3d4,
        0x3d5,
        0x3c4,
        0x3c5,
        0x3ce,
        0x3cf,
        0x3c0,
        0x3c1, 
        0x3c0,
        0x3da
    };

    set_vga_mode(320, 200, 8, &ports);

    for(int x = 0; x < 320; x++)
        for(int y = 0; y < 200; y++)
            put_pixel(x, y, 10, &ports);
}


int set_vga_mode(unsigned int width, unsigned int height, unsigned int color_depth, VGAPorts* ports){
    // Check if the mode is unsuported 
    if(!supports_mode(width, height, color_depth)){
        return 0;
    }

    // Set the onde mode 
    unsigned char g_320x200x256[] = {
        /* MISC */
            0x63,
        /* SEQ */
            0x03, 0x01, 0x0F, 0x00, 0x0E,
        /* CRTC */
            0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
            0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
            0xFF,
        /* GC */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
            0xFF,
        /* AC */
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x41, 0x00, 0x0F, 0x00,	0x00
    };

    write_vga_registers(g_320x200x256, ports);

    return 1; // True as if reached here then all good 
}

int supports_mode(unsigned int width, unsigned int height, unsigned int color_depth){
    // Only Support One Mode 
    return width == 320 && height == 200 && color_depth == 8;
}

void write_vga_registers(unsigned char * registers, VGAPorts* ports){
    // Misc 1 
    port_byte_out(ports->mics, *(registers++));
    
    // SEQ 5
    for(unsigned char i = 0; i < 5; i++){
        port_byte_out(ports->sequencer_index, i); // Set index 
        port_byte_out(ports->sequencer_data, *(registers++)); // Set Value 
    }

    // CRTC 25 
    // Need to unlock cathod before sending data 
    port_byte_out(ports->crtc_index, 0x03);
    port_byte_out(ports->crtc_data, port_byte_in(ports->crtc_data) | 0x80); 
    port_byte_out(ports->crtc_index, 0x11);
    port_byte_out(ports->crtc_data, port_byte_in(ports->crtc_data) & ~0x80); 

    registers[0x03] = registers[0x03] | 0x80; // Prevent locking 
    registers[0x11] = registers[0x11] | ~0x80;

    for(unsigned char i = 0; i < 25; i++){
        port_byte_out(ports->crtc_index, i); // Set Index 
        port_byte_out(ports->crtc_data, *(registers++)); // Set Value
    }

    // GC 9
    for(unsigned char i = 0; i < 9; i++){
        port_byte_out(ports->graphics_controller_index, i); // Set Index
        port_byte_out(ports->graphics_controller_data, *(registers++)); // Set Value
    }

    // AC 21 
    for(unsigned char i = 0; i < 21; i++){
        port_byte_in(ports->attribute_controller_reset); // Reset before send data. Reading triggers a reset 
        port_byte_out(ports->attribute_controller_index, i); // Set Index 
        port_byte_out(ports->attribute_controller_write, *(registers++)); // Set Value
    }

    port_byte_in(ports->attribute_controller_reset); // Reset again 
    port_byte_out(ports->attribute_controller_index, 0x20);
}

void put_pixel(unsigned int x, unsigned int y,  unsigned char color_value, VGAPorts* ports){
    unsigned char* pixel_address = get_frame_buffer_segment(ports) + 320*y + x; // TODO change if wanting to update height width
    *pixel_address = color_value;
}

unsigned char* get_frame_buffer_segment(VGAPorts* ports){
    port_byte_out(ports->graphics_controller_index, 0x06);
    unsigned char seg_number = ((port_byte_in(ports->graphics_controller_data) >> 2) & 0x03);

    switch(seg_number){
        default:
        case 0: return (unsigned char*)0x00000;
        case 1: return (unsigned char*)0xA0000;
        case 2: return (unsigned char*)0xB0000;
        case 3: return (unsigned char*)0xB8000;
    }
}

void init_pizel_values(int* values, VGAPorts* ports){

}



