/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/window.hpp>
#include <elements/support.hpp>
#include <SDL3/SDL.h>


namespace elements = cycfi::elements;

namespace cycfi {
    namespace elements {
        
        window::window(std::string const &name, int style_, rect const &bounds) {

            _window = SDL_CreateWindow(name.c_str(), bounds.width(), bounds.height(), SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
            SDL_ShowWindow((SDL_Window *) _window);
        }

        window::~window() {
            SDL_DestroyWindow((SDL_Window *) _window);
        }

        point window::size() const {
            int w, h;
            SDL_GetWindowSize((SDL_Window *) _window, &w, &h);
            return point(w, h);
        }

        void window::size(point const &p) {
            SDL_SetWindowSize((SDL_Window *) _window, p.x, p.y);
        }

        void window::limits(view_limits limits_) {
            SDL_SetWindowMinimumSize((SDL_Window *) _window, limits_.min.x, limits_.min.y);
            SDL_SetWindowMaximumSize((SDL_Window *) _window, limits_.max.x, limits_.max.y);
        }

        point window::position() const {
            int w, h;
            SDL_GetWindowPosition((SDL_Window *) _window, &w, &h);
            return point(w, h);
        }

        void window::position(point const &p) {
            SDL_SetWindowPosition((SDL_Window *) _window, p.x, p.y);
        }
    }
}

