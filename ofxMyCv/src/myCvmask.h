#ifndef MYCVMASK_H
#define MYCVMASK_H

#include"myUtilities.h"

class myCvmask : myUtilities
{
    public:
        myCvmask();
        virtual ~myCvmask();
        IplImage* getMask(IplImage* input);
        void setCoordinate(int index, CvPoint coord);
        void setAsRectangle(CvRect rect);
        CvPoint getCoordinate(int index);
        CvRect getRect();
        void draw(int x, int y, float scale);
        void draw(int x, int y);

    protected:
    private:

    IplImage *mask, *output;
    CvPoint maskPts[4];
    CvRect maskRect;
    bool maskChanged;

};

#endif // MYCVMASK_H
