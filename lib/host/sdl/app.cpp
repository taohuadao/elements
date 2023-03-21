/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements/app.hpp>
#include <elements/support/font.hpp>
#include <elements/support/resource_paths.hpp>
#include <infra/filesystem.hpp>

#include <SDL3/SDL.h>

#include <cstring>

#ifndef ELEMENTS_HOST_ONLY_WIN7
#include <shellscalingapi.h>
#endif

namespace cycfi { namespace elements
{
   // UTF8 conversion utils defined in base_view.cpp

   // Convert a wide Unicode string to an UTF8 string
   std::string utf8_encode(std::wstring const& wstr);

   // Convert an UTF8 string to a wide Unicode String
   std::wstring utf8_decode(std::string const& str);

   bool DoEvent(const SDL_Event& event);
}}

namespace cycfi { namespace elements
{
   //note to future users, ripped this from linux's implemenation, may not entirely make sense on windows
   fs::path find_resources(const fs::path app_path)
   {
      const fs::path app_dir = app_path.parent_path();

      if (app_dir.filename() == "bin")
      {
         fs::path path = app_dir.parent_path() / "share" / app_path.filename() / "resources";
         if (fs::is_directory(path))
            return path;
      }

      const fs::path app_resources_dir = app_dir / "resources";
      if (fs::is_directory(app_resources_dir))
         return app_resources_dir;

      return fs::current_path() / "resources";
   }

   struct init_app
   {
      init_app(std::string id, const char *program_path)
      {
         const fs::path resources_path = find_resources(program_path);
         font_paths().push_back(resources_path);
         add_search_path(resources_path);
      }
   };

   app::app(
      int         // argc
    , char**      argv
    , std::string name
    , std::string id
   )
   {
      _app_name = name;
      static init_app init{ id, argv[0] };

   }

   app::~app()
   {
   }


   void app::run()
   {
       SDL_Event event;
       while(_running){
           while (SDL_PollEvent(&event)) {
               DoEvent(event);

               switch (event.type) {
                   case SDL_EVENT_QUIT:
                   {
                       _running = false;
                       break;
                   }
               }
           }
           SDL_Delay(1);
       }
   }

   void app::stop()
   {
      _running = false;
   }

   fs::path app_data_path()
   {
      LPWSTR path = nullptr;
      return fs::path{ path };
   }
}}

