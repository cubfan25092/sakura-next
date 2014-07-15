#include <string>
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <soci.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef SAKURA_USE_MYSQL
#include <soci-mysql.h>
#endif
#include <soci-sqlite3.h>

#include "../utils/log/logger.hpp"
#include "../utils/file/file_utils.hpp"
#include "../utils/db/db_utils.hpp"
#include "../settings/settings_manager.hpp"


const char *log_name = "Sakura";
bool firstrun = false;

#ifdef SAKURA_USE_MYSQL
void do_mysql_creds(std::fstream &pf, std::unordered_map<std::string, std::string> &dbinfo)
{
	dbinfo["type"] = "mysql";
	bool prompt = true, serve = false, ssl = false, sock = false;
	std::string dbname, user, pass, host, server, port, sslca, sslcert, servestr, sslstr, socktest, sockstr;
	while(prompt){
		std::cout << "Enter the name of the database: ";
		std::getline(std::cin, dbname);
		dbinfo["mysql_db_name"] = dbname; 
		std::cout << "Enter the username: ";
		std::getline(std::cin, user);
		dbinfo["mysql_user"] = user;
		std::cout << "Enter the password: ";
		std::getline(std::cin, pass);
		dbinfo["mysql_pass"] = pass;
		std::cout << "Do you use a custom socket?(y/n)";
		std::getline(std::cin, socktest);
		if(socktest == "y" || socktest == "yes" || socktest == "true"){
			std::cout << "Enter socket: ";
			std::getline(std::cin, sockstr);
			dbinfo["mysql_sock"] = sockstr;
			sock = true;
		}
		std::cout << "Is this hosted on a separate server?(y/n) ";
		std::getline(std::cin, servestr);
		if(servestr == "y" || servestr == "yes" || servestr == "true"){
			serve = true;
			dbinfo["mysql_external_server"] = "true";
		}
		if(serve){
			std::cout << "Enter server address: ";
			std::getline(std::cin, server);
			dbinfo["mysql_host"] = server;
			std::cout << "Enter port: ";
			std::getline(std::cin, port);
			dbinfo["mysql_port"] = port;
			std::cout << "Do you use ssl? (y/n) ";
			std::getline(std::cin, sslstr);
			if(sslstr == "y" || sslstr == "yes" || sslstr == "true"){
				ssl = true;
			}
			if(ssl){
				std::cout << "CA: ";
				std::getline(std::cin, sslca);
				dbinfo["mysql_sslca"] = sslca;
				std::cout << "Cert path: ";
				std::getline(std::cin, sslcert);
				dbinfo["mysql_cert_path"] = sslcert;
			}

		}
		std::cout << "Attempting login..." << std::endl;
		std::string connstr = utils::db::build_db_connstr(dbinfo, false);
		try{
			soci::session sq(connstr);
			prompt = false;
		} catch(soci::mysql_soci_error const &e){
			std::cout << e.what() << std::endl;
			prompt = true;
		} catch(soci::soci_error const &e){
			std::cout << e.what() << std::endl;
			prompt = true;
		}

	}
	std::cout << "Login OK!" << std::endl << "Storing Credentials";
	pf << "type:mysql" << std::endl << "mysql_db_name:" << dbname << std::endl << "mysql_user:" << user << std::endl
		<< "mysql_pass:" << pass;
	if(serve){
		pf << "mysql_server:" << server << std::endl << "mysql_port:" << port;
		if(ssl){
			pf << "mysql_sslca:" << sslca << std::endl << "mysql_cert_path:" << sslcert;
		}
	}
	if(sock){
		pf << "mysql_sock:" << sockstr << std::endl;
	}
}
#endif
void print_usage()
{

}

