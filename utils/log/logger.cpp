#include "logger.hpp"

using namespace utils::log;

double logger::get_time()
{
	std::chrono::time_point<std::chrono::high_resolution_clock> end =
		std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>
		(end - start).count() / 1000000.0;
}


logger::logger() : log_file (new std::ofstream())
{
	prio = priority::ERROR | priority::WARN;
	start = std::chrono::high_resolution_clock::now();
	console = true;
}

logger &logger::get_instance()
{
	static logger w;
	return w;
}


int logger::set_log_file(const std::string &fname, bool erase_existing)
{
	std::lock_guard<std::mutex> lock(write_lock);
	if(log_file->is_open()){
		log_file->close();
	}
	//log_file->exceptions(std::ios::badbit | std::ios::failbit);
	try{
		if(erase_existing){
			log_file->open(fname, std::ios::trunc);
		} else{
			log_file->open(fname, std::ios::app);
		}
	} catch (std::ios::failure e){
		return ELOG_FILE;
	}
	if(log_file->is_open() && log_file->good()){
		#ifdef HAVE_BOOST_LOCALE
		boost::locale::generator gen;
		std::locale l = gen("");
		std::locale::global(l);
		log_file->imbue(l);
		#endif
		return 0;
	} else{
		return ELOG_FILE;
	}
}

void logger::set_priority(priority_set p)
{
	if(p > priority::ALL){
		writef("logger", priority::WARN, "Invalid priority set: %u", p);
	} else{
		prio |= p;
	}
}

void logger::clear_priority(priority_set p)
{
	if(p > priority::ALL){
		writef("logger", priority::WARN, "Invalid priority clear: %u", p);
	} else{
		prio &= (~p);
	}
}


void logger::write_console(bool should_write)
{
	console = should_write;
}

void logger::write(const char *name, const priority p, const std::string &msg)
{
	if(p & prio){
		write_internal(name, p, msg.c_str());
	}
}

void logger::write(const char *name, const priority p, const char *msg)
{
	if(p & prio){
		write_internal(name, p, msg);
	}
}

void logger::writef(const char *name, const priority p, const char * fmt, ...)
{
	if(p & prio){
		va_list args, rargs;
		va_start(args, fmt);
		va_copy(rargs, args);
		size_t needed = 0;
		needed = vsnprintf(nullptr, 0, fmt, args);
		char buffer[++needed];
		if(needed < 2 || vsnprintf(buffer, needed, fmt, rargs) < 0){
			write_internal("logger", utils::log::priority::WARN, "Error writing formatted string to log.");
		} else{
			write_internal(name, p, buffer);
		}
	}
}


std::string logger::write_preamble(const char *name)
{
	std::stringstream ss;
	time_t now = time(0);
	#ifdef HAVE_BOOST_LOCALE
	ss << "(" << boost::locale::as::date << now << "::" << boost::locale::as::time << now << ")" 
    	<< " [" << std::fixed << std::setprecision(6) << boost::locale::as::number << get_time() << std::resetiosflags(std::ios::fixed) 
		<< std::setprecision(0) << "::" << name << "] ";
	#else
	struct tm * ts = localtime(&now);
	char *c = asctime(ts);
	c[strlen(c) - 1] = '\0';
	ss << "(" << c << ")" << " [" << std::fixed << std::setprecision(6)
	    << get_time() << std::resetiosflags(std::ios::fixed) 
		<< std::setprecision(0) << "::" << name << "] ";
	#endif
	return ss.str();
}

void logger::write_internal(const char *name, const priority p, const char *str)
{
	
	if(log_file && log_file->is_open() && log_file->good()){
		//make sure that the string and name are utf8
		//it is fine to just use strlen as it will return the number of bytes
		if(!utf8::is_valid(name, name+strlen(name))){
			std::string replace; 
			utf8::replace_invalid(name, name+strlen(name), back_inserter(replace));
			name = replace.c_str();
		}
		if(!utf8::is_valid(str, str+strlen(str))){
			std::string replace; 
			utf8::replace_invalid(str, str+strlen(str), back_inserter(replace));
			str = replace.c_str();
		}
		std::lock_guard<std::mutex> lock(write_lock);
    	std::string s = write_preamble(name);
		switch(p){
			case priority::ERROR:
				(*log_file) << s <<  "(ERROR) " << str << std::endl;
				if(console){
					std::cerr << s  << "(ERROR) " << str << std::endl;
				}
				break;
			case priority::WARN:
				(*log_file) << s <<  "(WARN) " << str << std::endl;
				if(console){
					std::cout << s  << "(WARN) " << str << std::endl;
				}
				break;
			case priority::INFO:
				(*log_file) << s <<  "(INFO) " << str << std::endl;
				if(console){
					std::clog << s << "(INFO) " << str << std::endl;
				}
				break;
			case priority::DEBUG:
				(*log_file) << s <<  "(DEBUG) " << str << std::endl;
				if(console){
					std::clog << s  << "(DEBUG) " << str << std::endl;
				}
				break;
			default:
				(*log_file) << s <<  "(ERROR) " << "Unknown priority" << p << std::endl;
				if(console){
					std::cerr << s << "(ERROR) " << str << std::endl;
				}
				break;
		}  
	}
}