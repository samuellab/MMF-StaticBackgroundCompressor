/* 
 * File:   StackReader.h
 * Author: Marc
 *
 * Created on October 27, 2010, 4:32 PM
 */

#ifndef STACKREADER_H
#define	STACKREADER_H

#include "StaticBackgroundCompressor.h"
#include "ExtraDataWriter.h"
#include <map>
#include <string>
#include <cv.h>

class StackReader {
public:
    StackReader();
    StackReader(const char *fname);
    virtual ~StackReader();
    virtual void setInputFileName (const char *fname);
    virtual void openInputFile ();
    virtual void closeInputFile ();

    virtual void getBackground (int frameNum, IplImage **dst, int frameRange = 0);
    virtual void getFrame (int frameNum, IplImage **dst);
    virtual void annotatedFrame (int frameNum, IplImage **dst);
    virtual void playMovie (int startFrame = 0, int endFrame = -1, int delay_ms = 50, char *windowName = NULL, bool annotated = true);

    virtual void createSupplementalDataFile(const char *fname);
    virtual ExtraDataWriter *getSupplementalData();

    virtual inline bool dataFileOk (){
        return (infile != NULL && infile->good());
    }

    virtual inline int getTotalFrames () {
        return totalFrames;
    }

    virtual CvRect getLargestROI ();
    

protected:
    std::string fname;
    std::ifstream *infile;
    std::map<int, std::ifstream::pos_type> keyframelocations; //start frame, place in file

    int totalFrames;
    int startFrame;
    int endFrame;
    StaticBackgroundCompressor *sbc;

    virtual void init();

    virtual void parseInputFile();
    virtual void setSBC(int frameNum);

    CvRect validROI;
private:
      StackReader(const StackReader& orig);
  
};

#endif	/* STACKREADER_H */

