#include "myUtilities.h"

myUtilities::myUtilities()
{

}


void myUtilities::drawIplImage(IplImage *image, int x = 0, int y = 0, GLfloat xZoom = 1.0f, GLfloat yZoom = 1.0f){

	GLenum format;
	
	ofxCvColorImage tempCol;
	ofxCvColorImage tempLum;
	
	//doesn't work

	switch(image->nChannels) {
			case 1:
				tempLum.allocate(image->width, image->height);
				tempLum = image;
				tempLum.draw(x,y, image->width * xZoom, image->height * yZoom);
				break;
			case 2:
				break;
			case 3:
				tempCol.allocate(image->width, image->height);
				tempCol = image;
				tempCol.draw(x, y, image->width * xZoom, image->height * yZoom);
				break;
			default:
				return;
	}
	



}


myUtilities::~myUtilities()
{
    //dtor


}
