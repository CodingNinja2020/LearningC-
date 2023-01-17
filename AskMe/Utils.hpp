#pragma once

#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <vector>
using namespace std;

/////////////////////////////// Helper Methods ///////////////////////////////
vector<string> ReadFileLines(const string &path);
void WriteFileLines(const string &path, const vector<string> &lines,
                    bool append = true);
vector<string> SplitString(const string &str, const string &delimiter = ",");
int ToInt(const string &str);
int ReadInt(int low, int high);
int ShowReadMenu(const vector<string> &choices);
