#include "../logger.hpp"
#include <pthread.h>
#include <unistd.h>

using namespace utils;
#define LOG_NAME "LogTests"

void *thread_write(void *n)
{
	char buf[80];
	snprintf(buf, 80, "thread%lu.txt", pthread_self());
	log::logger::get_instance().set_log_file(buf, false);	
	for(int i = 0; i < 20; ++i){
		log::logger::get_instance().writef(LOG_NAME, log::priority::ERROR, "thread %lu", pthread_self());
	}
	return NULL;
}

int main(int argc, char const *argv[])
{
	int ret = log::logger::get_instance().set_log_file("test.txt", true);
	if(ret == ELOG_FILE){
		std::cout << "FAILED: couldnt open file" << std::endl;
		return -1;
	}
	log::logger::get_instance().write(LOG_NAME, log::priority::INFO, "test info not written by default");
	log::logger::get_instance().write(LOG_NAME, log::priority::DEBUG, "test debug not written by default");
	log::logger::get_instance().set_priority(log::priority::DEBUG | log::priority::INFO);
	log::logger::get_instance().write(LOG_NAME, log::priority::DEBUG, "test debug written on flag");
	log::logger::get_instance().write(LOG_NAME, log::priority::ERROR, "test error still written");
	log::logger::get_instance().write(LOG_NAME, log::priority::WARN, "test warn still written");
	log::logger::get_instance().write(LOG_NAME, log::priority::INFO, "test info written on flag");
	log::logger::get_instance().clear_priority(log::priority::DEBUG | log::priority::INFO);
	log::logger::get_instance().write(LOG_NAME, log::priority::INFO, "test info not written on flag");
	log::logger::get_instance().write(LOG_NAME, log::priority::ERROR, "test error still written");
	log::logger::get_instance().write(LOG_NAME, log::priority::WARN, "test warn still written");
	log::logger::get_instance().clear_priority(log::priority::ALL);
	log::logger::get_instance().write(LOG_NAME, log::priority::DEBUG,"test debug not written");
	log::logger::get_instance().write(LOG_NAME, log::priority::ERROR, "test error not written");
	log::logger::get_instance().write(LOG_NAME, log::priority::WARN, "test warn not written");
	log::logger::get_instance().write(LOG_NAME, log::priority::INFO, "test info not written");
	log::logger::get_instance().set_priority(log::priority::ERROR | log::priority::WARN);
	//test console
	log::logger::get_instance().write_console(false);
	log::logger::get_instance().write(LOG_NAME, log::priority::ERROR, "test error still written(no console)");
	log::logger::get_instance().write(LOG_NAME, log::priority::WARN, "test warn still written(no console)");
	//test writef
	log::logger::get_instance().write_console(true);
	log::logger::get_instance().writef(LOG_NAME, log::priority::ERROR, "derp%d%s%lu", 4, "lol", 56754734);
	logwf(LOG_NAME,"%s%d" ,"lol", 42);
	//test unicode
	loge(LOG_NAME, "ハローワールド");
	//thread tests
	pthread_t threads[8];
	for(int i = 0; i < 8; ++i){
		pthread_create(&threads[i], NULL, thread_write, NULL);
	}
	for(int i = 0; i < 8; ++i){
		pthread_join(threads[i], NULL);
	}
	return 0;
}
