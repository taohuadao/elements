/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)

   Key mapping ported to C++ from GLFW3

   Copyright (c) 2009-2016 Camilla Berglund <elmindreda@glfw.org>

   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would
      be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not
      be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
      distribution.
=============================================================================*/
#include <elements/base_view.hpp>
#include <elements/support/canvas.hpp>
#include <elements/support/resource_paths.hpp>
#include <cairo.h>
#include <chrono>
#include <map>
#include <SDL3/SDL.h>

#include <unordered_map>

namespace cycfi {
    namespace elements {

        key_code translate_key(const SDL_Event& event);



        namespace {
            constexpr unsigned IDT_TIMER1 = 100;
//            HCURSOR current_cursor = nullptr;

            struct view_info {
                using time_point = std::chrono::time_point<std::chrono::steady_clock>;
                using key_map = std::map<key_code, key_action>;

                base_view *vptr = nullptr;
                bool is_dragging = false;
                int w = 0;
                int h = 0;
                bool mouse_in_window = false;
                time_point click_start = {};
                int click_count = 0;
                time_point scroll_start = {};
                double velocity = 1.0;
                point scroll_dir;
                key_map keys = {};

                SDL_Renderer *renderer = nullptr;
                SDL_Texture *render_target = nullptr;
                cairo_surface_t *surface = nullptr;
                cairo_t *context = nullptr;
            };
            std::unordered_map<void *, view_info *> ViewInfoMaps;

            view_info *get_view_info(void *wnd) {

                auto iter = ViewInfoMaps.find(wnd);
                if (ViewInfoMaps.find(wnd) == ViewInfoMaps.end()) {
                    return nullptr;
                }
                return iter->second;
            }

            void on_cursor(SDL_Window *window, base_view *view,const SDL_Event& event, cursor_tracking state) {
                float pos_x = event.motion.x;
                float pos_y = event.motion.y;
                view->cursor({pos_x, pos_y}, state);
            }

            void on_scroll(SDL_Window *window,  view_info* info,const SDL_Event& event, point dir)
            {
                constexpr auto acceleration = 1.1;
                auto now = std::chrono::steady_clock::now();
                auto elapsed = now - info->scroll_start;
                info->scroll_start = now;

                std::chrono::duration<double, std::milli> fp_ms = elapsed;

                bool reset_accel =
                        elapsed > std::chrono::milliseconds(250) ||
                        (info->scroll_dir.x > 0 != dir.x > 0) ||
                        (info->scroll_dir.y > 0 != dir.y > 0)
                ;
                info->scroll_dir = dir;

                if (reset_accel)
                    info->velocity = 1.0;
                else
                    info->velocity *= acceleration;

                dir.x *= info->velocity;
                dir.y *= info->velocity;

                point pos;
                pos.x = event.wheel.mouseX;
                pos.y = event.wheel.mouseY;


                float scale = 1.0f;
                info->vptr->scroll(dir, { pos.x / scale, pos.y / scale });
            }

            bool on_paint(SDL_Window *window, view_info *info) {
                if (base_view *view = info->vptr) {

                    if(!info->renderer){
                        info->renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED);
                    }

                    int w, h;
                    SDL_GetWindowSize(window, &w, &h);
                    if(w > info->w || h > info->h){
                        if(info->context){
                            cairo_destroy(info->context);
                        }
                        if(info->surface){
                            cairo_surface_destroy(info->surface);
                        }
                        if(info->render_target){
                            SDL_DestroyTexture(info->render_target);
                        }
                        info->render_target = SDL_CreateTexture(info->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
                        info->surface = cairo_image_surface_create_for_data((unsigned char *) SDL_GetWindowSurface(window)->pixels, CAIRO_FORMAT_ARGB32, w, h,SDL_GetWindowSurface(window)->pitch);
                        info->context = cairo_create(info->surface);
                    }

                    view->draw(info->context,rect(0,0,w,h));

                    void *pixels;
                    int pitch;
                    SDL_LockTexture(info->render_target, NULL, &pixels, &pitch);
                    for (int i = 0; i < h; ++i) {
                        memcpy((char *) pixels + pitch * i, (char *) SDL_GetWindowSurface(window)->pixels + w * 4 * i, w * 4);
                    }

                    SDL_UnlockTexture(info->render_target);
                    SDL_RenderClear(info->renderer);
                    SDL_RenderTexture(info->renderer, info->render_target, NULL, NULL);
                    SDL_RenderPresent(info->renderer);

                }//
                return 0;
            }

