#include <iostream>

int Help(int n, int min, int max) {
	if (n <= 0) return 0;
	if (n == 1) return (max - min + 1);
	if (max == min) return 1;

	return Help(n - 1, min, max) + Help(n, min + 1, max);

}

int main(int argc, char * argv[])
{
	std::cout << Help(3, 1, 3) << std::endl;
	std::cout << "Hello";
	int x;
	std::cin >> x;
	return EXIT_SUCCESS;
}
