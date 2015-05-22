/* 
 * File:   WindowsThreadStackCompressor.h
 * Author: Marc
 *
 * modifies LinearStackCompressor to use windows threads and mutexes
 * * Generates a stack of background removed images on disk;  This is the highest level object in the library
 * it maintains a set of StaticBackgroundCompressor objects, each of which holds a background image and a set of background removed images
 *
 *
 * compresses images as they are added to the stack with explicit multi-threading
 * as long as the average processing time (including writing to disk)
 * is less than the average arrival time of images, this will compress images continuously without
 * slowing acquisition as every function returns almost immediately
 * 
 *
 * below is an example use of WTSC to compress a stack of jpg images
 *
 * string stub = "\\\\labnas2\\LarvalCO2\\Image Data\\50 mL CO2 in 2 L air\\20101001\\CS3\\CS3_";
 *   WindowsThreadStackCompressor wtsc;
 *   wtsc.setThresholds(0, 5, 2, 3);
 *   wtsc.setIntervals(90, 1);
 *   wtsc.setOutputFileName("c:\\teststack.mmf");
 *   wtsc.setFrameRate(5);
 *   int nframes = 1000;
 *   wtsc.startThreads();
 *   wtsc.startRecording(nframes);
 *   stringstream s;
 *   for (int j = 0; j < nframes; ++j) {
 *       s.str("");
 *       s << stub << j << ".jpg";
 *       IplImage *im = cvLoadImage(s.str().c_str(), 0);
 *       assert(im != NULL);
 *       cout << "loaded image " << j <<"\n";
 *       BlankMetaData *bmd = new BlankMetaData;
 *       wtsc.newFrame(im, bmd);
 *       cvReleaseImage(&im);
 *   }
 *   wtsc.stopRecording();
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#ifndef WINDOWSTHREADSTACKCOMPRESSOR_H
#define	WINDOWSTHREADSTACKCOMPRESSOR_H

#ifdef WIN32

    #include "LinearStackCompressor.h"
    #include "tictoc/tictoc.h"
    #include <windows.h>
    #include <process.h>
    #include <queue>

    class WindowsThreadStackCompressor : public LinearStackCompressor {
    public:
        /* WindowsThreadStackCompressor();
        *
        * creates a wtsc and initializes it, but does not open a data file or start recording
        * also, does not start required threads;  user must call startThreads prior to
        * calling newFrame
        */

        WindowsThreadStackCompressor();
        /* virtual ~WindowsThreadStackCompressor();
        * finishes recording (compresses & writes any outstanding frames to disk)
        * then closes output file (if open), stops threads and frees memory
        *
        */
        virtual ~WindowsThreadStackCompressor();

         /* virtual int checkIfThreadsHaveEnded(int timeout= 2000);
         * 
         * returns 0 if threads have ended
         * otherwise returns an integer reflecting which threads are still active
         *
         */
        virtual int checkIfThreadsAreActive(int timeout = 2000);
       
         /* virtual int endThreads(int timeout= 2000);
         * 
         * ends all compression and writing threads, call only before destruction 
         *
         */
        virtual int endThreads(int timeout= 2000);
       
        
        
        /* virtual int startThreads(); //returns -1 on error, 0 on ok
        * 
        * starts compression and writing threads that run in background
        * 
        * may be called at any time after construction, must be called before first newFrame
        */
        virtual int startThreads(); //returns -1 on error, 0 on ok

        /* virtual void newFrame(const IplImage *im, ImageMetaData *metadata = NULL);
        *
        * adds an image and any associated metadata to the stack of images to be compressed
        *
        * a copy of the IplImage is made and stored; the user is free to delete or reuse the image after calling
        * the metadata becomes the property and responsibility of the lsc;
        * the user should make no further use of the metadata and lsc will free it when appropriate
        *
        * a note on timing: a few memory allocations, copying the image over, and updating of the background via a comparison
        * are all carried out prior to this function returning;  the total overhead of these function calls is small
        * the function must also acquire one or more mutexes before proceeding; however, the code has been written to minimize the
        * amount of time any thread holds on to the mutexes required by next frame
        *
        * it is up to the user to determine the maximum rate at which frames can be added in the environment in which this code is used
        * it has been tested at 5 Hz with 5 MP images without problems on a typical windows machine
        */
        virtual void newFrame(const IplImage *im, ImageMetaData *metadata);
        /* virtual void finishRecording();
        *
        * stops recording and waits for all images to be written to disk before proceeding
        * does not close output file
        */
        virtual void finishRecording();

        /* virtual bool nothingLeftToCompressOrWrite();
         * returns true if all images have been compressed and written to disk
         */
         
        virtual bool nothingLeftToCompressOrWrite(bool verbose = false);
        
        virtual void goIdle();
        /* virtual void goIdle();
         * stops recording; adds any queued images to last static background compressor
         * then returns (does not wait for threads to finish)
         *
         */
        
        /* virtual void openOutputFile ();
        * virtual void closeOutputFile ();
        *
        * opens, closes output files;  neither needs to be called explicitly by user, but functionality is provided anyway
        *
        * writing thread will automatically open the output file as soon as it has stacks to write, so
        * user should be sure to set a file name prior to starting recording
        * output file is closed automatically on destruction, or on creation of a new output file
        */
        virtual void openOutputFile ();
        virtual void closeOutputFile ();

        /* setNumCompressionThreads (int numThreads)
         * sets the maximum number of threads used by the static background compressor
         * note: does not change the number of threads used directly by this class,
         *   which has a separate writing threads and compression threads
         */
        virtual inline void setNumCompressionThreads (int numThreads) {
            maxCompressionThreads = numThreads;
        }
        
        inline TICTOC::tictoc const& NonthreadedTimer () {
            return nonthreadedTimer;
        }
        inline TICTOC::tictoc const& WritingThreadTimer () {
            return writingThreadTimer;
        }
        inline TICTOC::tictoc const& CompressionThreadTimer () {
            return compressionThreadTimer;
        }

        inline TICTOC::tictoc const& InputBufferThreadTimer () {
            return inputBufferThreadTimer;
        }

        /* bool readyForNewFrame(); 
         * returns false if input buffer is over full & frame will be discarded
         * returns true if new frame will be added to stack
         *
         */
        virtual bool readyForNewFrame(); 
        
        std::string generateTimingReport();

        
        virtual void numStacksWaiting (int &numToCompress, int &numToWrite);

    protected:
        CRITICAL_SECTION inputBufferCS;
        CRITICAL_SECTION activeStackCS;
        CRITICAL_SECTION compressingStackCS;
        CRITICAL_SECTION imageStacksCS;
        CRITICAL_SECTION outfileCS;
        CRITICAL_SECTION writingStackCS;
        CRITICAL_SECTION compressedStacksCS;
        CRITICAL_SECTION stacksToWriteCS;
        

        HANDLE compressionThread;
        HANDLE writingThread;
        HANDLE inputBufferThread;

        int maxCompressionThreads;
         bool compressionThreadActive;
        bool writingThreadActive;
        bool inputBufferThreadActive;
        
        bool stacksLeftToCompress;
        bool stacksLeftToWrite;

        TICTOC::tictoc nonthreadedTimer;
        TICTOC::tictoc compressionThreadTimer;
        TICTOC::tictoc writingThreadTimer;
        TICTOC::tictoc inputBufferThreadTimer;
        
        int numTimesNewFrameCalled;
        int numTimesFramePassedToInputBuffer;
        int numTimesFramePassedToStackCompressor;
        int numTimesFrameIsCompressed;
        int numTimesFrameIsNotRecorded;
        
        unsigned __stdcall compressionThreadFunction();
        unsigned __stdcall writingThreadFunction();
        unsigned __stdcall inputBufferThreadFunction();
        
        static unsigned __stdcall startCompressionThread(void *ptr);
        static unsigned __stdcall startWritingThread(void *ptr);
        static unsigned __stdcall startInputBufferThread(void *ptr);
        
        typedef std::pair<IplImage *, ImageMetaData *> InputImT;
        std::queue<InputImT> inputBuffer;
        
        virtual bool inputBufferEmpty();
        virtual int inputBufferLength();
        
        virtual void createStack();
       
        
        virtual void addFrameToStack(IplImage **im, ImageMetaData *metadata);
        
        virtual void setCompressionStack();
        virtual void setWritingStack();
        virtual void mergeCompressedStacks();
        virtual void emptyInputBuffer();
        
       

    private:
        WindowsThreadStackCompressor(const WindowsThreadStackCompressor& orig);
        void init();

    };
#else
    #include "LinearStackCompressor.h"
    typedef LinearStackCompressor WindowsThreadStackCompressor;
#endif

#endif	/* WINDOWSTHREADSTACKCOMPRESSOR_H */