            int get_mods() {
                int mods = 0;

                const uint8_t *currentKeyStates = SDL_GetKeyboardState(NULL);
                if (currentKeyStates[SDL_SCANCODE_LSHIFT] || currentKeyStates[SDL_SCANCODE_RSHIFT]) {
                    // Shift键当前被按下
                    mods |= mod_shift;
                }
                if (currentKeyStates[SDL_SCANCODE_LCTRL] || currentKeyStates[SDL_SCANCODE_RCTRL]) {
                    // CTRL键当前被按下
                    mods |= mod_control | mod_action;
                }
                if (currentKeyStates[SDL_SCANCODE_MENU]) {
                    // ALT键当前被按下
                    mods |= mod_alt;
                }
                if (currentKeyStates[SDL_SCANCODE_LGUI] || currentKeyStates[SDL_SCANCODE_RGUI]) {
                    // Win键当前被按下
                    mods |= mod_super;
                }

                return mods;
            }

            mouse_button get_button(SDL_Window *window, view_info *info, const SDL_Event &event) {
                float pos_x = event.button.x;
                float pos_y = event.button.y;

                bool down = info->is_dragging;
                switch (event.type) {
                    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                        auto now = std::chrono::steady_clock::now();
                        auto elapsed = now - info->click_start;
                        info->click_start = now;
                        if (elapsed > std::chrono::milliseconds(200)) {
                            info->click_count = 1;
                        } else {
                            ++info->click_count;
                        }

                        if (!info->is_dragging) {
                            info->is_dragging = true;
                            // SetCapture(hwnd);
                        }
                        down = true;
                    }
                        break;

                    case SDL_EVENT_MOUSE_BUTTON_UP: {
                        down = false;
                        if (info->is_dragging) {
                            info->is_dragging = false;
                            //ReleaseCapture();
                        }
                        break;
                    }
                }

                auto const which =
                        [&event]() {
                            switch (event.button.button) {
                                case SDL_BUTTON_LEFT:
                                    return mouse_button::left;

                                case SDL_BUTTON_MIDDLE:
                                    return mouse_button::middle;

                                case SDL_BUTTON_RIGHT:
                                    return mouse_button::right;

                                default:
                                    return mouse_button::left;
                            }
                        }();

                return {
                        down,
                        info->click_count,
                        which,
                        get_mods(),
                        {pos_x, pos_y}
                };
            }

            bool handle_key(base_view& _view, view_info::key_map& keys, key_info k)
            {
                bool repeated = false;

                if (k.action == key_action::release
                    && keys[k.key] == key_action::release)
                    return false;

                if (k.action == key_action::press
                    && keys[k.key] == key_action::press)
                    repeated = true;

                keys[k.key] = k.action;

                if (repeated)
                    k.action = key_action::repeat;

                return _view.key(k);
            }

            bool on_key(SDL_Window *window, view_info* info, const SDL_Event &event)
            {
                auto const key = translate_key(event);
                auto const action = event.type == SDL_EVENT_KEY_UP? key_action::release : key_action::press;
                auto const mods = get_mods();

                if (key == key_code::unknown)
                    return false;

                if (action == key_action::release && (key == key_code::left_shift || key == key_code::right_shift))
                {
                    // HACK: Release both Shift keys on Shift up event, as when both
                    //       are pressed the first release does not emit any event
                    bool r1 = handle_key(*info->vptr, info->keys, { key_code::left_shift, action, mods });
                    bool r2 = handle_key(*info->vptr, info->keys, { key_code::right_shift, action, mods });
                    return r1 || r2;
                }
                else if (key == key_code::print_screen)
                {
                    // HACK: Key down is not reported for the Print Screen key
                    bool r1 = handle_key(*info->vptr, info->keys, { key, key_action::press, mods });
                    bool r2 = handle_key(*info->vptr, info->keys, { key, key_action::release, mods });
                    return r1 || r2;
                }

                return handle_key(*info->vptr, info->keys, { key, action, mods });
            }

