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
#include <SDL3/SDL.h>

namespace cycfi { namespace elements
{
   key_code translate_key(const SDL_Event& event)
   {
      switch (event.key.keysym.scancode)
      {
         default:    return key_code::unknown;

         case SDL_SCANCODE_0:  return key_code:: _0;
         case SDL_SCANCODE_1:  return key_code:: _1;
         case SDL_SCANCODE_2:  return key_code:: _2;
         case SDL_SCANCODE_3:  return key_code:: _3;
         case SDL_SCANCODE_4:  return key_code:: _4;
         case SDL_SCANCODE_5:  return key_code:: _5;
         case SDL_SCANCODE_6:  return key_code:: _6;
         case SDL_SCANCODE_7:  return key_code:: _7;
         case SDL_SCANCODE_8:  return key_code:: _8;
         case SDL_SCANCODE_9:  return key_code:: _9;

         case SDL_SCANCODE_A:  return key_code::a;
         case SDL_SCANCODE_B:  return key_code::b;
         case SDL_SCANCODE_C:  return key_code::c;
         case SDL_SCANCODE_D:  return key_code::d;
         case SDL_SCANCODE_E:  return key_code::e;
         case SDL_SCANCODE_F:  return key_code::f;
         case SDL_SCANCODE_G:  return key_code::g;
         case SDL_SCANCODE_H:  return key_code::h;
         case SDL_SCANCODE_I:  return key_code::i;
         case SDL_SCANCODE_J:  return key_code::j;
         case SDL_SCANCODE_K:  return key_code::k;
         case SDL_SCANCODE_L:  return key_code::l;
         case SDL_SCANCODE_M:  return key_code::m;
         case SDL_SCANCODE_N:  return key_code::n;
         case SDL_SCANCODE_O:  return key_code::o;
         case SDL_SCANCODE_P:  return key_code::p;
         case SDL_SCANCODE_Q:  return key_code::q;
         case SDL_SCANCODE_R:  return key_code::r;
         case SDL_SCANCODE_S:  return key_code::s;
         case SDL_SCANCODE_T:  return key_code::t;
         case SDL_SCANCODE_U:  return key_code::u;
         case SDL_SCANCODE_V:  return key_code::v;
         case SDL_SCANCODE_W:  return key_code::w;
         case SDL_SCANCODE_X:  return key_code::x;
         case SDL_SCANCODE_Y:  return key_code::y;
         case SDL_SCANCODE_Z:  return key_code::z;

         case SDL_SCANCODE_APOSTROPHE:  return key_code::apostrophe;
         case SDL_SCANCODE_BACKSLASH:  return key_code::backslash;
         case SDL_SCANCODE_COMMA:  return key_code::comma;
         case SDL_SCANCODE_EQUALS:  return key_code::equal;
         case SDL_SCANCODE_GRAVE:  return key_code::grave_accent;
         case SDL_SCANCODE_LEFTBRACKET:  return key_code::left_bracket;
         case SDL_SCANCODE_MINUS:  return key_code::minus;
         case SDL_SCANCODE_PERIOD:  return key_code::period;
         case SDL_SCANCODE_RIGHTBRACKET:  return key_code::right_bracket;
         case SDL_SCANCODE_SEMICOLON:  return key_code::semicolon;
         case SDL_SCANCODE_SLASH:  return key_code::slash;
//         case 0x056:  return key_code::world_2;

         case SDL_SCANCODE_BACKSPACE:  return key_code::backspace;
         case SDL_SCANCODE_DELETE:  return key_code::_delete;
         case SDL_SCANCODE_END:  return key_code::end;
         case SDL_SCANCODE_RETURN:  return key_code::enter;
         case SDL_SCANCODE_ESCAPE:  return key_code::escape;
         case SDL_SCANCODE_HOME:  return key_code::home;
         case SDL_SCANCODE_INSERT:  return key_code::insert;
         case SDL_SCANCODE_MENU:  return key_code::menu;
         case SDL_SCANCODE_PAGEDOWN:  return key_code::page_down;
         case SDL_SCANCODE_PAGEUP:  return key_code::page_up;
         case SDL_SCANCODE_PAUSE:  return key_code::pause;
//         case SDL_SCANCODE_PAUSE:  return key_code::pause;
         case SDL_SCANCODE_SPACE:  return key_code::space;
         case SDL_SCANCODE_TAB:  return key_code::tab;
         case SDL_SCANCODE_CAPSLOCK:  return key_code::caps_lock;
         case SDL_SCANCODE_NUMLOCKCLEAR:  return key_code::num_lock;
         case SDL_SCANCODE_SCROLLLOCK:  return key_code::scroll_lock;

         case SDL_SCANCODE_F1:  return key_code::f1;
         case SDL_SCANCODE_F2:  return key_code::f2;
         case SDL_SCANCODE_F3:  return key_code::f3;
         case SDL_SCANCODE_F4:  return key_code::f4;
         case SDL_SCANCODE_F5:  return key_code::f5;
         case SDL_SCANCODE_F6:  return key_code::f6;
         case SDL_SCANCODE_F7:  return key_code::f7;
         case SDL_SCANCODE_F8:  return key_code::f8;
         case SDL_SCANCODE_F9:  return key_code::f9;
         case SDL_SCANCODE_F10:  return key_code::f10;
         case SDL_SCANCODE_F11:  return key_code::f11;
         case SDL_SCANCODE_F12:  return key_code::f12;
         case SDL_SCANCODE_F13:  return key_code::f13;
         case SDL_SCANCODE_F14:  return key_code::f14;
         case SDL_SCANCODE_F15:  return key_code::f15;
         case SDL_SCANCODE_F16:  return key_code::f16;
         case SDL_SCANCODE_F17:  return key_code::f17;
         case SDL_SCANCODE_F18:  return key_code::f18;
         case SDL_SCANCODE_F19:  return key_code::f19;
         case SDL_SCANCODE_F20:  return key_code::f20;
         case SDL_SCANCODE_F21:  return key_code::f21;
         case SDL_SCANCODE_F22:  return key_code::f22;
         case SDL_SCANCODE_F23:  return key_code::f23;
         case SDL_SCANCODE_F24:  return key_code::f24;

         case SDL_SCANCODE_LALT:  return key_code::left_alt;
         case SDL_SCANCODE_LCTRL:  return key_code::left_control;
         case SDL_SCANCODE_LSHIFT:  return key_code::left_shift;
//         case 0x15b:  return key_code::left_super;
         case SDL_SCANCODE_PRINTSCREEN:  return key_code::print_screen;
         case SDL_SCANCODE_RALT:  return key_code::right_alt;
         case SDL_SCANCODE_RCTRL:  return key_code::right_control;
         case SDL_SCANCODE_RSHIFT:  return key_code::right_shift;
//         case 0x15c:  return key_code::right_super;
         case SDL_SCANCODE_DOWN:  return key_code::down;
         case SDL_SCANCODE_LEFT:  return key_code::left;
         case SDL_SCANCODE_RIGHT:  return key_code::right;
         case SDL_SCANCODE_UP:  return key_code::up;

         case SDL_SCANCODE_KP_0:  return key_code::kp_0;
         case SDL_SCANCODE_KP_1:  return key_code::kp_1;
         case SDL_SCANCODE_KP_2:  return key_code::kp_2;
         case SDL_SCANCODE_KP_3:  return key_code::kp_3;
         case SDL_SCANCODE_KP_4:  return key_code::kp_4;
         case SDL_SCANCODE_KP_5:  return key_code::kp_5;
         case SDL_SCANCODE_KP_6:  return key_code::kp_6;
         case SDL_SCANCODE_KP_7:  return key_code::kp_7;
         case SDL_SCANCODE_KP_8:  return key_code::kp_8;
         case SDL_SCANCODE_KP_9:  return key_code::kp_9;
         case SDL_SCANCODE_KP_PLUS:  return key_code::kp_add;
         case SDL_SCANCODE_KP_DECIMAL:  return key_code::kp_decimal;
         case SDL_SCANCODE_KP_DIVIDE:  return key_code::kp_divide;
         case SDL_SCANCODE_KP_ENTER:  return key_code::kp_enter;
         case SDL_SCANCODE_KP_EQUALS:  return key_code::kp_equal;
         case SDL_SCANCODE_KP_MULTIPLY:  return key_code::kp_multiply;
         case SDL_SCANCODE_KP_MINUS:  return key_code::kp_subtract;
      }
   }
}}
