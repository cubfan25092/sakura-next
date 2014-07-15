#pragma once

#include <string>

namespace audio
{
	struct song
	{
		std::string title;
		std::string album;
		std::string artist;
		std::string album_artist;
		std::string genre;
		std::string composer;
		std::string performer;
		std::string language;
		std::string vocaloid;
		std::string album_art_path;
		std::string filepath;
		std::string moodbar_filepath;
		std::string album_art_b64;
		std::string comment;
		std::string lyrics;
		std::string format;
		
		unsigned track_number;
		unsigned total_tracks;
		unsigned year;
		unsigned bitrate;
		unsigned date;
		unsigned play_count;
		unsigned popularimeter;
		unsigned disc_number;
		unsigned total_discs;
		unsigned file_size;
		unsigned sample_rate;
		unsigned skip_count;
		unsigned last_played;
		unsigned added;
		unsigned last_skipped;
		unsigned last_modified;
		unsigned bpm;
		unsigned channels;
		unsigned permissions;
		unsigned owner;
		unsigned group;
		unsigned id;
		int score;
		int rating;

		bool vbr;
		bool compilation;

	};
}