#include <iostream>
#include <unordered_map>
#include "../../utils/db/db_utils.hpp"
#include "../../utils/log/logger.hpp"
#include "../settings_manager.hpp"

int main(int argc, char const *argv[])
{
	utils::log::logger::get_instance().set_priority(utils::log::priority::ALL);
	utils::log::logger::get_instance().set_log_file("testlog.txt", true);
	settings::settings_manager s;
	std::unordered_map<std::string, std::string> icache;
	std::unordered_map<std::string, std::string> dbinfo;
	dbinfo["sqlite_dbpath"] = "testsettings.db";
	s.init(icache, utils::db::build_db_connstr(dbinfo, false), true);
	s.save_setting("test.setting0", "4096");
	std::cout << s.get_setting("test.setting0") << std::endl;
	return 0;
}