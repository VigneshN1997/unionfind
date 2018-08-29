#include <iostream>
#include <random>
#include <climits>
#include <cstdlib>
using namespace std;
int main(int argc, char const *argv[])
{
	/* code */
	const unsigned int range_from  = 0;
	const unsigned int range_to    = UINT_MAX;
	std::random_device                  rand_dev;
	std::mt19937                        generator(rand_dev());
	std::uniform_int_distribution<unsigned int>  distr(range_from, range_to);
	std::cout << distr(generator) << '\n';
	std::cout << distr(generator) << '\n';
	std::cout << distr(generator) << '\n';
	std::cout << distr(generator) << '\n';
	std::cout << distr(generator) << '\n';
	std::cout << distr(generator) << '\n';
	std::cout << distr(generator) << '\n';
	std::cout << distr(generator) << '\n';
	std::cout << distr(generator) << '\n';
	return 0;
}
