#include "dht_crawler.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	cout << "creating crawler object" << endl;
	dht_crawler* crawler = new dht_crawler("result.txt", 20, 1800);

	cout << "creating crawler sessions" << endl;
	crawler->create_sessions(32900);

	cout << "running crawler" << endl;
	crawler->run();

	return 0;
}