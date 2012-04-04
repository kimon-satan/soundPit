#ifndef MY_COLOUR_CHANNEL_H
#define MY_COLOUR_CHANNEL_H
#include "ofxOpenCv.h"
#include "ofxCvMyCv.h"

class myColourChannel : myUtilities
{
    public:
        myColourChannel();
        virtual ~myColourChannel();

        void allocateImages(CvSize vidSize);
        void setHistFromSample(IplImage* sample, CvRect selection);
        void recalcSampleImages(IplImage* sample);
        void recalcHistImg(int hist);

        void newHist();
        void resetHist();

        void drawSampleBp(int x, int y, int mode); //0: justthisHist, 1: allcombined
        void drawHistogram(int x, int y);
        void drawVidBp(int x, int y);
        void drawVMask(int x, int y);
        void drawMenu(int x, int y);
        void keyPressed(int key);

        IplImage* get_vid_bp();

        void backProject(IplImage* vid_image);

        int selectedHist, num_hists;
        int selectedItem, colIndex;
	     int h_bins, s_bins;

        bool enabled, selected;
	int erode, dilate, gauss, thresh_v [2],  bp_thresh;
        vector <CvHistogram*> histograms;

    protected:



    private:

        IplImage *sample_img, *sample_hsv, *sample_h, *sample_s, *sample_v, *sample_bp, *sample_comb;
        IplImage *temp_a, *temp_b;
        IplImage *sample_planes[2];
        IplImage *vid_img, *vid_hsv, *vid_h, *vid_s, *vid_v, *vid_bp;
        IplImage *vid_planes[2];
        IplImage *v_mask;

        vector <IplImage*> hist_imgs;
		ofxCvGrayscaleImage histDisp, vMaskDisp, sampleBpDisp, sampleCombDisp, vidBpDisp;
		
   
        float h_ranges[2], s_ranges[2];

};

#endif // BGSEGMENTER_H