            bool on_text(SDL_Window *window, view_info* info, const SDL_Event &event)
            {
                Uint32* codes = SDL_iconv_utf8_ucs4(event.text.text);
                for(int i = 0;i < SDL_TEXTINPUTEVENT_TEXT_SIZE;++i){
                    if(!codes[i]){
                        break;
                    }
                    if(event.text.text[i]){
                        info->vptr->text({ codes[i], get_mods() });
                    }
                }
                return true;

            }

            SDL_Window* GetWindowFromEvent(const SDL_Event& event){
                SDL_WindowID windowId = 0;
                switch (event.type) {
                    case SDL_EVENT_WINDOW_SHOWN:
                    case SDL_EVENT_WINDOW_HIDDEN:
                    case SDL_EVENT_WINDOW_EXPOSED:
                    case SDL_EVENT_WINDOW_MOVED:
                    case SDL_EVENT_WINDOW_RESIZED:
                    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    case SDL_EVENT_WINDOW_MINIMIZED:
                    case SDL_EVENT_WINDOW_MAXIMIZED:
                    case SDL_EVENT_WINDOW_RESTORED:
                    case SDL_EVENT_WINDOW_MOUSE_ENTER:
                    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                    case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    case SDL_EVENT_WINDOW_FOCUS_LOST:
                    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    case SDL_EVENT_WINDOW_HIT_TEST:
                    case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
                    case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
                        //
                    case SDL_EVENT_KEY_DOWN:
                    case SDL_EVENT_KEY_UP:
                    case SDL_EVENT_TEXT_EDITING:
                    case SDL_EVENT_TEXT_INPUT:
                    case SDL_EVENT_KEYMAP_CHANGED:
                    case SDL_EVENT_TEXT_EDITING_EXT:
                    //
                    case SDL_EVENT_MOUSE_MOTION:
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    case SDL_EVENT_MOUSE_BUTTON_UP:
                    case SDL_EVENT_MOUSE_WHEEL:
                    {

                        windowId = event.window.windowID;
                        break;
                    }

                    case SDL_EVENT_JOYSTICK_AXIS_MOTION :
                    case SDL_EVENT_JOYSTICK_HAT_MOTION :
                    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
                    case SDL_EVENT_JOYSTICK_BUTTON_UP:
                    case SDL_EVENT_JOYSTICK_ADDED:
                    case SDL_EVENT_JOYSTICK_REMOVED:
                    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:{
                        break;
                    }

                    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                    case SDL_EVENT_GAMEPAD_BUTTON_UP:
                    case SDL_EVENT_GAMEPAD_ADDED:
                    case SDL_EVENT_GAMEPAD_REMOVED:
                    case SDL_EVENT_GAMEPAD_REMAPPED:
                    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
                    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
                    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
                    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:{
                        break;
                    }

                    case SDL_EVENT_FINGER_DOWN:
                    case SDL_EVENT_FINGER_UP:
                    case SDL_EVENT_FINGER_MOTION:{
                        break;
                    }

                    case SDL_EVENT_CLIPBOARD_UPDATE:{
                        break;
                    };

                    case SDL_EVENT_DROP_FILE:
                    case SDL_EVENT_DROP_TEXT:
                    case SDL_EVENT_DROP_BEGIN:
                    case SDL_EVENT_DROP_COMPLETE:
                    case SDL_EVENT_DROP_POSITION:{
                        break;
                    }

                    case SDL_EVENT_AUDIO_DEVICE_ADDED:
                    case SDL_EVENT_AUDIO_DEVICE_REMOVED:{
                        break;
                    }

                    case SDL_EVENT_SENSOR_UPDATE:{
                        break;
                    }

                    case SDL_EVENT_RENDER_TARGETS_RESET:
                    case SDL_EVENT_RENDER_DEVICE_RESET:{
                        break;
                    }
                }

                if( windowId){
                    return SDL_GetWindowFromID(windowId);
                }
                return nullptr;
            }


        }

