#ifndef BGSEGMENTER_H
#define BGSEGMENTER_H
#include "ofxOpenCv.h"

class bgSegmenter
{
    public:
        bgSegmenter();
        virtual ~bgSegmenter();

        void AllocateImages(CvSize sz);
        void accumulateBackground(IplImage *I);
        void setHighThreshold( float scale );
        void setLowThreshold( float scale );
        void createModelsfromStats();
        void backgroundDiff( IplImage *I, IplImage *Imask );
        void learnNewBackground();
		void draw(int x, int y);

        IplImage * segment(IplImage *frame);
        IplImage * getMask();
        IplImage * getMulMask();
	
	ofxCvGrayscaleImage maskDisp;

        //float 3 channels

        IplImage *IavgF,*IdiffF,*IprevF,*IhiF,*IlowF;
        IplImage *Iscratch,*Iscratch2;

        //float 1 channels
        IplImage *Igray1,*Igray2,*Igray3;
        IplImage *Ilow1, *Ilow2, * Ilow3;
        IplImage *Ihi1, *Ihi2, *Ihi3;

        IplImage *Imaskt;
        IplImage *view_col;

        IplImage *mask1, *mask3, *scratch;

        //counts number of imagescvfloat Icount;

        float Icount;
        float high_scale;
        float low_scale;
        int learnCount;
        int dilate;
        int erode;
        int thresh;



    protected:
    private:

        bool allocated;


};

#endif // BGSEGMENTER_H
