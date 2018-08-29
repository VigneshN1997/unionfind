#include <iostream>
#include <fstream>
#include <sstream>
#include<unordered_map>
using namespace std;

int main() {
	unordered_map<string, int> config;
	ifstream configFile("config.txt");
	string line;
	while(getline(configFile, line)) {
		istringstream is_line(line);
		string key;
		if(getline(is_line, key, '='))
		{
			string value;
			if(getline(is_line, value)) {
				stringstream val(value);
				int x;
				val >> x;
			 	config[key] = x;
			}
		}
	}
	unordered_map<string, int>::iterator itr;
	for(itr = config.begin(); itr != config.end(); itr++) {
		cout << itr->first << ":" << itr->second << "\n";
	}
	return 0;
}