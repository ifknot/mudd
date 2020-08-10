#include <iostream>

#include "concurrent_dungeon.h"

int main() {

	std::cout << "mudd\n";

	mudd::concurrent_dungeon<int, 10, 10> d;

	std::cout << d.read(5, 5);
	d.write(1, 5, 5);
}