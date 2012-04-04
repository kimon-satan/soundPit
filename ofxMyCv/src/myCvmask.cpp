#include "myCvmask.h"

myCvmask::myCvmask()
{
    maskChanged = true;

    maskPts[0] = cvPoint(0,90);
    maskPts[1] = cvPoint(160,0);
    maskPts[2] = cvPoint(320,90);
    maskPts[3] = cvPoint(160,180);


}

IplImage* myCvmask::getMask(IplImage* input){

    if(maskChanged){
        mask  = cvCreateImage(cvGetSize(input),8,1);
        output = cvCreateImage(cvGetSize(input),input->depth,input->nChannels);
        cvZero(mask);
        cvFillConvexPoly(mask,&maskPts[0], 4,cvScalar(255,255,255));

        maskChanged = false;
    }

    cvZero(output);
    cvCopy(input,output,mask);

    return output;
}

void myCvmask::setCoordinate(int index, CvPoint coord){

        maskPts[index] = coord;
        maskChanged = true;

}

CvPoint myCvmask::getCoordinate(int index){

    return maskPts[index];

}

void myCvmask::setAsRectangle(CvRect rect){

    maskRect = rect;
    maskPts[0].x = rect.x; maskPts[0].y = rect.y;
    maskPts[1].x = rect.x + rect.width; maskPts[1].y = rect.y;
    maskPts[2].x = rect.x + rect.width; maskPts[2].y = rect.y + rect.height;
    maskPts[3].x = rect.x; maskPts[3].y = rect.y + rect.height;
    maskChanged = true;

}

CvRect myCvmask::getRect(){

return maskRect;

}
void myCvmask::draw(int x, int y){

draw(x,y,1);

}

void myCvmask::draw(int x, int y, float scale){

    ofPushStyle();
    glPushMatrix();

        ofSetLineWidth(2);
        ofNoFill();
        glTranslatef((float)x,(float)y,0);
        glScalef(scale,scale,1.0);
        ofSetColor(255,0,0);

        ofBeginShape();
        ofVertex(maskPts[0].x,maskPts[0].y);
        ofVertex(maskPts[1].x,maskPts[1].y);
        ofVertex(maskPts[2].x,maskPts[2].y);
        ofVertex(maskPts[3].x,maskPts[3].y);
        ofVertex(maskPts[0].x,maskPts[0].y);
        ofEndShape();

    glPopMatrix();
    ofPopStyle();

}

myCvmask::~myCvmask()
{
    //dtor
}
