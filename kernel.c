#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/*
build instructions for this file 
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
*/

enum vga_color {
	BLACK = 0,
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	BROWN = 6,
	LIGHT_GREY = 7,
	DARK_GREY = 8,
	LIGHT_BLUE = 9,
	LIGHT_GREEN = 10,
	LIGHT_CYAN = 11,
	LIGHT_RED = 12,
	LIGHT_MAGENTA = 13,
	LIGHT_BROWN = 14,
	WHITE = 15,
};


static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(GREEN, BLACK);
    /*VGA buffer start at this location*/
	terminal_buffer = (uint16_t*) 0xB8000; 
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    /*  if text reaches to its limit the reset
        if at very left of the screen go back to the start
        if very bottom of the screen go back to the top*/
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}


/* write char array to the terminal */
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

/*  writestring to the terminal */
void printf(const char* data) {
	terminal_write(data, strlen(data));
}


/* this is our main function well we dont the to name is main the name is what we have declared in boot.s*/
void kernel_main(void) {
	/* Initialize terminal interface */
	terminal_initialize();
 
	/* Newline support is left as an exercise. */
	uint8_t z_color = vga_entry_color(GREEN, WHITE);
	uint8_t e_color = vga_entry_color(RED, LIGHT_BLUE);
	uint8_t r_color = vga_entry_color(BLUE, LIGHT_GREY);
	uint8_t o_color = vga_entry_color(BLACK, LIGHT_CYAN);
	uint8_t t_color = vga_entry_color(MAGENTA, CYAN);
	uint8_t h_color = vga_entry_color(BROWN, LIGHT_GREEN);
	
	terminal_putentryat('Z', z_color, 37, 12);
	terminal_putentryat('E', e_color, 38, 12);
	terminal_putentryat('R', r_color, 39, 12);
	terminal_putentryat('O', o_color, 40, 12);
	terminal_putentryat('T', t_color, 41, 12);
	terminal_putentryat('H', h_color, 42, 12);
}

