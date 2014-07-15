#pragma once

#include <cstring>
#include <cstdlib>
#include <stdarg.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <iostream>
#include <chrono>
#include <string>
#include <memory>
#include <cstdio>
#include <ctime>
#include <stddef.h>
#ifdef HAVE_BOOST_LOCALE
#include <boost/locale.hpp>
#endif
#include "../../external/lib/utf8/utf8.h"
 

#define ELOG_FILE (-7)

/* macro for writing to error */
#define loge(name, msg) \
	do{ \
		utils::log::logger::get_instance().write(name, utils::log::priority::ERROR, msg); \
	} while(0) 

/* macro for writing to warn */
#define logw(name, msg) \
	do{ \
		utils::log::logger::get_instance().write(name, utils::log::priority::WARN, msg); \
	} while(0) 

/* macro for writing to info */
#define logi(name, msg) \
	do{ \
		utils::log::logger::get_instance().write(name, utils::log::priority::INFO, msg); \
	} while(0) 

/* macro for writing to debug */
#define logd(name, msg) \
	do{ \
		utils::log::logger::get_instance().write(name, utils::log::priority::DEBUG, msg); \
	} while(0) 
/* macro for writef to error */
#define logef(name, ...) \
	do{ \
		utils::log::logger::get_instance().writef(name, utils::log::priority::ERROR, __VA_ARGS__); \
	} while(0)


/* macro for writef to warn */
#define logwf(name, ...) \
	do{ \
		utils::log::logger::get_instance().writef(name, utils::log::priority::WARN, __VA_ARGS__); \
	} while(0)

/* macro for writef to info */
#define logif(name, ...) \
	do{ \
		utils::log::logger::get_instance().writef(name, utils::log::priority::INFO, __VA_ARGS__); \
	} while(0)	

/* macro for writef to debug */
#define logdf(name, ...) \
	do{ \
		utils::log::logger::get_instance().writef(name, utils::log::priority::DEBUG, __VA_ARGS__); \
	} while(0)

namespace utils
{
	namespace log
	{
	/* a priority is a byte. each level is represented by one bit being set in this byte
		thus a set of priorities is a byte */
	typedef uint8_t priority_set;
	enum priority : uint8_t
		{
			ERROR = 0x1,
			WARN = 0x2,
			INFO = 0x4,
			DEBUG = 0x8,
			ALL = 0xf

		};
		class logger
		{
		public:

				static logger &get_instance();

				/* sets the log file to the file with name fname
				 * any other log file will be closed
				 * return: 0 on sucess, (file is opened and writable)
				 * ELOG_FILE on failure
				 */
				int set_log_file(const std::string &fname, bool erase_existing);

				/* adds the priorities to the logger */
				void set_priority(utils::log::priority_set p);

				/* clears the given priorities on the logger */
				void clear_priority(utils::log::priority_set p);

				/* writes the string message to the log. the module
				 * writing this line is name, and the priority is p
				 * this does not guarantee that this line is written
				 * for example, there may be an error, or the priority
				 * may be silenced
				 */
				void write(const char *name, const utils::log::priority p, const std::string &msg);

				/* writes the cstring message to the log. the module
				 * writing this line is name, and the priority is p
				 * this does not guarantee that this line is written
				 * for example, there may be an error, or the priority
				 * may be silenced
				 */
				void write(const char *name, const utils::log::priority p, const char *msg);

				/* works like printf just for writing to the log */
				void writef(const char *name, const utils::log::priority p,  const char *fmt, ...);

				void write_console(bool should_write);
				//friend std::ostream& operator<< (std::ostream& stream, const std::string str);
				//friend std::ostream& operator<< (std::ostream& stream, const int i);
				//friend std::ostream& operator<< (std::ostream& stream, const bool b);
				//friend std::ostream& operator<< (std::ostream& stream, const char *str);
		private:
				double get_time();
				logger();
				logger(logger &copy){};
				void operator=(logger &rhs);
				logger(logger &&other){};
				void operator=(logger &&rhs);
				std::string write_preamble(const char *name);
				void write_internal(const char *name, utils::log::priority p, const char *str);
				std::unique_ptr<std::ofstream> log_file;
				utils::log::priority_set prio;
				std::chrono::time_point<std::chrono::high_resolution_clock> start;
				std::mutex write_lock;
				bool console;
				bool preamble_written = false;

		};
	}
}