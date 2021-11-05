int set_vga_mode(unsigned int width, unsigned int height, unsigned int color_depth);

void put_pixel_exact(unsigned int x, unsigned int y, unsigned char color_value);
void put_pixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b);