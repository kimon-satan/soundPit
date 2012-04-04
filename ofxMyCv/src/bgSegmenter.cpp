#include "bgSegmenter.h"

bgSegmenter::bgSegmenter()
{
    //ctor
    allocated = false;
    erode = 1;
    dilate = 1;
    thresh = 100;
    high_scale = 7.0;
    low_scale = 6.0;
    learnCount = 30;
}

void bgSegmenter::AllocateImages(CvSize sz){

IavgF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
IdiffF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
IprevF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
IhiF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
IlowF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
Ilow1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
Ilow2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
Ilow3 =cvCreateImage(sz, IPL_DEPTH_32F, 1);
Ihi1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
Ihi2 =cvCreateImage(sz, IPL_DEPTH_32F, 1);
Ihi3 =cvCreateImage(sz, IPL_DEPTH_32F, 1);
mask1 = cvCreateImage( sz, IPL_DEPTH_8U, 1 );
mask3 = cvCreateImage( sz, IPL_DEPTH_8U, 3 );
cvZero(IavgF);
cvZero(IdiffF);
cvZero(IprevF);
cvZero(IhiF);
cvZero(IlowF);
Icount = 0.0000001;

Iscratch = cvCreateImage(sz, IPL_DEPTH_32F,3);
Iscratch2 = cvCreateImage(sz, IPL_DEPTH_32F,3);
Igray1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
Igray2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
Igray3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
Imaskt = cvCreateImage(sz, IPL_DEPTH_8U,1);
scratch = cvCreateImage(sz, IPL_DEPTH_8U,3);

cvZero(Iscratch);
cvZero(Iscratch2);

allocated = true;

	maskDisp.allocate(sz.width, sz.height);

	
}

//learn bg stats for one more frame
//i is a color sample of bg, 3-channel, 8u

void bgSegmenter::accumulateBackground(IplImage *I){

static int first = 1;
cvCvtScale(I, Iscratch, 1, 0);
if(!first){
cvAcc(Iscratch, IavgF);
cvAbsDiff(Iscratch, IprevF, Iscratch2);
cvAcc(Iscratch2,IdiffF);
}

first = 0;
cvCopy( Iscratch, IprevF);

}

void bgSegmenter::setHighThreshold( float scale ) {
    high_scale = scale;
	cvConvertScale( IdiffF, Iscratch, scale );
	cvAdd( Iscratch, IavgF, IhiF );
	cvSplit( IhiF, Ihi1, Ihi2, Ihi3, 0 );
}

void bgSegmenter::setLowThreshold( float scale ) {
    low_scale = scale;
	cvConvertScale( IdiffF, Iscratch, scale );
	cvSub( IavgF, Iscratch, IlowF );
	cvSplit( IlowF, Ilow1, Ilow2, Ilow3, 0 );
}


void bgSegmenter::createModelsfromStats(){

cvConvertScale(IavgF, IavgF,(double)(1.0/Icount),0);
cvConvertScale(IdiffF, IdiffF,(double)(1.0/Icount),0);

//make sure diff is something

cvAddS(IdiffF, cvScalar(1.0,1.0,1.0),IdiffF);
setHighThreshold(high_scale);
setLowThreshold(low_scale);

}


void bgSegmenter::backgroundDiff( IplImage *I, IplImage *Imask ){
	cvCvtScale( I, Iscratch, 1, 0);
	cvSplit( Iscratch, Igray1, Igray2, Igray3, 0 );

	// channel 1
	cvInRange( Igray1, Ilow1, Ihi1, Imask );
	// channel 2
	cvInRange( Igray2, Ilow2, Ihi2, Imaskt );
	cvAnd( Imask, Imaskt, Imask );
	// channel 3
	cvInRange( Igray3, Ilow3, Ihi3, Imask );
	cvAnd( Imask, Imaskt, Imask );

//	Invert results
	cvSubRS(Imask,cvScalarAll(255),Imask);
}

void bgSegmenter::learnNewBackground(){

Icount = 0;

}

void bgSegmenter::draw(int x, int y){
	
	maskDisp = mask1;
	maskDisp.draw(x,y);
	
}

IplImage* bgSegmenter::segment(IplImage *frame){

if(!allocated)AllocateImages(cvGetSize(frame));
cvCopy(frame, scratch);

Icount += 1.0;

		if( Icount < learnCount ){
			accumulateBackground( scratch );
		}else if( Icount == learnCount ){
			createModelsfromStats();
		}else{

			backgroundDiff(scratch, mask1 );

            //cvErode(mask1,mask1, CV_SHAPE_RECT, erode);
            //cvDilate(mask1,mask1, CV_SHAPE_RECT,dilate);

            cvMorphologyEx(mask1,mask1,0,0,CV_MOP_CLOSE, dilate);
            cvMorphologyEx(mask1,mask1,0,0,CV_MOP_OPEN, erode);


			cvCvtColor(mask1,mask3,CV_GRAY2BGR);
			cvNorm( mask3, mask3, CV_C, 0);
			cvThreshold(mask3, mask3, thresh, 1, CV_THRESH_BINARY); // still  adjusts sensitivity
			cvMul(scratch, mask3, scratch, 1.0 );
		}

return scratch;

}



IplImage * bgSegmenter::getMask(){

return mask1;

}

IplImage * bgSegmenter::getMulMask(){

return scratch;

}

bgSegmenter::~bgSegmenter()
{
    //dtor

    cvReleaseImage( &IavgF );
	cvReleaseImage( &IdiffF );
	cvReleaseImage( &IprevF );
	cvReleaseImage( &IhiF );
	cvReleaseImage( &IlowF );
	cvReleaseImage( &Ilow1 );
	cvReleaseImage( &Ilow2 );
	cvReleaseImage( &Ilow3 );
	cvReleaseImage( &Ihi1 );
	cvReleaseImage( &Ihi2 );
	cvReleaseImage( &Ihi3 );
	cvReleaseImage( &Iscratch );
	cvReleaseImage( &Iscratch2 );
	cvReleaseImage( &Igray1 );
	cvReleaseImage( &Igray2 );
	cvReleaseImage( &Igray3 );
	cvReleaseImage( &Imaskt );
	cvReleaseImage( &view_col);
}
