#include <AppKit/AppKit.h>

#include "put_window_behind_desktop_icons.h"

void put_window_behind_desktop_icons(void *window) {
  NSWindow *win = (NSWindow *)window;
  win.level = -2147483623;
}
