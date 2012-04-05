#include "myColourChannel.h"

myColourChannel::myColourChannel()
{

    colIndex = 0;
    selectedHist = 0;
    selectedItem = 0;
    num_hists = 1;
    enabled = false;
    selected = false;

    h_bins = 30; s_bins =32;
	int hist_size[] = {h_bins, s_bins };
	h_ranges[0] = 0; h_ranges[1] = 180;
    s_ranges[0] = 0; s_ranges[1] = 256;

    float * ranges[] = {h_ranges, s_ranges};

    CvHistogram *tempHist;
    IplImage *tempImg;

    for(int i =0; i < num_hists; i++){
    tempHist = cvCreateHist(2, hist_size, CV_HIST_ARRAY,ranges,1);
    histograms.push_back(tempHist);
    tempImg = cvCreateImage(cvSize(h_bins*5,s_bins *5),8,1);
    hist_imgs.push_back(tempImg);
    }

	histDisp.allocate(h_bins * 5, s_bins * 5);


    dilate = 1;
    erode = 1;
    gauss = 3;
    bp_thresh = 50;
    thresh_v[0] = 50;
    thresh_v[1] = 200;

}

void myColourChannel::allocateImages(CvSize vidSize){

    sample_img = cvCreateImage( vidSize, 8, 3 );
	sample_hsv = cvCreateImage( vidSize, 8, 3 );
	sample_bp = cvCreateImage( vidSize, 8, 1 );
	sample_comb = cvCreateImage( vidSize, 8, 1 );
	temp_a = cvCreateImage( vidSize, 8, 1 );
	temp_b = cvCreateImage( vidSize, 8, 1 );

	sample_h = cvCreateImage( vidSize, 8, 1 );
	sample_s = cvCreateImage( vidSize, 8, 1 );
	sample_v = cvCreateImage( vidSize, 8, 1 );

    sample_planes[0] =  sample_h;
	sample_planes[1] =  sample_s;

	v_mask = cvCreateImage( vidSize, 8, 1 );
    vid_bp = cvCreateImage( vidSize, 8, 1 );
    vid_h = cvCreateImage( vidSize, 8, 1 );
	vid_s = cvCreateImage( vidSize, 8, 1 );
	vid_v = cvCreateImage( vidSize, 8, 1 );

    vid_planes[0] = vid_s;
    vid_planes[1] = vid_h;

	vMaskDisp.allocate(vidSize.width, vidSize.height);
	sampleCombDisp.allocate(vidSize.width, vidSize.height);
	sampleBpDisp.allocate(vidSize.width, vidSize.height);
	vidBpDisp.allocate(vidSize.width, vidSize.height);



}

void myColourChannel::newHist(){

    CvHistogram *tempHist;
    IplImage *tempImg;

    int hist_size[] = {h_bins, s_bins };
    float * ranges[] = {h_ranges, s_ranges};
    tempHist = cvCreateHist(2, hist_size, CV_HIST_ARRAY,ranges,1);
    cvClearHist(tempHist);
    histograms.push_back(tempHist);
    tempImg = cvCreateImage(cvSize(h_bins*5,s_bins *5),8,1);
    cvZero(tempImg);
    hist_imgs.push_back(tempImg);

    num_hists +=1;
    selectedHist = histograms.size()-1;

}

void myColourChannel::resetHist(){

    for(int i = 0; i < num_hists; i++){

		cvReleaseHist(&histograms[i]);

    }

    histograms.clear();
    num_hists = 0;
    newHist();

}

void myColourChannel::recalcHistImg(int hist){

    cvZero(hist_imgs[hist]);
    float max_value = 0;
    cvGetMinMaxHistValue(histograms[hist], 0, &max_value, 0, 0);

	for(int h = 0; h < h_bins; h++)
	{
        for(int s = 0; s < s_bins; s++)
        {

             float bin_val = cvQueryHistValue_2D(histograms[hist],h,s);
             int intensity = cvRound(bin_val * 500/max_value);

             cvRectangle(
                           hist_imgs[hist],
                           cvPoint(h*5, s*5),cvPoint(h*5 + 5, s*5 +5),
                           cvScalarAll(intensity),
                           CV_FILLED
                           );
        }
    }

		histDisp = hist_imgs[selectedHist];

}