        bool DoEvent(const SDL_Event &event) {
            SDL_Window* window = GetWindowFromEvent(event);
            while(window){
                view_info* viewInfo = ViewInfoMaps[window];
                if(!viewInfo){
                    break;
                }
                switch (event.type) {

                    case SDL_EVENT_WINDOW_SHOWN:{break;}
                    case SDL_EVENT_WINDOW_HIDDEN:{break;}
                    case SDL_EVENT_WINDOW_EXPOSED:{
                        on_paint(window,viewInfo);
                        break;
                    }
                    case SDL_EVENT_WINDOW_MOVED:{
                        break;
                    }
                    case SDL_EVENT_WINDOW_RESIZED:{break;}
                    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:{break;}
                    case SDL_EVENT_WINDOW_MINIMIZED:{break;}
                    case SDL_EVENT_WINDOW_MAXIMIZED:{break;}
                    case SDL_EVENT_WINDOW_RESTORED:{break;}
                    case SDL_EVENT_WINDOW_MOUSE_ENTER:{
                        float pos_x,pos_y;
                        SDL_GetMouseState(&pos_x,&pos_y);
                        viewInfo->vptr->cursor({pos_x, pos_y}, cursor_tracking::entering);
                        viewInfo->mouse_in_window = true;
                        break;
                    }
                    case SDL_EVENT_WINDOW_MOUSE_LEAVE:{
                        float pos_x,pos_y;
                        SDL_GetMouseState(&pos_x,&pos_y);
                        viewInfo->vptr->cursor({pos_x, pos_y}, cursor_tracking::entering);
                        viewInfo->mouse_in_window = false;
                        break;
                    }
                    case SDL_EVENT_WINDOW_FOCUS_GAINED:{break;}
                    case SDL_EVENT_WINDOW_FOCUS_LOST:{break;}
                    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:{break;}
                    case SDL_EVENT_WINDOW_HIT_TEST:{break;}
                    case SDL_EVENT_WINDOW_ICCPROF_CHANGED:{break;}
                    case SDL_EVENT_WINDOW_DISPLAY_CHANGED:{break;}


                    case SDL_EVENT_KEY_DOWN:{on_key(window,viewInfo,event);break;}
                    case SDL_EVENT_KEY_UP:{on_key(window,viewInfo,event);break;}
                    case SDL_EVENT_TEXT_EDITING:{break;}
                    case SDL_EVENT_TEXT_INPUT:{
                        on_text(window,viewInfo,event);
                        ;break;}
                    case SDL_EVENT_KEYMAP_CHANGED:{break;}
                    case SDL_EVENT_TEXT_EDITING_EXT:{break;}
                        //
                    case SDL_EVENT_MOUSE_MOTION:{
                        if (viewInfo->is_dragging)
                        {
                            viewInfo->vptr->drag(get_button(window, viewInfo, event));
                        }
                        else
                        {
                            if (!viewInfo->mouse_in_window)
                            {
                                on_cursor(window, viewInfo->vptr, event, cursor_tracking::entering);
                                viewInfo->mouse_in_window = true;
                            }else{
                                on_cursor(window, viewInfo->vptr, event, cursor_tracking::hovering);
                            }
                        }
                        break;
                    }
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:{  viewInfo->vptr->click(get_button(window, viewInfo, event));;break;}
                    case SDL_EVENT_MOUSE_BUTTON_UP:{ viewInfo->vptr->click(get_button(window, viewInfo, event));break;}
                    case SDL_EVENT_MOUSE_WHEEL:{
                        on_scroll(window, viewInfo, event, { 0, event.wheel.y});
                        break;
                    }


                    case SDL_EVENT_JOYSTICK_AXIS_MOTION :{break;}
                    case SDL_EVENT_JOYSTICK_HAT_MOTION :{break;}
                    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:{break;}
                    case SDL_EVENT_JOYSTICK_BUTTON_UP:{break;}
                    case SDL_EVENT_JOYSTICK_ADDED:{break;}
                    case SDL_EVENT_JOYSTICK_REMOVED:{break;}
                    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:{break;}

                    case SDL_EVENT_GAMEPAD_AXIS_MOTION:{break;}
                    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:{break;}
                    case SDL_EVENT_GAMEPAD_BUTTON_UP:{break;}
                    case SDL_EVENT_GAMEPAD_ADDED:{break;}
                    case SDL_EVENT_GAMEPAD_REMOVED:{break;}
                    case SDL_EVENT_GAMEPAD_REMAPPED:{break;}
                    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:{break;}
                    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:{break;}
                    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:{break;}
                    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:{break;}

                    case SDL_EVENT_FINGER_DOWN:{break;}
                    case SDL_EVENT_FINGER_UP:{break;}
                    case SDL_EVENT_FINGER_MOTION:{break;}

                    case SDL_EVENT_CLIPBOARD_UPDATE:{break;};

                    case SDL_EVENT_DROP_FILE:{break;}
                    case SDL_EVENT_DROP_TEXT:{break;}
                    case SDL_EVENT_DROP_BEGIN:{break;}
                    case SDL_EVENT_DROP_COMPLETE:{break;}
                    case SDL_EVENT_DROP_POSITION:{break;}

                    case SDL_EVENT_AUDIO_DEVICE_ADDED:{break;}
                    case SDL_EVENT_AUDIO_DEVICE_REMOVED:{break;}

                    case SDL_EVENT_SENSOR_UPDATE:{break;}

                    case SDL_EVENT_RENDER_TARGETS_RESET:{break;}
                    case SDL_EVENT_RENDER_DEVICE_RESET:{break;}

                }
                on_paint(window,viewInfo);
                return false;
            }




        }

