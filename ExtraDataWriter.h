/* 
 * File:   ExtraDataWriter.h
 * Author: Marc
 *
 * Created on November 10, 2010, 3:43 PM
 * A class that supports writing extra data on a frame by frame basis
 *
 * used to write out metadata stored in background removed image stacks
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#ifndef EXTRADATAWRITER_H
#define	EXTRADATAWRITER_H

class ExtraDataWriter {
public:
    ExtraDataWriter();
    void nextFrame();
    int getCurrentFrameNumber();
    void goToFrame(int frameNum);

    void addElement (std::string columnName, double value);
    void addElement (std::pair<std::string, double> element);
    void addElements (const std::vector<std::string> &columnNames, const std::vector<double> &values);
    void addElements (const std::vector<std::pair<std::string, double> > &elements);
    void addElements (const std::map<std::string, double> &elements);


    void writeFile (std::ostream &os);
    void writeFile (std::ostream &os, const std::vector<std::string> &columnsToWrite);
    void writeFile (const char *fname);
    void writeFile (const char *fname, const std::vector<std::string> &columnsToWrite);



    virtual ~ExtraDataWriter();
protected:
    typedef std::pair<std::string, double> FrameEntryT;
    typedef std::map<std::string, double> FrameT;
    std::vector<FrameT *> frames;
    std::vector<std::string> columnHeadings;
    int currentFrameNumber;
    void writeRow (std::ostream &os,const std::vector<std::string> &columnsToWrite, FrameT *frame);
    void writeColumnHeaders(std::ostream &os, const std::vector<std::string> &columnsToWrite);
    FrameT *currentFrame;

private:
     ExtraDataWriter(const ExtraDataWriter& orig);
   
};

#endif	/* EXTRADATAWRITER_H */

