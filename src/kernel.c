#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#endif

/* Only work for the 32-bit ix86 targets */
#if !defined(__i386__)
#endif

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

/* Keyboard apping */
char keyboard_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a',
    's','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c',
    'v','b','n','m',',','.','/',0,'*',0,' ','\0'
};

/**
 * Read a byte from an I/O port
 * @param port The I/O port number (e.g., 0x60 for keyboard data)
 * @return The byte read from the port
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * Create a VGA color attribute byte from foreground and background colors
 */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

/**
 * Create a VGA text entry combining character and color
 */
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY; // Pointer to VGA memory

/**
 * Initialize the terminal by clearing the screen and resetting cursor
 */
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_LIGHT_BLUE);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

/**
 * Put a character at  position (x,y)
 * @param c The character to display
 * @param color The color attribute to use
 * @param x The column position (0-79)
 * @param y The row position (0-24)
 */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

/**
 * Scroll the terminal content up by one line
 * Moves all lines up and clears the bottom line
 */
void terminal_scroll() 
{
	// Move all lines up by one row
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y - 1) * VGA_WIDTH + x] = terminal_buffer[y * VGA_WIDTH + x];
        }
    }

	// Clear the bottom line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }

	// Set cursor to last line
    terminal_row = VGA_HEIGHT - 1;
}

/**
 * Put a character at the current cursor position
 * @param c The character to display
 */
void terminal_putchar(char c) 
{
    if (c == '\n') {        // New line   
        terminal_column = 0;   
        terminal_row++;   
    } 
    else if (c == '\b') { // Backspace
        if (terminal_column > 0) {
            terminal_column--;
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        }
        return;
    } 
    else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        terminal_column++;
    }

	// New line if the end of the current one is reached
    if (terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
    }

	// Scroll if the bottom is reached
    if (terminal_row >= VGA_HEIGHT) {
        terminal_scroll();
    }
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void kernel_main(void) 
{
	terminal_initialize(); // Initialize the VGA terminal

	terminal_writestring("Hello, Kernel !\n");

	while (1) {
		uint8_t scancode = inb(0x60);

		if(scancode & 0x80) continue;

		// Convert scancode to ASCII
		char c = keyboard_map[scancode];
		if(c != 0) terminal_putchar(c);

		// wait for key release
		while ((inb(0x60) & 0x80) == 0) {}
	}
}