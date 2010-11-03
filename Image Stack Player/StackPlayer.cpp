/* 
 * File:   StackPlayer.cpp
 * Author: Marc
 *
 * Created on November 3, 2010, 11:12 AM
 */

#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <ostream>

#include "StackReader.h"
using namespace std;

const int default_delay = 50;


int parseArguments(int argc, char **argv, string &filename, int &delayms, int &startframe, int &endframe);

/*
 * 
 */
int main(int argc, char** argv) {
    string filename;
    int delayms, startframe, endframe;
    if (parseArguments(argc, argv, filename, delayms, startframe, endframe) < 0) {
        return 0;
    }
    StackReader sr(filename.c_str());
    if (!sr.dataFileOk()) {
        cout << "couldn't open or parse data file: " << filename;
        return 0;
    }
    sr.playMovie(startframe, endframe, delayms, "MMF Player");
    return 0;
}

int parseArguments(int argc, char **argv, string &filename, int &delayms, int &startframe, int &endframe) {
    if (argc <= 1) {
        cout << "stackplayer movie.mmf -delayms [inter-frame delay in ms] -startframe [starting frame] -endframe [ending frame]" << endl;
        cout << "or stackplayer -filename movie.mmf -delayms [inter-frame delay in ms] -startframe [starting frame] -endframe [ending frame]" << endl;
        return -1;
    }
    vector<string> argsin;
    for (int j = 1; j < argc; ++j) {
        argsin.push_back(string(argv[j]));
    }
    filename = string("");
    delayms = default_delay;
    for (vector<string>::iterator it = argsin.begin(); it != argsin.end()-1; ++it) {
        if (it->compare("-filename")) {
            filename = *(it+1);
        }
        if (it->compare("-delayms")) {
            stringstream ss(*(it+1));
            ss >> delayms;
        }
        if (it->compare("-startframe")) {
            stringstream ss(*(it+1));
            ss >> startframe;
        }
        if (it->compare("-endframe")) {
            stringstream ss(*(it+1));
            ss >> endframe;
        }
    }
    if (filename.empty()) {
        filename = argsin.front();
    }
    return 0;    
}
