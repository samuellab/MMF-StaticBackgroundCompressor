/* 
 * File:   testmain.cpp
 * Author: Marc
 *
 * Created on October 5, 2010, 12:03 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#include <cstdlib>
#include "cv.h"
#include "highgui.h"
#include "StaticBackgroundCompressor.h"
#include "LinearStackCompressor.h"
#include "BlankMetaData.h"
#include "StackReader.h"
#include "WindowsThreadStackCompressor.h"
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
void testLSC();
void testWTSC();
void testRecompress();
void testSR();
int main(int argc, char** argv) {
   // createTestStack();
  //loadAndPlayTestStack();
 // testLSC();

   // testSR();
    //testWTSC();
    testRecompress();
    return 0;
}
void loadAndPlayTestStack() {
 //   ifstream is ("c:\\imagestack.bin",ifstream::binary);
    ifstream is ("c:\\teststack.bin", ifstream::binary);
    is.seekg(LinearStackCompressor::headerSizeInBytes);
    StaticBackgroundCompressor *sbc = StaticBackgroundCompressor::fromDisk(is);
    cout << (is.good() ? "no error\n":"error\n");
    cout << (is.eof() ? "at end of file\n":"not at end of file\n");
    cout << (is.tellg() - (ifstream::pos_type) LinearStackCompressor::headerSizeInBytes) << "bytes read\n";
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
    sbc.setThresholds(0, 5, 2, 3);
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

void testSR() {
   // StackReader sr("\\\\labnas2\\LarvalCO2\\mess_stack.mmf");
  //  StackReader sr("c:\\teststack.bin");
   // StackReader sr("\\\\labnas2\\LarvalCO2\\Image Data\\control air only\\20101105\\CS5\\CS5_stack.mmf");
    //StackReader sr("\\\\labnas2\\Phototaxis\\exponentialstack.mmf");
    StackReader sr("E:\\from Bruno - thermo from Janelia\\CS@CS@t10@n#n#n#n@30.mmf");
    cout << "created sr\n";
//    sr.createSupplementalDataFile("c:\\testcs5_dat.dat");
    sr.playMovie();
}

void testRecompress() {
    
    StackReader sr("E:\\from Bruno - thermo from Janelia\\CS@CS@t10@n#n#n#n@30.mmf");
    cout << "created sr\n";
    cout << sr.diagnostics();
    sr.decimateStack("E:\\from Bruno - thermo from Janelia\\CS@CS@t10@n#n#n#n@30_reduced.mmf", 10, 4, 6, 4);
    cout << "done!\n";
     
    StackReader sr2("E:\\from Bruno - thermo from Janelia\\CS@CS@t10@n#n#n#n@30_reduced.mmf");
    cout << "sr2 loaded\n" << endl;
    cout << sr2.diagnostics();
    sr2.playMovie(0,-1,30,"recompressed movie", true);

}

void testLSC() {
    string stub = "\\\\labnas2\\LarvalCO2\\Image Data\\50 mL CO2 in 2 L air\\20101001\\CS3\\CS3_";
    LinearStackCompressor lsc;
//    StaticBackgroundCompressor sbc;
 //   sbc.setThresholds(0, 5);
    lsc.setThresholds(0, 5, 2, 3);
    lsc.setIntervals(128, 1);
    lsc.setOutputFileName("c:\\teststack.bin");
    int nframes = 1000;
    lsc.startRecording(nframes);
    stringstream s;
    for (int j = 0; j < nframes; ++j) {
        s.str("");
        s << stub << j << ".jpg";
        IplImage *im = cvLoadImage(s.str().c_str(), 0);
        assert(im != NULL);
        cout << "loaded image " << j <<"\n";
        BlankMetaData *bmd = new BlankMetaData;
        lsc.newFrame(im, bmd);
        cvReleaseImage(&im);
   //     cout << "added image to stack\n";
    }

    lsc.stopRecording();
    
}

void testWTSC() {
    string stub = "\\\\labnas2\\LarvalCO2\\Image Data\\50 mL CO2 in 2 L air\\20101001\\CS3\\CS3_";
    WindowsThreadStackCompressor lsc;
//    StaticBackgroundCompressor sbc;
 //   sbc.setThresholds(0, 5);
    lsc.setThresholds(0, 5, 2, 3);
    lsc.setIntervals(90, 1);
    lsc.setOutputFileName("c:\\teststack.mmf");
    lsc.setFrameRate(5);
    int nframes = 1000;
    lsc.startThreads();
    lsc.startRecording(nframes);
    stringstream s;
    for (int j = 0; j < nframes; ++j) {
        s.str("");
        s << stub << j << ".jpg";
        IplImage *im = cvLoadImage(s.str().c_str(), 0);
        assert(im != NULL);
        cout << "loaded image " << j <<"\n";
        BlankMetaData *bmd = new BlankMetaData;
        lsc.newFrame(im, bmd);
        cvReleaseImage(&im);
   //     cout << "added image to stack\n";
    }

    lsc.stopRecording();

}
