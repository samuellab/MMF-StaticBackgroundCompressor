/* 
 * File:   main.cpp
 * Author: Marc
 *
 * Created on July 6, 2012, 6:00 PM
 */



#include <cstdlib>
#include <io.h>

#include "StackReader.h"
#include "wtscWrapper.h"
#include "StackWriterWrapper.h"
using namespace std;

/*
 * 
 */
int testStackWriter();

int main(int argc, char** argv) {

    return testStackWriter();
}

int testStackWriter() {
    const char *sourcename = "E:\\from Bruno - thermo from Janelia\\CS@CS@t10@n#n#n#n@30.mmf";
    StackReader sr(sourcename);
    if (sr.isError()) {
        cout << "stack reader error " << sr.getError() << endl;
        return -1;
    }  else {
        cout << "stack reader opened ok" << endl;
    }
    const char *dstname = "c:\\teststackwriter.mmf";
    //wtscWrapper *ww = new wtscWrapper(dstname, 10, 5, 5, 180, 30);
    void *ww = createBrightFieldStackWriter(dstname, 10, 5,5,90,30);
    cout << "ww = " << (intptr_t) ww << endl;
    assert (NULL != ww);
    const int numframes = 1000;
    
    if (startRecording(ww, numframes)) {
        cout << "error on start recording\n" << endl;
    }
    for (int j = 0; j < numframes; ++j) {
        IplImage *im = NULL;
        sr.getFrame(j, &im);
        cout << "adding frame " << j << endl;
        addFrame(ww, (void *) im);
        int ntc, ntw;
        if (getNumStacksQueued(ww, &ntc, &ntw)) {
                cout << "error in get num stacks queued" << endl;
                return -1;
        } else {
            cout << " ntc = " << ntc << ", ntw = " << ntw;
        }
        while (ntc > 1 || ntw > 1) {
            Sleep(10);
            if (getNumStacksQueued(ww, &ntc, &ntw)) {
                cout << "error in get num stacks queued" << endl;
                return -1;
            } else {
                cout << " ntc = " << ntc << ", ntw = " << ntw;
            }
        }
        cvReleaseImage(&im);
    }
    stopRecording(ww);
    destroyStackWriter(ww);
    return 0;
        
}