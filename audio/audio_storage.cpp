#include "audio_storage.hpp"


using namespace audio;

void get_song(const std::string &query, audio::song &s)
{
	sql << query, soci::into(s); 
}


void save_song(const audio::song &s)
{
	sql << "update songs set "
}