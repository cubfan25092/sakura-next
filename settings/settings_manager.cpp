#include "settings_manager.hpp"

using namespace settings;

void settings_manager::update_cache(const std::string &key, const std::string &value)
{
	std::lock_guard<std::mutex> lock(settings_lock);
	logd(log_name, "cache update");
	auto it = cache.find(key);
	if(it != cache.end()){
		cache[key] = value;
	}
}

void settings_manager::init(std::unordered_map<std::string, std::string> &cmdline_opts, const std::string &db_conn_str, bool firstrun)
{
	logd(log_name, "init");
	logd(log_name, db_conn_str);
	try{
		sql.open(db_conn_str);
	} catch(soci::soci_error const &e){
		loge(log_name, "Couldn't connect to settings database!");
		logef(log_name, "Attempted connection string: %s", db_conn_str.c_str());
		logef(log_name, "Result: %s", e.what());
		loge(log_name, "Cannot continue.");
		exit(EXIT_FAILURE);
	}
	if(firstrun){
		logd(log_name, "Creating settings table");
		try{
			//std::lock_guard<std::mutex> lock(settings_lock);
			sql << "create table settings(name VARCHAR(255) PRIMARY KEY NOT NULL, value"
				<< " VARCHAR(255) NOT NULL)";
		} catch(soci::soci_error const &e){
			loge(log_name, "Couldn't create settings database!");
			logef(log_name, "Result: %s", e.what());
			loge(log_name, "Cannot continue.");
			exit(EXIT_FAILURE);
		}
	}
	std::lock_guard<std::mutex> lock(settings_lock);
	cache = std::move(cmdline_opts);
	logi(log_name, "Connected to settings database");
}

std::string settings_manager::get_setting(const std::string &key)
{
	{
		std::lock_guard<std::mutex> lock(settings_lock);
		auto it = cache.find(key);
		if(it != cache.end()){
			logdf(log_name, "Found cached value for %s", key.c_str());
			return cache[key];
		}
	};
	try{
		std::string result;
		sql << "select value from settings where name = :key", soci::into(result), soci::use(key, "key");
		//update the cache
		std::lock_guard<std::mutex> lock(settings_lock);
		logdf(log_name, "caching query for %s", key.c_str());
		cache[key] = result;
		return result;
	} catch(soci::soci_error const &e){
		loge(log_name, "Error in settings database");
		logef(log_name, "Error: %s", e.what());
		return "";
	}
}

bool settings_manager::save_setting(const std::string &key, const std::string &value)
{
	try{
		sql << "replace into settings(name, value) values(:key, :value)", soci::use(key, "key"), soci::use(value, "value") ;
	} catch(soci::soci_error const &e){
		loge(log_name, "Error in settings database");
		logef(log_name, "Error: %s", e.what());
		return false;
	}
	std::lock_guard<std::mutex> lock(settings_lock);
	//update the cache
	logdf(log_name, "caching query for %s", key.c_str());
	cache[key] = value;
	return true; 
}