#pragma once

#ifdef HAVE_GLIBMM
#include <glibmm.h>
#else
#include <cstdlib>
#endif
#include <string>

namespace utils
{
	namespace file
	{
		static std::string get_sakura_home_dir()
		{
			std::string s;
			#ifdef HAVE_GLIBMM
			s = Glib::get_user_config_dir();
			#endif
			if(s.empty()){
				char *home = getenv("HOME");
				if(home != nullptr){
					s = home;
					#ifdef __APPLE__
					s.append("/Library/Application Support/sakura/");
					#else
					s.append(".config/sakura/");
					#endif
				} else{
					#ifdef __APPLE__
					s = "/Library/Application Support/sakura/"
					#else
					s = "/var/sakura/";
					#endif
				}
			}
			return s;
		}
	}
}