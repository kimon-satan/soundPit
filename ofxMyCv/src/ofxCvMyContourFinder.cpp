
#include "ofxCvMyContourFinder.h"

//--------------------------------------------------------------------------------
bool mysort_carea_compare( const CvSeq* a, const CvSeq* b) {
	// use opencv to calc size, then sort based on size
	float areaa = fabs(cvContourArea(a, CV_WHOLE_SEQ));
	float areab = fabs(cvContourArea(b, CV_WHOLE_SEQ));

    //return 0;
	return (areaa > areab);
}


//--------------------------------------------------------------------------------


ofxCvMyContourFinder::ofxCvMyContourFinder() {

    _width = 0;
    _height = 0;
	myMoments = (CvMoments*)malloc( sizeof(CvMoments) );
	reset();
}


ofxCvMyContourFinder::~ofxCvMyContourFinder() {
	free( myMoments );
}

void ofxCvMyContourFinder::reset() {

    blobs.clear();
    cvSeqBlobs.clear();
    myblobs.clear();
    nBlobs = 0;
}
//--------------------------------------------------------------------------------
int ofxCvMyContourFinder::findContours( IplImage*  input,
									  int minArea,
									  int maxArea,
									  int nConsidered,
									  bool bFindHoles,
                                      int approximation) {


    // get width/height disregarding ROI

    _width = input->width;
    _height = input->height;

	reset();

	// opencv will clober the image it detects contours on, so we want to
    // copy it into a copy before we detect contours.  That copy is allocated
    // if necessary (necessary = (a) not allocated or (b) wrong size)
	// so be careful if you pass in different sized images to "findContours"
	// there is a performance penalty, but we think there is not a memory leak
    // to worry about better to create mutiple contour finders for different
    // sizes, ie, if you are finding contours in a 640x480 image but also a
    // 320x240 image better to make two ofxCvMyContourFinder objects then to use
    // one, because you will get penalized less.



	if( !inputCopy ) {
		inputCopy = cvCreateImage(cvSize(_width,_height), input->depth, input->nChannels);
	} else if( inputCopy->width != _width || inputCopy->height != _height ) {
        // reallocate to new size
        cvReleaseImage(&inputCopy);
        inputCopy = cvCreateImage(cvSize(_width,_height), input->depth, input->nChannels);
	}

    cvSetImageROI(inputCopy, cvGetImageROI(input));
    cvCopy(input, inputCopy);

	contour_storage = cvCreateMemStorage( 1000 );
	storage	= cvCreateMemStorage( 1000 );

	CvContourRetrievalMode  retrieve_mode = (bFindHoles) ? CV_RETR_LIST : CV_RETR_EXTERNAL;
	CvContourScanner scanner = cvStartFindContours( inputCopy, contour_storage,
                    sizeof(CvContour), retrieve_mode, CV_CHAIN_APPROX_SIMPLE);

    CvSeq* c;
    int numCont = 0;

    while(( c = cvFindNextContour(scanner)) != NULL)
    {

        CvSeq* c_new;

        if( approximation > 0){

            c_new = cvApproxPoly(
                c,
                sizeof(CvContour),
                contour_storage,
                CV_POLY_APPROX_DP,
                approximation,
                0
            );

        } else {

            c_new = cvConvexHull2(
                c,
                contour_storage,
                CV_CLOCKWISE,
                1
                );

        }


        float area = fabs( cvContourArea(c_new, CV_WHOLE_SEQ) );
		if( (area > minArea) && (area < maxArea) ) {
            cvSeqBlobs.push_back(c_new);
		}
        numCont++;

    }

//    cvEndFindContours(scanner);


	// sort the pointers based on size
	if( cvSeqBlobs.size() > 1 ) {
        sort( cvSeqBlobs.begin(), cvSeqBlobs.end(), mysort_carea_compare );
	}


	// now, we have cvSeqBlobs.size() contours, sorted by size in the array
    // cvSeqBlobs let's get the data out and into our structures that we like
	for( int i = 0; i < MIN(nConsidered, (int)cvSeqBlobs.size()); i++ ) {
		myblobs.push_back( ofxCvMyBlob() );
		float area = cvContourArea( cvSeqBlobs[i], CV_WHOLE_SEQ );
		CvRect rect	= cvBoundingRect( cvSeqBlobs[i], 0 );
		cvMoments( cvSeqBlobs[i], myMoments );

		myblobs[i].area                     = fabs(area);
		myblobs[i].hole                     = area < 0 ? true : false;
		myblobs[i].length 			      = cvArcLength(cvSeqBlobs[i]);
		myblobs[i].boundingRect.x           = rect.x;
		myblobs[i].boundingRect.y           = rect.y;
		myblobs[i].boundingRect.width       = rect.width;
		myblobs[i].boundingRect.height      = rect.height;

		if(cvSeqBlobs[i]->total >= 6){
        myblobs[i].box2D_cv                 = cvMinAreaRect2(cvSeqBlobs[i]);
		}
		myblobs[i].bounding_cv              = cvBoundingRect(cvSeqBlobs[i]);

		double x = (myMoments->m10 / myMoments->m00);
		double y = (myMoments->m01 / myMoments->m00);
		myblobs[i].centroid.x 			  = (int)x;
		myblobs[i].centroid.y 			  = (int)y;
		myblobs[i].centroid_cv              = cvPoint2D32f(x,y);

       // myblobs[i].contour = (CvPoint *)malloc(cvSeqBlobs[i]->total * sizeof(CvPoint));
       // cvCvtSeqToArray(cvSeqBlobs[i], myblobs[i].contour, CV_WHOLE_SEQ);

		// get the points for the blob:

		CvPoint          pt;
		CvSeqReader       reader;
		cvStartReadSeq( cvSeqBlobs[i], &reader, 0 );


    	for( int j=0; j < cvSeqBlobs[i]->total; j++ ) {

			CV_READ_SEQ_ELEM( pt, reader );
            myblobs[i].pts.push_back( ofPoint((float)pt.x, (float)pt.y) );

		}

		myblobs[i].nPts = myblobs[i].pts.size();

	}

    nBlobs = myblobs.size();

	// Free the storage memory.
	// Warning: do this inside this function otherwise a strange memory leak
	if( contour_storage != NULL ) { cvReleaseMemStorage(&contour_storage); }
	if( storage != NULL ) { cvReleaseMemStorage(&storage); }

	return nBlobs;

}

