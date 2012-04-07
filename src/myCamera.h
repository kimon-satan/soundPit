#ifndef MYCAMERA_H
#define MYCAMERA_H

#include "ofMain.h"
class myCamera: public ofCamera
{
    public:
        myCamera();
        virtual ~myCamera();
        ofVec3f getZPlaneProjection(ofVec2f screenPoint, ofRectangle viewport);
        void begin(ofRectangle viewport = ofGetCurrentViewport());
        void end(); // Overrides virtual end
    protected:
    private:

    //matrices etc
    ofRectangle viewportRect;
    GLint viewport[4];
    GLdouble matM[16], matP[16];




};

#endif // MYCAMERA_H
