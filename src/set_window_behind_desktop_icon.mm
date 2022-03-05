#include <AppKit/AppKit.h>

#include "set_window_behind_desktop_icon.h"

void set_window_behind_desktop_icon(void *window) {
  NSWindow *win = (NSWindow *)window;
  win.level = -2147483623;
}
