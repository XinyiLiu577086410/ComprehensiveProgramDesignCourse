#ifndef BITMAP
#define BITMAP
unsigned char bit0 = 0x1;
unsigned char bit1 = bit0 << 1;
unsigned char bit2 = bit1 << 1;
unsigned char bit3 = bit2 << 1;
unsigned char bit4 = bit3 << 1;
unsigned char bit5 = bit4 << 1;
unsigned char bit6 = bit5 << 1;
unsigned char bit7 = bit6 << 1;

unsigned char masks[8] = {
    bit0,
    bit1,
    bit2,
    bit3,
    bit4,
    bit5,
    bit6,
    bit7,
};

#endif