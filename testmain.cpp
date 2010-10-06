/* 
 * File:   testmain.cpp
 * Author: Marc
 *
 * Created on October 5, 2010, 12:03 PM
 */

#include <cstdlib>
#include "cv.h"
#include "highgui.h"
#include "StaticBackgroundCompressor.h"
#include <string>
#include <sstream>
#include <iostream>
#include <ostream>
using namespace std;

/*
 * 
 */
void createTestStack();
void loadAndPlayTestStack() ;

int main(int argc, char** argv) {
    createTestStack();
  loadAndPlayTestStack();
    return 0;
}
void loadAndPlayTestStack() {
    ifstream is ("c:\\imagestack.bin",ifstream::binary);
    StaticBackgroundCompressor *sbc = StaticBackgroundCompressor::fromDisk(is);
    cout << (is.good() ? "no error\n":"error\n");
    cout << (is.eof() ? "at end of file\n":"not at end of file\n");
    cout << is.tellg() << "bytes read\n";
    is.seekg(0,ifstream::end);
     cout << is.tellg() << "bytes in file\n";
    is.close();
    cout.flush();
    cout << "estimated size on disk is " << sbc->sizeOnDisk() << "\n";
    cout.flush();
    sbc->playMovie();
    delete sbc;
}
void createTestStack() {
    string stub = "\\\\labnas2\\LarvalCO2\\Image Data\\50 mL CO2 in 2 L air\\20101001\\CS3\\CS3_";
    StaticBackgroundCompressor sbc;
    sbc.setThresholds(0, 5);
    int nframes = 1000;
    stringstream s;
    for (int j = 0; j < nframes; ++j) {
        s.str("");
        s << stub << j << ".jpg";
        IplImage *im = cvLoadImage(s.str().c_str(), 0);
        assert(im != NULL);
//        cout << "loaded image " << j <<"\n";
        sbc.addFrame(im);
        cvReleaseImage(&im);
   //     cout << "added image to stack\n";
    }
 //   cout << "calculating background\n";
    sbc.calculateBackground();
 //   cout << "processing images\n";
    int nf;
    while((nf = sbc.processFrame()) > 0) {
  //      cout << nf << "\t";
    }
    cout << "size on disk should be ~ " << sbc.sizeOnDisk();
    cout << "saving to disk\n";
    ofstream outfile ("c:\\imagestack.bin",ofstream::binary);
    cout << "opened\n";
    cout << (outfile.good() ? "no error\n":"error\n");
    sbc.toDisk(outfile);
    cout << "wrote " << outfile.tellp() << " bytes\n";
     cout << (outfile.good() ? "no error\n":"error\n");
    outfile.close();
    cout << "done\n";
    
}