void parse_opts(std::unordered_map<std::string, std::string> &opts, int argc, char* const* argv)
{
	static struct option long_options[] = {
		{"database", required_argument, 0, 'd'},
		{"log_file", required_argument, 0, 'l'},
		{"max_clients", required_argument, 0, 'c'},
		{"max_threads", required_argument, 0, 't'},
		{"scan_interval", required_argument, 0, 'i'},
		{"port", required_argument, 0, 'p'},
		{"https", no_argument, 0, 'h'},
		{"verbose", no_argument, 0, 'v'},
		{"debug", no_argument, 0, 'b'},
		{"sqlite_dbpath", required_argument, 0, 's'},
		{0, 0, 0, 0}
	};
	int long_index = 0, opt = 0;
		while((opt = getopt_long(argc, argv,"d:l:c:t:i:p:s:hvb", long_options, &long_index )) != -1){
			switch(opt){
				case 'd':
					if(!std::strcmp("mysql", optarg)){
						#ifndef SAKURA_USE_MYSQL
						std::cerr << "This was compiled without MySQL support. You cannot use MySQL" << std::endl;
						exit(EXIT_FAILURE);
						#else
						opts["use_mysql"] = "true";
						#endif
					}
					break;
				case 'l':
					opts["log_file"] = optarg;
					break;
				case 'c':
					opts["max_clients"] = optarg;
					break;
				case 't':
					opts["max_threads"] = optarg;
					break;
				case 'i':
					opts["scan_interval"] = optarg;
					break;
				case 'p':
					opts["port"] = optarg;
					break;
				case 'h':
					opts["https"] = true;
					break;
				case 'v':
					utils::log::logger::get_instance().set_priority(utils::log::priority::INFO);
					break;
				case 'b':
					utils::log::logger::get_instance().set_priority(utils::log::priority::ALL);
					break;
				case 's':
					opts["sqlite_dbpath"] = optarg;
					break;
				default:
					print_usage();
					exit(EXIT_FAILURE);
					break;
			}
	}
}


int main(int argc, char *const *argv)
{
	//cmdline args override anything in settings
	std::unordered_map<std::string, std::string> cmdline_args, dbinfo;
	std::string db_conn_str;
	parse_opts(cmdline_args, argc, argv);
	std::string conf = utils::file::get_sakura_home_dir();
	//check if first run by checking if the config dir exists
	if(access(conf.c_str(), F_OK)){
		if(mkdir(conf.c_str(), 0777)){
			std::cout << "Couldnt create config directory. " << conf << " Exiting" << std::endl;
			exit(EXIT_FAILURE);
		}
		//mkdir succeeded, set first run
		firstrun = true;
	}
	//set up the log
	std::string lstr = conf + "curr.log";
	utils::log::logger::get_instance().set_log_file(lstr, false);
	std::string pfname = conf + "sakura_preinit.conf";
	std::fstream persist_file(pfname, std::ios::out | std::ios::in | std::ios::app);
	if(firstrun){
		logi(log_name, "It looks like this is the first time Sakura has been run. Setting up defaults.");
		//do firstrun tasks
		if(cmdline_args["use_mysql"] == "true"){
			#ifdef SAKURA_USE_MYSQL
			std::cout << "You are using MySQL but there is no credentials file. Please enter your credentials" << std::endl;
			do_mysql_creds(persist_file, dbinfo);
			#endif
		} else{
			if(cmdline_args.find("sqlite_dbpath") != cmdline_args.end()){
				persist_file <<"type:sqlite3" << std::endl << "sqlite_dbpath:" << cmdline_args["sqlite_dbpath"];
			} else{
				persist_file <<"type:sqlite3" << std::endl << "sqlite_dbpath:sakura.db";
			}
			persist_file.flush();
		}
		db_conn_str = utils::db::build_db_connstr(dbinfo, false);
		//create the settings table, it doesnt exist yet
		settings::settings_manager s;
		s.init(cmdline_args, db_conn_str, true);
		//settings have been initialized
	} else{
		//read the config file to fill out dbinfo
		while(!persist_file.eof()){
			std::string key, value, line;
			std::getline(persist_file, line);
			int split = line.find(':');
			if(split == std::string::npos){
				logef(log_name, "Error parsing preinit file. Cannot continue");
				exit(EXIT_FAILURE);
			}
			key = line.substr(0, split);
			value = line.substr(split + 1, std::string::npos);
			if(key == "sqlite_dbpath"){
				value = conf + value;
			}
			dbinfo[key] = value;
		}
		logi(log_name, "Finished Reading config file. Initializing threads");
		//dbinfo is filled out. we can make the string now
		db_conn_str = utils::db::build_db_connstr(dbinfo, true);
	}
	//ok. start threads and stuff
	//for now, while prealpha, main gets its own copy of thread's objects
	//and just runs a repl
	settings::settings_manager s;
	s.init(cmdline_args, db_conn_str, false);
	while(true){
		std::string str;
		std::cout << "sakura>";
		std::getline(std::cin, str);

	}
	return 0;
}