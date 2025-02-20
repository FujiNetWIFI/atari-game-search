#ifndef BAR_H
#define BAR_H

#include <stdbool.h>

extern unsigned char bar_y;

/**
 * Functions to display a selection bar
 */

extern void bar_setup_regs();

/**
 * Clear bar from screen
 */
void bar_clear();

/**
 * Get current bar position
 * @return bar index
 */
unsigned char bar_get();

void bar_up();
void bar_down();


/**
 * Show bar at Y position
 */
void fastcall bar_show(unsigned char y);

#endif
