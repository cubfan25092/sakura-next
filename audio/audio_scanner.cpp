#include "audio_scanner.hpp"

using namespace media::audio;


const char *audio_scanner::log_name = "audio_scanner";
audio_scanner::scan_types audio_scanner::stype = audio_scanner::scan_types::AVERAGE;

audio_scanner &audio_scanner::get_instance()
{ 
	static audio_scanner as;
	return as;
}

void audio_scanner::scan(settings::settings_manager &s)
{
	//basic idea here is to apply ftw to every dir in scan dirs
	//then the function used by ftw will check for validity of the file,
	//check if we have seen it already, and if not add it.
	//we also cache the scan type here to avoid the db overhead of doing it
	//for every file in every directory
	std::string result = s.get_setting("audio.scan_type");
	if(!result.empty()){
		set_scan_type(std::atoi(result.c_str()));
	}
	for(auto path : dirs){
		logdf(log_name, "Scanning directory %s", path.c_str());
		ftw(path.c_str(), ftw_func, 128);
	}
}


int audio_scanner::ftw_func(const char *fpath, const struct stat *sb, const int typeflag)
{
	switch(typeflag){
		case FTW_F:
			if(stype == scan_types::ACCURATE){
				TagLib::FileRef f(fpath, true, TagLib::AudioProperties::Accurate);
				if(!f.isNull()){
					logdf(log_name, "Found audio file %s. Reading accurately.", fpath);
				}
				//TODO: call db add on media database
			} else if(stype == scan_types::FAST){
				TagLib::FileRef f(fpath, true, TagLib::AudioProperties::Fast);
				if(!f.isNull()){
					logdf(log_name, "Found audio file %s. Reading fast. Values may be inaccurate", fpath);
				}
				//TODO: call db add on media database
			} else{
				TagLib::FileRef f(fpath, true, TagLib::AudioProperties::Average);
				if(!f.isNull()){
					logdf(log_name, "Found audio file %s. Reading average speed.", fpath);
				}
				//TODO: call db add on media database
			}
			break;
		case FTW_DNR:
			logwf(log_name, "Couldnt read directory: %s", fpath);
			break;
		default:
			break;
	}
	return 0;
}

void audio_scanner::add_scan_dir(const std::string &path)
{
	logdf(log_name, "Adding scan path: %s", path.c_str());
	dirs.insert(path);
}

\
void audio_scanner::set_scan_type(int newtype)
{
	if(newtype <= scan_types::FAST){
		stype = static_cast<scan_types>(newtype);
	}
}

void audio_scanner::remove_scan_dir(std::string &path)
{
	logdf(log_name, "Removing scan path: %s", path.c_str());
	if(dirs.erase(path) == 0){
		logwf(log_name, "Scan path %s wasn't a scan path", path.c_str());
	}

}
