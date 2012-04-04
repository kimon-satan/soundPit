#ifndef MY_UTILITIES_H
#define MY_UTILITIES_H
#include "ofMain.h"
#include "ofxOpenCv.h"

class myUtilities
{
    public:
        myUtilities();
        virtual ~myUtilities();
        void drawIplImage(IplImage *image, int x, int y , GLfloat xZoom, GLfloat yZoom);


    protected:


    private:



};

#endif //
