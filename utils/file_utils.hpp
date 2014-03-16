/**

File: file_utils.hpp
Date: 15 March 2014
Author: cubfan2509
Version History:
				*0.0.1 - Initial version


The MIT License (MIT)

Copyright (c) 2014 cubfan2509

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


Various file utility functions used by the server

**/

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
		/**
		 * gets the directory where we should store all non-cache data
		 * for things that have glib, we can get the xdg directory for this
		 * otherwise, get the home directory, and put it in .config, approximating
		 * xdg defaults.
		 * For OS X, this is well defined to be the Library/Application Support directory
		 * if there is no home (ie, we're runnning as our own user with no home), 
		 * default to var on not os x, and /Library/Application Support on os x
		 **/
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
			} else{
				s.append("sakura");
			}
			return s;
		}
	}
}