#include <iostream>
#include "../file_utils.hpp"




int main(int argc, char const *argv[])
{
	std::cout << utils::file::get_sakura_home_dir();
	return 0;
}