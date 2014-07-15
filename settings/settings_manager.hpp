#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <exception>
#include <cstdlib>
#include <soci.h>

#include "../utils/log/logger.hpp"

namespace settings
{
	class settings_manager
	{
	public:
		void init(std::unordered_map<std::string, std::string> &cmdline_opts, const std::string &db_conn_str, bool firstrun);

		std::string get_setting(const std::string &key);;

		bool save_setting(const std::string &key, const std::string &val);

		~settings_manager()
		{
			sql.close();
		}

		void update_cache(const std::string &key, const std::string &val);

	private:
		std::unordered_map<std::string, std::string> cache;
		soci::session sql;
		std::mutex settings_lock;
		const char *log_name = "settings_manager";


	};
}