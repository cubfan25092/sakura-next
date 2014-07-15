#pragma once

#include <unordered_set>
#include <cstdlib>
#include <ftw.h>
#include <fileref.h>


#include "../settings/settings_manager.hpp"
#include "../utils/log/logger.hpp"




namespace media
{
	namespace audio
	{
		class audio_scanner
		{
		public:
			enum scan_types
				{
					ACCURATE = 0,
					AVERAGE = 1,
					FAST = 2
				};

			static audio_scanner &get_instance();

			void scan(settings::settings_manager &s);

			void set_scan_type(int newtype);

			void add_scan_dir(const std::string &path);

			void remove_scan_dir(std::string &path);

		private:
				audio_scanner(){};
				audio_scanner(audio_scanner &copy){};
				void operator=(audio_scanner &rhs);
				audio_scanner(audio_scanner &&other){};
				void operator=(audio_scanner &&rhs);
				static int ftw_func(const char *fpath, const struct stat *sb, const int typeflag);
				std::unordered_set<std::string> dirs;
				static const char * log_name;
				//type of read to perform to determin file values
				static scan_types stype;

		};	
	}
}