void myColourChannel::setHistFromSample(IplImage* sample, CvRect selection){

    cvCvtColor(sample, sample_hsv, CV_BGR2HSV);

    cvCvtPixToPlane(sample_hsv, sample_h, sample_s, sample_v, 0);

    cvClearHist(histograms[selectedHist]);

    cvSetImageROI( sample_h, selection);
    cvSetImageROI( sample_s, selection);

    cvCalcHist(sample_planes, histograms[selectedHist], 0);
   // cvThreshHist(histograms[selectedHist],3.0);
    float max_value = 0;
    cvGetMinMaxHistValue(histograms[selectedHist], 0, &max_value, 0, 0);
    cvConvertScale( histograms[selectedHist]->bins, histograms[selectedHist]->bins,(max_value > 0) ? 256. / max_value : 0., 0 );

    cvResetImageROI(sample_h);
    cvResetImageROI(sample_s);

    recalcSampleImages(sample);
    recalcHistImg(selectedHist);



}

void myColourChannel::recalcSampleImages(IplImage* sample){

    cvCvtColor(sample, sample_hsv, CV_BGR2HSV);
    cvCvtPixToPlane(sample_hsv, sample_h, sample_s, sample_v, 0);
    cvZero(sample_bp);
    cvCalcBackProject(sample_planes, sample_bp, histograms[selectedHist]);

    cvZero(sample_comb);

    for(int i = 0; i < histograms.size(); i++){
        temp_a = sample_comb;
        cvCalcBackProject(sample_planes, temp_b, histograms[i]);
        cvAdd(temp_a,temp_b,sample_comb);
    }

	sampleCombDisp = sample_comb;
	sampleBpDisp = sample_bp;

}

void myColourChannel::backProject(IplImage* vid_hsv){

    if(enabled)
    {
        cvCvtPixToPlane(vid_hsv, vid_h, vid_s, vid_v, 0);
        vid_planes[0] = vid_h;
        vid_planes[1] = vid_s;

        cvZero(vid_bp);
        cvZero(v_mask);

        cvInRangeS(
                   vid_v,
                   cvScalarAll(thresh_v[0]),
                   cvScalarAll(thresh_v[1]),
                   v_mask);


        cvThreshold(v_mask, v_mask, 1.0,256, CV_THRESH_BINARY);


        for(int j = 0; j < histograms.size(); j++){
                    temp_a = vid_bp;
                    cvCalcBackProject(vid_planes, temp_b, histograms[j]);
                    cvInRangeS(temp_b,cvScalarAll(bp_thresh),cvScalarAll(256),temp_b);
                    cvAdd(temp_a,temp_b,vid_bp);
                }

        cvMul(vid_bp, v_mask, vid_bp, 1.0 );
        if(erode!= 0)cvErode(vid_bp, vid_bp, NULL, erode);
        if(dilate != 0)cvDilate(vid_bp, vid_bp, NULL, dilate);
        cvSmooth(vid_bp, vid_bp,CV_GAUSSIAN,gauss,gauss);

		vMaskDisp = v_mask;
		vidBpDisp = vid_bp;

		vidBpDisp.flagImageChanged();
		vMaskDisp.flagImageChanged();

    }else{

    cvZero(vid_bp);

    }



}

IplImage* myColourChannel::get_vid_bp(){

return vid_bp;

}

void myColourChannel::drawHistogram(int x, int y){

	histDisp.draw(x,y);


}

void myColourChannel::drawSampleBp(int x, int y, int mode){

    if(mode == 0){

		sampleBpDisp.draw(x,y);


    }else if(mode == 1){

		sampleCombDisp.draw(x,y);

    }

}

void myColourChannel::drawVidBp(int x, int y){

    vidBpDisp.draw(x,y);

}

void myColourChannel::drawVMask(int x, int y){

    vMaskDisp.draw(x,y);

}

