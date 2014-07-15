#include <iostream>
#include <unordered_map>
#include "../db_utils.hpp"
#include "../../file/file_utils.hpp"
#include "../../log/logger.hpp"



int main(int argc, char const *argv[])
{
	utils::log::logger::get_instance().set_priority(utils::log::priority::ALL);
	utils::log::logger::get_instance().set_log_file("testlog.txt", true);
	std::unordered_map<std::string, std::string> dbinfo;
	dbinfo["sqlite3_path"] = utils::file::get_sakura_home_dir() + "sakura.db";
	std::cout << utils::db::build_db_connstr(dbinfo, true) << std::endl;
	dbinfo["type"] = "mysql";
	dbinfo["mysql_db_name"] = "sakura";
	dbinfo["mysql_user"] = "sakurauser";
	dbinfo["mysql_pass"] = "sakurapass";
	std::cout << utils::db::build_db_connstr(dbinfo, true) << std::endl;
	//test sock case
	dbinfo["mysql_sock"] = "/test/sock.test";
	std::cout << utils::db::build_db_connstr(dbinfo, true) << std::endl;
	dbinfo["mysql_external_server"] = "true";
	dbinfo["mysql_host"] = "herp.derp";
	dbinfo["mysql_port"] = "4096";
	std::cout << utils::db::build_db_connstr(dbinfo, true) << std::endl;
	dbinfo["mysql_use_ssl"] = "true";
	dbinfo["mysql_sslca"] = "derpcorp";
	dbinfo["mysql_cert_path"] = "/test/derpcorp/derp.cert";
	std::cout << utils::db::build_db_connstr(dbinfo, true) << std::endl;
	return 0;
}