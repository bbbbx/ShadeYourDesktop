#pragma once

#if defined(__APPLE__)
    void put_window_behind_desktop_icons(void* window);
#elif defined(_WIN32) || defined(_WIN64)
    void put_window_behind_desktop_icons(HWND window);
#endif
