#include <stdbool.h>

/**
 * Functions to display a selection bar
 */

extern void bar_setup_regs();

/**
 * Clear bar from screen
 */
void bar_clear(bool old);

/**
 * Set bar color
 */
void bar_set_color(unsigned char c);

/**
 * Get current bar position
 * @return bar index
 */
unsigned char bar_get();

void bar_up();
void bar_down();


/**
 * Update bar display
 */
void bar_update(void);

/**
 * Show bar at Y position
 */
void fastcall bar_show(unsigned char y);
