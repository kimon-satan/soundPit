/*
* ofxCvContourFinder.h
*
* Finds white blobs in binary images and identifies
* centroid, bounding box, area, length and polygonal contour
* The result is placed in a vector of ofxCvBlob objects.
*
*/

#ifndef OFX_CV_MY_CONTOUR_FINDER
#define OFX_CV_MY_CONTOUR_FINDER

#include "ofxCvMyBlob.h"
#include "ofxCvContourFinder.h"
#include <algorithm>

class ofxCvMyContourFinder : public ofxCvContourFinder{

  public:

    ofxCvMyContourFinder();
    virtual  ~ofxCvMyContourFinder();

    vector<ofxCvMyBlob>  myblobs;

    virtual int  findContours( IplImage* input,
                               int minArea, int maxArea,
                               int nConsidered, bool bFindHoles,
                               int bUseApproximation = 0);
                               // approximation = don't do points for all points
                               // of the contour, if the contour runs
                               // along a straight line, for example...

    virtual void  draw() { draw(0,0, _width, _height); };
    virtual void  draw( float x, float y ) { draw(x,y, _width, _height); };
    virtual void  draw( float x, float y, float w, float h );

  protected:

    IplImage*     inputCopy;

    virtual void reset();

};



#endif
