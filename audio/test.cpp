#include <string>
#include <sstream>
#include <unordered_map>

#include "audio_scanner.hpp"
#include "../settings/settings_manager.hpp"
#include "../utils/log/logger.hpp"


int main(int argc, char const *argv[])
{
	utils::log::logger::get_instance().set_log_file("test.log", true);
	utils::log::logger::get_instance().write_console(true);
	utils::log::logger::get_instance().set_priority(utils::log::priority::ALL);
	settings::settings_manager s;
	std::unordered_map<std::string, std::string>c;
	std::stringstream ss;
	ss << media::audio::audio_scanner::scan_types::AVERAGE;
	s.init(c, "sqlite3://test.db", true);
	s.save_setting("audio.scan_type", ss.str());

	media::audio::audio_scanner::get_instance().add_scan_dir("/home/kalmiya/Music/");
	media::audio::audio_scanner::get_instance().scan(s);
	return 0;
}