        namespace {
            void *make_window(base_view *_this, host_window_handle host, rect bounds) {

                if(!host){
                    SDL_Window* view = SDL_CreateWindow("ElementsView", 0, 0, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
                    SDL_SetWindowPosition(view, bounds.left, bounds.top);
                    SDL_SetWindowSize(view, bounds.right - bounds.left, bounds.bottom - bounds.top);
                    host = view;
                }

                view_info *info = new view_info{_this};
                ViewInfoMaps.insert(std::make_pair(host,info));

                return host;
            }
        }

        base_view::base_view(extent size_) {
            rect bounds = {0, 0, (size_.x), (size_.y)};
            _view = make_window(this, nullptr, bounds);
        }

        base_view::base_view(host_window_handle window) {
            rect bounds;
            int x, y;
            SDL_GetWindowPosition((SDL_Window *) window, &x, &y);
            int w, h;
            SDL_GetWindowSize((SDL_Window *) window, &w, &h);
            bounds.left = x;
            bounds.top = y;
            bounds.right = x + w;
            bounds.bottom = y + h;
            _view = make_window(this, window, bounds);
        }

        base_view::~base_view() {
            ViewInfoMaps.erase(_view);

        }

        point base_view::cursor_pos() const {

            float x;
            float y;
            SDL_GetMouseState(&x,&y);
            return {x,y};
        }

        elements::extent base_view::size() const {
            int w, h;
            SDL_GetWindowSize((SDL_Window *) _view, &w, &h);
            return point(w, h);
        }

        void base_view::size(elements::extent p) {
            SDL_SetWindowSize((SDL_Window *) _view, p.x, p.y);
        }

        void base_view::refresh() {
            view_info* viewInfo = ViewInfoMaps[_view];
            if(!viewInfo){
                return;
            }
            on_paint((SDL_Window*)_view,viewInfo);
        }

        void base_view::refresh(rect area) {
            refresh();
        }

        float base_view::hdpi_scale() const {
            return 1.0f;
        }

        std::string clipboard() {
            return  SDL_GetClipboardText();
        }

        void clipboard(std::string const &text) {
            SDL_SetClipboardText(text.c_str());
        }

        void set_cursor(cursor_type type) {
            struct cursors {
                cursors() {
                    _cursors[cursor_type::arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
                    _cursors[cursor_type::ibeam] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
                    _cursors[cursor_type::cross_hair] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
                    _cursors[cursor_type::hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
                    _cursors[cursor_type::h_resize] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
                    _cursors[cursor_type::v_resize] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
                }

                std::map<cursor_type, SDL_Cursor*> _cursors;
            };
            static cursors data;

            SDL_Cursor* cursor = data._cursors[type];
            SDL_SetCursor(cursor);
        }

        point scroll_direction() {
            return {+1.0f, +1.0f};
        }
    }
}

