#pragma once
#include<fstream>
#include<iostream>
#include<string>
using namespace std;
class FileIO
{
public:
	FileIO() {}
	~FileIO() { cout << "FileIO对象被回收" << endl; }
	string read_file_to_string(string path) {
		string result;
		string temp;
		ifstream in;
		in.open(path);
		if (!in.is_open())
		{
			cerr << "文件打开错误" << endl;
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