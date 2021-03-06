#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
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

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll(){
    for(unsigned int i = 0; i < VGA_HEIGHT; i++){
        for (unsigned int m = 0; m < VGA_WIDTH; m++){
            terminal_buffer[i * VGA_WIDTH + m] = terminal_buffer[(i + 1) * VGA_WIDTH + m];
        }
    }
 //   terminal_row--;
}

void terminal_putchar(char c) {
    unsigned char uc = c;
    if (terminal_row >= VGA_HEIGHT) {
        terminal_scroll();
    }
    if (c == '\n') {
        if (terminal_row++ > VGA_HEIGHT) {
            terminal_scroll();
            return;
        }
        else {
            terminal_column = 0;
            return;
        }
    }
    if (c == '\b') {
        // Test if going back would change row
        if (terminal_column - 1 <= 0) {
            // Set column 0 to ' '
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
            // Move up one row
            terminal_row--;
            // set column to max
            terminal_column = VGA_WIDTH-1;
            }
        terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        terminal_column--;
        return;
    }
	if (++terminal_column == VGA_WIDTH-1) {
		terminal_column = 1;
        terminal_row++;
        if (terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    }
    terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
