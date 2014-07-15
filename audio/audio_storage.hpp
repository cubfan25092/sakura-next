#pragma once

#include <string>
#include <soci.h>
#include "song.hpp"





namespace audio
{

	class audio_storage
	{
	public:
		void get_song(const std::string &query, audio::song &s);
		void save_song(const audio::song &s);
		void init(const std::string &db_conn_str, const bool firstrun);

		~audio_storage()
		{
			sql.close();
		}
	private:
		soci::session sql;
		const char *log_name = "audio_storage";

	};
}

namespace soci
{
	template<>
	struct type_conversion<audio::song>
	{
		typedef values base_type;
		static void from_base(values const & v, audio::song &s)
		{
			s.title = v.get<std::string>("title", "");
			s.album = v.get<std::string>("album", "");
			s.artist = v.get<std::string>("artist", "");
			s.album_artist = v.get<std::string>("album_artist", "");
			s.genre = v.get<std::string>("genre");
			s.composer = v.get<std::string>("composer", "");
			s.performer = v.get<std::string>("performer", "");
			s.vocaloid = v.get<std::string>("vocaloid", "");
			s.language = v.get<std::string>("language", "");
			s.title = v.get<std::string>("album_artist", "");
			s.genre = v.get<std::string>("genre", "");
			s.composer = v.get<std::string>("composer", "");
			s.album_art_path = v.get<std::string>("album_art_path", "");
			s.filepath = v.get<std::string>("filepath", "");
			s.moodbar_filepath = v.get<std::string>("moodbar_filepath", "");
			s.album_art_b64 = v.get<std::string>("album_art_b64", "");
			s.comment = v.get<std::string>("comment", "");
			s.lyrics = v.get<std::string>("lyrics", "");


			s.track_number = v.get<unsigned>("track_number", 0);
			s.total_tracks = v.get<unsigned>("total_tracks", 0);
			s.year = v.get<unsigned>("year", 0);
			s.date = v.get<unsigned>("date", 0);
			s.bitrate = v.get<unsigned>("bitrate", 0);
			s.play_count = v.get<unsigned>("play_count", 0);
			s.popularimeter = v.get<unsigned>("popularimeter", 0);
			s.disc_number = v.get<unsigned>("disc_number", 0);
			s.total_discs = v.get<unsigned>("total_discs", 0);
			s.file_size = v.get<unsigned>("file_size", 0);
			s.sample_rate = v.get<unsigned>("sample_rate", 0);
			s.skip_count = v.get<unsigned>("skip_count", 0);
			s.last_played = v.get<unsigned>("last_played", 0);
			s.added = v.get<unsigned>("added", 0);
			s.last_skipped = v.get<unsigned>("last_skipped", 0);
			s.last_modified = v.get<unsigned>("last_modified", 0);
			s.bpm = v.get<unsigned>("bpm", 0);
			s.channels = v.get<unsigned>("channels", 0);
			s.permissions = v.get<unsigned>("permissions", 0444);
			s.owner = v.get<unsigned>("owner", 1000);
			s.group = v.get<unsigned>("group", 1000);
			s.skip_count = v.get<unsigned>("skip_count", 0);
			s.last_played = v.get<unsigned>("last_played", 0);
			s.score = v.get<int>("score", 0);
			s.rating = v.get<int>("rating", 0);
			s.id = v.get<unsigned>("id", 0);
			s.vbr = v.get<bool>("vbr", false);
			s.compilation = v.get<bool>("compilation", false);
		}

		static void to_base(const audio::song &s, values &v, indicator &ind)
	};
}