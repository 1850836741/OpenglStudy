#pragma once
#include<fstream>
#include<iostream>
#include<string>
using namespace std;
class FileIO
{
public:
	FileIO() {}
	~FileIO() { cout << "FileIO���󱻻���" << endl; }
	string read_file_to_string(string path) {
		string result;
		string temp;
		ifstream in;
		in.open(path);
		if (!in.is_open())
		{
			cerr << "�ļ��򿪴���" << endl;
			return NULL;
		}
		in.rdbuf();
		while (getline(in, temp)) {
			result += temp+"\n";
		}
		in.close();
		return result;
	}
private:

};