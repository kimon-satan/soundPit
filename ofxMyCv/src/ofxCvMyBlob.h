/*
* ofxCvBlob.h
* openFrameworks
*
* A blob is a homogenous patch represented by a polygonal contour.
* Typically a blob tracker uses the contour to figure out the blob's
* persistence and "upgrades" it with ids and other temporal
* information.
*
*/


#ifndef OFX_CV_MY_BLOB_H
#define OFX_CV_MY_BLOB_H


#include "ofxCvBlob.h"

class ofxCvMyBlob : public ofxCvBlob {

    public:

        CvPoint2D32f        centroid_cv;
        CvRect              bounding_cv;
        CvBox2D             box2D_cv;
        CvConnectedComp     cont_cv;

        int                isFromLft;
        bool db_allct;

        //----------------------------------------
        ofxCvMyBlob() {
            area 		= 0.0f;
            length 		= 0.0f;
            hole 		= false;
            nPts        = 0;
            cont_cv.area = 0;
            bounding_cv = cvRect(0,0,0,0);
            box2D_cv.size.width = 0;
            box2D_cv.size.height = 0;
            isFromLft = 10;
            db_allct = false;

        }


};


#endif


