#ifndef SCRADLE_KEYBOARD_INPUT_H
#define SCRADLE_KEYBOARD_INPUT_H

namespace scradle {

/**
 * Set terminal to non-blocking input mode
 * @param enable If true, enable non-blocking mode; if false, restore original settings
 */
void setNonBlockingInput(bool enable);

/**
 * Check if a key was pressed (non-blocking)
 * @return The character that was pressed, or 0 if no key was pressed
 */
char checkKeyPress();

}  // namespace scradle

#endif  // SCRADLE_KEYBOARD_INPUT_H