void ofxCvMyContourFinder::draw( float x, float y, float w, float h ) {

    float scalex =
     0.0f;
    float scaley = 0.0f;
    if( _width != 0 ) { scalex = w/_width; } else { scalex = 1.0f; }
    if( _height != 0 ) { scaley = h/_height; } else { scaley = 1.0f; }

    if(bAnchorIsPct){
        x -= anchor.x * w;
        y -= anchor.y * h;
    }else{
        x -= anchor.x;
        y -= anchor.y;
    }

	// ---------------------------- draw the bounding rectangle
	ofSetColor(0xDD00CC);
    glPushMatrix();
    glTranslatef( x, y, 0.0 );
    glScalef( scalex, scaley, 0.0 );

	ofNoFill();
	for( int i=0; i<(int)myblobs.size(); i++ ) {
		ofRect( myblobs[i].boundingRect.x, myblobs[i].boundingRect.y,
                myblobs[i].boundingRect.width, myblobs[i].boundingRect.height );
	}

	// ---------------------------- draw the blobs
	ofSetColor(0x00FFFF);

	for( int i=0; i<(int)myblobs.size(); i++ ) {
		ofNoFill();
		ofBeginShape();
		for( int j=0; j<myblobs[i].nPts; j++ ) {
			ofVertex( myblobs[i].pts[j].x, myblobs[i].pts[j].y );
		}
		ofEndShape();

	}
	glPopMatrix();

	ofSetColor(255,255,255);
}