void myColourChannel::drawMenu(int x, int y){

glPushMatrix();

glTranslatef(x,y,0);


    glTranslatef(0,15,0);
    if(selected){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
    ofDrawBitmapString("Historgram Settings:",0,0);

    int cSetMenuColors[10];

    for(int i = 0; i < 10; i++){

    if(selectedItem == i && selected){
    cSetMenuColors[i] = 0xff0000;
    }else{cSetMenuColors[i] = 0xffffff;}

    }

    glTranslatef(0,15,0);
    ofSetHexColor(cSetMenuColors[0]);
    ofDrawBitmapString("selectedColour: " + ofToString(colIndex, 0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(cSetMenuColors[1]);
    ofDrawBitmapString("selectedHist: " + ofToString(selectedHist, 0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(cSetMenuColors[2]);
    ofDrawBitmapString("numHists: " + ofToString(num_hists, 0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(cSetMenuColors[3]);
    ofDrawBitmapString("bp_thresh: " + ofToString(bp_thresh,0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(cSetMenuColors[4]);
    ofDrawBitmapString("erode: " + ofToString(erode, 0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(cSetMenuColors[5]);
    ofDrawBitmapString("dilate: " + ofToString(dilate, 0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(cSetMenuColors[6]);
    ofDrawBitmapString("gauss: " + ofToString(gauss, 0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(cSetMenuColors[7]);
    ofDrawBitmapString("lower_vthresh: " + ofToString(thresh_v[0], 0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(cSetMenuColors[8]);
    ofDrawBitmapString("upper_vthresh: " + ofToString(thresh_v[1], 0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(cSetMenuColors[9]);
    ofDrawBitmapString("enabled: " + ofToString(enabled, 0),0,0);


    ofSetColor(255,255,255);

glPopMatrix();


}

void myColourChannel::keyPressed(int key){

    switch (key)
    {


    case OF_KEY_UP:
        if(selectedItem == 1){
		selectedHist +=1;
		selectedHist = selectedHist%num_hists;
		histDisp = hist_imgs[selectedHist];
		}
		if(selectedItem == 2){newHist();}
        if(selectedItem == 3){

			bp_thresh += 1;
		//	bp_thresh = min(bp_thresh, 255);
		}
        if(selectedItem == 4){erode +=1;}
        if(selectedItem == 5){dilate +=1;}
        if(selectedItem == 6){gauss+=2;}
        if(selectedItem == 7){thresh_v[0]+=1;}
        if(selectedItem == 8){thresh_v[1]+=1;}
        if(selectedItem == 9){enabled =true;}


    break;



    case OF_KEY_DOWN:
        if(selectedItem == 2)resetHist();
        if(selectedItem == 3){bp_thresh -=1;
		//	bp_thresh = max(bp_thresh, 0);
		}
        if(selectedItem == 4){erode -=1;}
        if(selectedItem == 5){dilate -=1;}
        if(selectedItem == 6){gauss-=2;}
        if(selectedItem == 7){thresh_v[0]-=1;}
        if(selectedItem == 8){thresh_v[1] -=1;}
        if(selectedItem == 9){enabled = false;}
    break;



    }

}

myColourChannel::~myColourChannel()
{
   cvReleaseImage(&sample_img);
   cvReleaseImage(&sample_bp);
   cvReleaseImage(&sample_comb);
   cvReleaseImage(&sample_hsv);
   cvReleaseImage(&sample_h);
   cvReleaseImage(&sample_s);
   cvReleaseImage(&sample_v);
   cvReleaseImage(&vid_bp);
   cvReleaseImage(&vid_img);
   cvReleaseImage(&vid_hsv);
   cvReleaseImage(&vid_h);
   cvReleaseImage(&vid_s);
   cvReleaseImage(&vid_v);
   cvReleaseImage(&v_mask);
   cvReleaseImage(&temp_a);
   cvReleaseImage(&temp_b);

    for(int i =0; i < hist_imgs.size(); i++){

    cvReleaseImage(&hist_imgs[i]);

    }

}
