#pragma once

#include <unordered_map>
#include <string>
#include <sstream>

#include "../log/logger.hpp"


namespace utils
{
	namespace db
	{
		static std::string build_db_connstr(std::unordered_map<std::string, std::string> &dbinfo, bool drop_cache)
		{
			static std::stringstream csstream;
			if(!csstream.str().empty() && !drop_cache){
				return csstream.str();
			}
			if(drop_cache){
				logd("db_utils", "Dropping conn string cache");
				csstream.str(std::string());
			}
			#ifdef SAKURA_USE_MYSQL
			//note we expect every basic mysql field to be defined
			//username, password, dbname, even if theyre empty
			if(dbinfo["type"] == "mysql"){
				logd("db_utils", "Got mysql");
				csstream << "mysql://db=" << dbinfo["mysql_db_name"] << " user="
					<< dbinfo["mysql_user"] << " pass=\'" << dbinfo["mysql_pass"]
					<< "\'";
					if(dbinfo["mysql_external_server"] == "true"){
						logd("db_utils", "using a mysql server");
						csstream << " host=" << dbinfo["mysql_host"] << " port="
							<< dbinfo["mysql_port"];
						if(dbinfo.find("mysql_sock") != dbinfo.end()){
							logd("db_utils", "using a custom socket for nonlocal mysql database");
							csstream << " unix_socket=" << dbinfo["mysql_sock"];
						}
						if(dbinfo["mysql_use_ssl"] == "true"){
							logd("db_utils", "using ssl for mysql");
							csstream << " sslca=" << dbinfo["mysql_sslca"]
								<< " sslcert=" << dbinfo["mysql_cert_path"];
						}	
					} else if(dbinfo.find("mysql_sock") != dbinfo.end()){
						logd("db_utils", "using a custom socket for local mysql database");
						csstream << " unix_socket=" << dbinfo["mysql_sock"];
					}

			} else{
				logd("db_utils", "Got sqlite3");
				csstream << "sqlite3://" << dbinfo["sqlite_dbpath"];
			}
			#else
			logd("db_utils", "Got sqlite3");
			csstream << "sqlite3://" << dbinfo["sqlite_dbpath"];
			#endif
			return csstream.str();
		}
	}
}
