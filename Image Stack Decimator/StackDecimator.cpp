/* 
 * File:   StackDecimator.cpp
 * Author: Marc
 *
 * Created on October 17, 2011, 9:22 AM
 */

#include <cstdlib>
#include "StackReader.h"
#include <sstream>
using namespace std;

/*
 * 
 */


int parseArguments(int argc, char **argv, string &filename, string &outputname, int &decimationcount, int &startframe, int &endframe, int &threshAboveBackground);

/*
 *
 */
int main(int argc, char** argv) {
    string filename;
    string outputname;
    int decimationcount = 3, startframe = 0, endframe = -1, threshAboveBackground = 10;
    if (parseArguments(argc, argv, filename, outputname, decimationcount, startframe, endframe,threshAboveBackground) < 0) {
        return 0;
    }
   // cout << "opening stack reader" << endl;
    StackReader sr(filename.c_str());
    if (!sr.dataFileOk()) {
        cout << "couldn't open or parse data file: " << filename;
        return 0;
    }
    sr.decimateStack(outputname.c_str(), threshAboveBackground, 3, 3, decimationcount);
   // cout << "stack reader opened" << endl;
  //  sr.playMovie(startframe, endframe, delayms, "MMF Player", annotated);
    return 0;
}

int parseArguments(int argc, char **argv, string &filename, string &outputname, int &decimationcount, int &startframe, int &endframe, int &threshAboveBackground) {
    if (argc <= 1) {
        cout << "decimatestack input.mmf output.mmf -decimationcount [frame skip factor] -threshold [threshold above background] -startframe [starting frame] -endframe [ending frame]" << endl << endl << "or" << endl;
        cout << "decimatestack -inputname input.mmf -outputname output.mmf -decimationcount [frame skip factor] -threshold [threshold above background] -startframe [starting frame] -endframe [ending frame]" << endl << endl;

        cout << "default decimation count is 3 (every third frame is copied)" <<endl<< "default threshold above background is 10" << endl << "startframe, endframe not supported yet"<<endl;
        return -1;
    }
    vector<string> argsin, unparsedargs;
    for (int j = argc-1; j >=1; --j) {
        argsin.push_back(string(argv[j]));
    }
    filename = outputname = string("");
    
    startframe = 0;
    endframe = 0;
    //for (vector<string>::iterator it = argsin.begin(); it != argsin.end()-1; ++it) {
    while (!argsin.empty()) {
        string s = argsin.back();
        argsin.pop_back();
        if (s.compare("-inputname") == 0) {
            filename = argsin.back();
            argsin.pop_back();
            continue;
        }
        if (s.compare("-outputname") == 0) {
            outputname = argsin.back();
            argsin.pop_back();
            continue;
        }
        if (s.compare("-decimationcount") == 0) {
            string s2 = argsin.back();
            stringstream ss(s2);
            argsin.pop_back();
            ss >> decimationcount;
            continue;
        }
        if (s.compare("-threshold") == 0) {
            stringstream ss((string) argsin.back());
            argsin.pop_back();
            ss >> threshAboveBackground;
            continue;
        }
        if (s.compare("-startframe") == 0) {
            stringstream ss((string) argsin.back());
            argsin.pop_back();
            ss >> startframe;
            continue;
        }
        if (s.compare("-endframe") == 0) {
            stringstream ss((string) argsin.back());
            argsin.pop_back();
            ss >> endframe;
            continue;
        }
        unparsedargs.push_back(s);
    }
    for (vector<string>::iterator it = unparsedargs.begin(); it != unparsedargs.end(); ++it) {
        if (it->at(0) == '-') {
            cout << "I didn't understand option: " << *it << endl <<endl;
            return parseArguments(0, NULL, filename, outputname, decimationcount, startframe, endframe, threshAboveBackground);
        }
        if (filename.empty()) {
            filename = *it;
            continue;
        }
        if (outputname.empty()) {
            outputname = *it;
            continue;
        }
        cout << "too many filenames" << endl << "inptuname = " << filename << endl << "outputname = " << outputname << endl << "extra name = " << *it << endl<< endl;
        return parseArguments(0, NULL, filename, outputname, decimationcount, startframe, endframe, threshAboveBackground);
    }

    if (filename.empty() || outputname.empty()) {
        cout << "must supply inputname and outputname" << endl<< endl;
        return parseArguments(0, NULL, filename, outputname, decimationcount, startframe, endframe, threshAboveBackground);
    }
    if (filename.compare(outputname) == 0) {
        cout << "inputname cannot be the same as output name" << endl<< endl;
        return parseArguments(0, NULL, filename, outputname, decimationcount, startframe, endframe, threshAboveBackground);
    }
    if (startframe != 0 || endframe != 0) {
        cout << "warning: startframe, endframe not supported in this implementation -- full stack will be decimated" << endl;
    }
    return 0;
}
