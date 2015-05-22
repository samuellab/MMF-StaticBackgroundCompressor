/* 
 * File:   StackPlayer.cpp
 * Author: Marc
 *
 * Created on November 3, 2010, 11:12 AM
 * 
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <ostream>

#include "MultiStackReader.h"
using namespace std;

const int default_delay = 10;


int parseArguments(int argc, char **argv, string &filename, int &delayms, int &startframe, int &endframe, bool &annotated, bool &makeMdat);

/*
 * 
 */
int main(int argc, char** argv) {
    string filename;
    int delayms, startframe = 0, endframe = -1;
    bool annotated,makeMdat;
    if (parseArguments(argc, argv, filename, delayms, startframe, endframe, annotated, makeMdat) < 0) {
        return 0;
    }
   // cout << "opening stack reader" << endl;
    MultiStackReader sr(filename.c_str());
    if (!sr.dataFileOk()) {
        cout << "couldn't open or parse data file: " << filename;
        return 0;
    }
    if (makeMdat) {
        std::size_t p = filename.find('.');
        string pf = filename.substr(0, p);
        pf = pf + ".mdat";
        cout << "creating mdat file in " << pf << endl;
        sr.createSupplementalDataFile(pf.c_str());
        cout << "done!" << endl;
    }
   // cout << "stack reader opened" << endl;
    sr.playMovie(startframe, endframe, delayms, "MMF Player", annotated);
    return 0;
}

int parseArguments(int argc, char **argv, string &filename, int &delayms, int &startframe, int &endframe, bool &annotated, bool &makeMdat) {
    if (argc <= 1) {
        cout << "stackplayer movie.mmf -delayms [inter-frame delay in ms] -startframe [starting frame] -endframe [ending frame]" << endl << endl;
        cout << "or stackplayer -filename movie.mmf -delayms [inter-frame delay in ms] -startframe [starting frame] -endframe [ending frame]" << endl <<endl;
        cout << "add -annotated to see boxes around all regions different from background" << endl;
        cout << "add -mdat to create a metadata file" << endl;
        return -1;
    }
    vector<string> argsin;
    for (int j = 1; j < argc; ++j) {
        argsin.push_back(string(argv[j]));
    }
    filename = string("");
    delayms = default_delay;
    for (vector<string>::iterator it = argsin.begin(); it != argsin.end()-1; ++it) {
        if (it->compare("-filename") == 0) {
            filename = *(it+1);
        }
        if (it->compare("-delayms") == 0) {
            stringstream ss(*(it+1));
            ss >> delayms;
        }
        if (it->compare("-startframe") == 0) {
            stringstream ss(*(it+1));
            ss >> startframe;
        }
        if (it->compare("-endframe") == 0) {
            stringstream ss(*(it+1));
            ss >> endframe;
        }
    }
    annotated = false;
    makeMdat = false;
    for (vector<string>::iterator it = argsin.begin(); it != argsin.end(); ++it) {
        if (it->compare("-annotated") == 0) {
            annotated = true;
        }
        if (it->compare("-mdat") == 0) {
            makeMdat = true;
        }
    }

    if (filename.empty()) {
        filename = argsin.front();
    }
    return 0;    
}
