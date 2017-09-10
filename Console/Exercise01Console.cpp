//Exercise 1 Console
//Bryce Graves

#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>

using namespace std;

int main(int argc, const char *argv[]) {
	int count = (atoi(argv[1]));
	string yourFruit;
	
	cout << "What is your favorite fruit? ";
	cin >> yourFruit;
	
	for (int i = 0; i < count; ++i) {
		cout << "You love " << yourFruit << endl;
	}
	
	cout << "We love fruit!\n";

}
