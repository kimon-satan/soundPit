#include "capture.h"

bool comparebyDistance (const distListItem a, const distListItem b) {
	return (a.dist < b.dist);
}
//--------------------------------------------------------------
capture::capture()
{

    width = 320;
    height = 240;
    learningBg = 30;

	ofSetFrameRate(32);

    //vidGrabber.setVerbose(true);
    //vidGrabber.setDeviceID(1);
    vidGrabber.initGrabber(width,height);

    vidImg.allocate(width,height);
    selectImg.allocate(width,height);
    zoomSelectImg.allocate(width,height);

    bgSeg.AllocateImages(cvSize(width, height));
    bgSeg.dilate = false;

    for(int col =0; col < 4; col ++){

        colChannels[col].allocateImages(cvSize(width,height));
        colChannels[col].colIndex = col;
        colChannels[col].erode = 0;
        colChannels[col].enabled = true;

    }

    vidImage = cvCreateImage( cvGetSize(vidImg.getCvImage()), 8, 3 );
	vidImage->origin = vidImg.getCvImage()->origin;

	vidhsv = cvCreateImage( cvGetSize(vidImg.getCvImage()), 8, 3 );
	//segImg = cvCreateImage( cvGetSize(selectImg.getCvImage()), 8, 3 );
	segImg.allocate(vidImg.width, vidImg.height);
	selImage = cvCreateImage( cvGetSize(selectImg.getCvImage()), 8, 3 );

    masker.setAsRectangle(cvRect(20,20,280,200));
    //maskedImg = cvCreateImage( cvGetSize(vidImg.getCvImage()), 8, 3 );
	maskedImg.allocate(width, height);

   	minBlob = 40;
	maxBlob = 1000;
    db_thresh = 10;

	selection = cvRect(100,100,10,10);

	selectChanged = true;
	learnBg = true;
	learnImage = true;
	zoomSel = false;

	selectedColour = 0;
	selectedView = 0;
	zoom = 5;
	zx = 0;
	zy = 0;
	sample_view = 0;



}

void capture::createZimg(){

	if(!zoomSel){
		selectImg.setROI(zx,zy,320/zoom,240/zoom);
		zoomSelectImg.clear();
		zoomSelectImg.allocate(320/zoom, 240/zoom);
		zoomSelectImg.setFromPixels(selectImg.getRoiPixels(),320/zoom,240/zoom);
		zoomSelectImg.resize(320,240);
		selectImg.resetROI();
	}else{
		zoomSelectImg = selectImg;
	}
}



//--------------------------------------------------------------
bool capture::update(trackingObject t_objs[][10] , int size){

	ofBackground(100,100,100);

    vidGrabber.grabFrame();
    bNewFrame = true; //vidGrabber.isFrameNew();

	if(learnImage){
		if(bNewFrame){
            selectImg.setFromPixels(vidGrabber.getPixels(), 320,240);
            //cvEqualizeHist(selectImg, selectImg);
            createZimg();
            learnImage = false;
		}

	}

	cvCopy(zoomSelectImg.getCvImage(), selImage);
	if(newZbp)colChannels[selectedColour].recalcSampleImages(selImage);
	if(selectChanged)colChannels[selectedColour].setHistFromSample(selImage, selection);

	selectChanged = false;
	newZbp = false;


	if(bNewFrame){

        if(learningBg > 0)learningBg-=1;

        vidImg.setFromPixels(vidGrabber.getPixels(), 320,240);
        maskedImg = masker.getMask(vidImg.getCvImage());

        if(learnBg){
			bgSeg.learnNewBackground();
			learnBg = false;
			learningBg = 40;
        }

        cvCvtColor(maskedImg.getCvImage(), vidhsv, CV_BGR2HSV );
        segImg = bgSeg.segment(vidhsv);

        //for each colour

		for(int colour = 0; colour < 4; colour ++)
		{

			//do the backproject

			colChannels[colour].backProject(segImg.getCvImage());

			//meanshift any exisiting blobs
			for(int blob = 0; blob < 10; blob++){

				if(t_objs[colour][blob].assigned){

					cvMeanShift( colChannels[colour].get_vid_bp(), t_objs[colour][blob].bounding_cv,
								cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
								&t_objs[colour][blob].cont_cv);

					//check for db_allcts in camshifts

					float distance = 100;

					ofVec2f ct_point;

					ct_point.set(t_objs[colour][blob].cont_cv.rect.x +
								 t_objs[colour][blob].cont_cv.rect.width/2,
								 t_objs[colour][blob].cont_cv.rect.y +
								 t_objs[colour][blob].cont_cv.rect.height/2);

					for(int sb = 0; sb < 10; sb++){

						if(t_objs[colour][sb].assigned){
							if(sb != blob){
                                ofPoint sb_point;
                                sb_point.set(t_objs[colour][sb].bounding_cv.x +
											 t_objs[colour][sb].bounding_cv.width/2 ,
											 t_objs[colour][sb].bounding_cv.y +
											 t_objs[colour][sb].bounding_cv.height/2);

                                distance = min(distance, ct_point.distance(sb_point));
							}
						}

					}

					if(distance > db_thresh && t_objs[colour][blob].cont_cv.area > 0){
                        t_objs[colour][blob].bounding_cv = t_objs[colour][blob].cont_cv.rect;
                        t_objs[colour][blob].ghostReset();

					}else if(distance < 1){
                        t_objs[colour][blob].cont_cv.area = 0;
					}
				}

			}

			//delete any lost blobs;
			for(int sb = 0; sb < 10; sb++){

				if(t_objs[colour][sb].cont_cv.area == 0 && t_objs[colour][sb].ghost > 5)
				{
					t_objs[colour][sb].assigned = false;
					t_objs[colour][sb].present = false;
					t_objs[colour][sb].reset();

				}else if(t_objs[colour][sb].assigned && t_objs[colour][sb].cont_cv.area == 0){

					t_objs[colour][sb].ghostIncrement();
					// printf("%i \n",t_objs[colour][sb].ghost);

				}

			}

			//search for new blobs
			contFinder[colour].findContours(
											colChannels[colour].get_vid_bp(),
											minBlob,
											maxBlob,
                                            10,
                                            false,
                                            0);

			//check that there's no double allocation
			for(int cb = 0; cb < contFinder[colour].myblobs.size(); cb++)
			{

				bool db_allct = false;
				ofVec2f cb_point;
				cb_point.set(contFinder[colour].myblobs[cb].centroid);

				if(((cb_point.x < 3) || (cb_point.x > width -3)) ||
				   ((cb_point.y < 3) || (cb_point.x > height -3)) )db_allct = true;

				for(int sb = 0; sb < 10; sb++){

					if(t_objs[colour][sb].assigned){
						ofxMyVec2f sb_point;
						sb_point.set(t_objs[colour][sb].bounding_cv.x +
                                     t_objs[colour][sb].bounding_cv.width/2 ,
                                     t_objs[colour][sb].bounding_cv.y +
                                     t_objs[colour][sb].bounding_cv.height/2);

						if(cb_point.distance(sb_point) < db_thresh*2)db_allct = true;

						if(sb_point.pointPolyTest(&contFinder[colour].myblobs[cb].pts) == 1)db_allct = true;
					}
				}

				//assign new blobs
				if(!db_allct){

                    vector<distListItem> distList;

                    //first assign for the nearest ghost
					for(int blob = 0; blob < 10; blob++){
						if(t_objs[colour][blob].assigned && t_objs[colour][blob].ghost > 0){
							distListItem temp;
							ofVec2f cp;
							cp.set(contFinder[colour].myblobs[cb].centroid);

							temp.dist = cp.distance(t_objs[colour][blob].centroid);
							temp.fp = blob;
							temp.np = cb;

							distList.push_back(temp);
						}
					}

					if(distList.size() > 1)sort(distList.begin(),distList.end(), comparebyDistance);

					if(distList.size() > 0){
                        t_objs[colour][distList[0].fp].bounding_cv = contFinder[colour].myblobs[cb].bounding_cv;
                        t_objs[colour][distList[0].fp].assigned = true;
                        t_objs[colour][distList[0].fp].ghostReset();

					}else{

                        //just assign to the first empty slot

                        for(int blob = 0; blob < 10; blob++){
                            if(!t_objs[colour][blob].assigned){
								t_objs[colour][blob].bounding_cv = contFinder[colour].myblobs[cb].bounding_cv;
								t_objs[colour][blob].assigned = true;
								break;
                            }
                        }

					}

				}

				//set the centroids
				for(int sb = 0; sb < 10; sb++){
					if(t_objs[colour][sb].assigned){
                        t_objs[colour][sb].centroid.set(t_objs[colour][sb].bounding_cv.x +
														t_objs[colour][sb].bounding_cv.width/2 ,
														t_objs[colour][sb].bounding_cv.y +
														t_objs[colour][sb].bounding_cv.height/2);
					}

				}

			}


		}

	}

    if(learningBg > 0)bNewFrame = false;
    return bNewFrame;

}



void capture::drawMenus(){

	glPushMatrix();

    glTranslatef(width + 25,height + 20,0);


    glTranslatef(0,15,0);
    if(selectedMenu == 0){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
    ofDrawBitmapString("Select Window:",0,0);

    int windMenuColors[4];

    for(int i = 0; i < 4; i++){

		if(selectedItem == i && selectedMenu == 0){
			windMenuColors[i] = 0xff0000;
		}else{windMenuColors[i] = 0xffffff;}

    }

    ofSetHexColor(windMenuColors[0]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("zoom: " + ofToString(zoom, 0),0,0);
    ofSetHexColor(windMenuColors[1]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("zx: " + ofToString(zx,0),0,0);
    ofSetHexColor(windMenuColors[2]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("zy: " + ofToString(zy,0),0,0);
    ofSetHexColor(windMenuColors[3]);
    glTranslatef(0,15,0);
    ofDrawBitmapString("sampleView: " + ofToString(sample_view,0),0,0);


    ofSetColor(255,255,255);

	glPopMatrix();

	glPushMatrix();

    glTranslatef(width + 25,height + 140,0);


    glTranslatef(0,15,0);
    if(selectedMenu == 1){ofSetHexColor(0x00ffff);}else{ofSetHexColor(0xffffff);}
    ofDrawBitmapString("Seg and Contour Settings:",0,0);

    int bSetMenuColors[6];

    for(int i = 0; i < 6; i++){

		if(selectedItem == i && selectedMenu == 1){
			bSetMenuColors[i] = 0xff0000;
		}else{bSetMenuColors[i] = 0xffffff;}

    }


    glTranslatef(0,15,0);
    ofSetHexColor(bSetMenuColors[0]);
    ofDrawBitmapString("bkHighThresh: " + ofToString(bgSeg.high_scale,2),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(bSetMenuColors[1]);
    ofDrawBitmapString("bkLowThresh: " + ofToString(bgSeg.low_scale,2),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(bSetMenuColors[2]);
    ofDrawBitmapString("bkErode: " + ofToString(bgSeg.erode,0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(bSetMenuColors[3]);
    ofDrawBitmapString("bkDilate: " + ofToString(bgSeg.dilate,0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(bSetMenuColors[4]);
    ofDrawBitmapString("minBlob: " + ofToString(minBlob,0),0,0);
    glTranslatef(0,15,0);
    ofSetHexColor(bSetMenuColors[5]);
    ofDrawBitmapString("maxBlob: " + ofToString(maxBlob,0),0,0);
    glTranslatef(0,15,0);


    ofSetColor(255,255,255);

	glPopMatrix();


    colChannels[selectedColour].drawMenu(width + 25, height + 290);


}

myCvmask* capture::getMasker(){

	return &masker;

};

//--------------------------------------------------------------
void capture::draw(trackingObject t_objs[][10], int size){

	ofSetHexColor(0xffffff);
	ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate(),2), 350, 720);
    drawMenus();

	ofFill();

    colChannels[selectedColour].drawHistogram(350,30);


    glPushMatrix();
    glTranslatef(20,height + 15, 0);
	zoomSelectImg.draw(0,0,320,240);


    if(zoomSel){
        ofNoFill();
        ofSetHexColor(0x00FFFF);
        ofRect(zx,zy,(float)320/zoom, (float)240/zoom);
        ofFill();
        ofSetColor(255,255,255);
	}

	ofNoFill();
	ofSetColor(255,255,255);
	ofRect(selection.x, selection.y, selection.width, selection.height);

    glTranslatef(0,height + 15,0);
    ofDrawBitmapString("selection image", 0, 0);
    glTranslatef(0,15,0);
    colChannels[selectedColour].drawSampleBp(0,0,sample_view);
    glTranslatef(0,height * 0.75 + 10,0);
    ofDrawBitmapString("selection backproject", 0, 0);
    glPopMatrix();


	glPushMatrix();
	glTranslatef(-30, 0, 0);
    vidImg.draw(width *2, 0);
    ofDrawBitmapString("live video", width *2, height+15);

    ofSetColor(100,100,100);
    ofRect(width*2, height * 2 + 18, 150, 20);
    ofSetColor(255,255,255);



    if(selectedView == 0){
		segImg.draw(width *2, height +20);
		ofDrawBitmapString("background diff", width *2, (height * 2)+ 35);}
    if(selectedView == 1){
		maskedImg.draw(width * 2, height  +20);
		ofDrawBitmapString("maskedImg", width *2, (height * 2)+ 35);}

    colChannels[selectedColour].drawVidBp(width*2,(height*2)+35);
    colChannels[selectedColour].drawVMask((width*3) + 20,(height*2)+35);

    ofDrawBitmapString("colour track:" + ofToString(selectedColour, 0), width *2, (height * 3)+ 15);

    ofSetColor(100,100,100);
    ofRect((width*3) + 20, 0, width, height);
    ofSetHexColor(0xffffff);

    ofNoFill();
    ofRect((width*3) + 20, 0, width, height);
    ofFill();

    contFinder[selectedColour].draw((width*3) + 20, 0);
    ofDrawBitmapString("blobs", (width*3) + 20, height+ 15);



    glPushMatrix();
    glTranslatef((width*3) + 20, height + 25,0);
    ofRect(0,0,width,height);

    for(int colour=0; colour < 4; colour++){
        for(int blob=0; blob < 10; blob++){

            if(t_objs[colour][blob].cont_cv.area > 0)
            {

                ofSetColor(255,255,255);

                ofSetHexColor(0xff00ff);
                ofRect(t_objs[colour][blob].bounding_cv.x, t_objs[colour][blob].bounding_cv.y,
                       t_objs[colour][blob].bounding_cv.width, t_objs[colour][blob].bounding_cv.height);

                ofSetColor(255,255,255);
				ofDrawBitmapString(ofToString( colour,0) + "," + ofToString(blob,0),
								   t_objs[colour][blob].bounding_cv.x + 15, t_objs[colour][blob].bounding_cv.y+15);
            }
        }

    }

	glPopMatrix();
    glPopMatrix();
    ofFill();


}



void capture::drawCap(int x, int y){

	ofSetColor(255,255,255);
	vidImg.draw(x,y);

}

void capture::drawCap(int x, int y, int w, int h){

	ofSetColor(255,255,255);
	vidImg.draw(x,y,w,h);

}

void capture::saveHistograms(string fileName){

	ofxXmlSettings XML;

    string filePath = "hist_xml/";
	filePath.append(fileName);

    int tagNum = XML.addTag("NBN_HISTOGRAMS");
    if( XML.pushTag("NBN_HISTOGRAMS", tagNum) ){


		for(int chan = 0; chan < 4; chan++){

		    tagNum = XML.addTag("CHAN");

			if(XML.pushTag("CHAN", tagNum)){

				XML.setValue("ERODE", colChannels[chan].erode);
				XML.setValue("DILATE", colChannels[chan].dilate);
				XML.setValue("GAUSS", colChannels[chan].gauss);
				XML.setValue("BP_THRESH", colChannels[chan].bp_thresh);
				XML.setValue("THRESH_V0", colChannels[chan].thresh_v[0]);
				XML.setValue("THRESH_V1", colChannels[chan].thresh_v[1]);
				XML.setValue("NUMHISTS", colChannels[chan].num_hists);

				for(int hist = 0; hist < colChannels[chan].num_hists; hist ++){

					string histName = "HIST";
					histName.append(ofToString(hist, 0));
					tagNum = XML.addTag(histName);

					if(XML.pushTag(histName, tagNum)){

						for(int h = 0; h < colChannels[chan].h_bins; h ++){

							for(int s = 0 ; s < colChannels[chan].s_bins; s++){

								float bin_val = cvQueryHistValue_2D(colChannels[chan].histograms[hist],h,s);

								tagNum = XML.addTag("BIN_VAL");
								if(XML.pushTag("BIN_VAL", tagNum)){
									XML.setValue("V", bin_val);
									XML.popTag();
								}

							}

						}

						XML.popTag();
					}

				}


				XML.popTag();

			}


		}
		XML.popTag();
    }

    XML.saveFile(filePath);


}

void capture::openHistograms(string fileName){

	ofxXmlSettings XML;

    string filePath = "hist_xml/";
	filePath.append(fileName);

	if(XML.loadFile(filePath)){

		if( XML.pushTag("NBN_HISTOGRAMS", 0) ){


			for(int chan = 0; chan < 4; chan++){

				if(XML.pushTag("CHAN", chan)){

					colChannels[chan].erode = XML.getValue("ERODE", 0.0f);
					colChannels[chan].dilate = XML.getValue("DILATE", 0.0f);
					colChannels[chan].gauss = XML.getValue("GAUSS",0.0f );
					colChannels[chan].bp_thresh = XML.getValue("BP_THRESH",0.0f);
					colChannels[chan].thresh_v[0] = XML.getValue("THRESH_V0", 0.0f);
					colChannels[chan].thresh_v[1] = XML.getValue("THRESH_V1", 0.0f);

					float num_hists = XML.getValue("NUMHISTS", 0.0f);


					for(int hist = 0; hist < num_hists; hist ++){

						string histName = "HIST";
						histName.append(ofToString(hist, 0));

						if(XML.pushTag(histName, 0)){

							int tagNum = 0;

							for(int h = 0; h < colChannels[chan].h_bins; h ++){

								for(int s = 0 ; s < colChannels[chan].s_bins; s++){


									if(XML.pushTag("BIN_VAL", tagNum)){
										cvSetReal2D((colChannels[chan].histograms[hist])->bins, h, s, XML.getValue("V", 0.0f));
										tagNum ++;
										XML.popTag();
									}


								}

							}

							colChannels[chan].recalcHistImg(hist);
							if(hist < num_hists - 1 )colChannels[chan].newHist();

							XML.popTag();
						}

					}


					XML.popTag();

				}


			}
			XML.popTag();
		}
	}



}

void capture::saveConfig(ofxXmlSettings XML , int tagNum){

	tagNum	= XML.addTag("CAPTURE");

    if( XML.pushTag("CAPTURE", tagNum) ){


		XML.setValue("BKTHRESH_H", bgSeg.high_scale, tagNum);
		XML.setValue("BKTHRESH_L", bgSeg.low_scale, tagNum);
		XML.setValue("BKERODE", bgSeg.erode, tagNum);
		XML.setValue("BKDILATE", bgSeg.dilate, tagNum);
		XML.setValue("MINBLOB", minBlob, tagNum);
		XML.setValue("MAXBLOB", maxBlob, tagNum);


		for(int i = 0; i < 4; i ++){
			tagNum = XML.addTag("MASKPT");

			if(XML.pushTag("MASKPT", tagNum)){
                XML.setValue("X", masker.getCoordinate(i).x, tagNum);
                XML.setValue("Y", masker.getCoordinate(i).y, tagNum);
                XML.popTag();
			}

		}


		XML.popTag();
    }

}

void capture::openConfig(ofxXmlSettings XML){

    if( XML.pushTag("CAPTURE", 0 )){

		bgSeg.high_scale = XML.getValue("BKTHRESH_H", 0);
		bgSeg.low_scale = XML.getValue("BKTHRESH_L",0);
		bgSeg.erode = XML.getValue("BKERODE", 0);
		bgSeg.dilate = XML.getValue("BKDILATE",0);
		minBlob = XML.getValue("MINBLOB",0);
		maxBlob = XML.getValue("MAXBLOB",0);

		for(int i = 0; i < 4; i ++){

			if(XML.pushTag("MASKPT", i)){

                masker.setCoordinate(i, cvPoint(XML.getValue("X",0), XML.getValue("Y",0)));

                XML.popTag();
			}

		}


		XML.popTag();
    }


}

//--------------------------------------------------------------
void capture::keyPressed  (int key){
	switch (key){

		case'S':

			vidGrabber.videoSettings();

			break;

		case 'i':
			learnImage = true;
			break;

		case ' ':
			learnBg = true;
			break;


		case OF_KEY_RETURN:
			selectedView +=1; selectedView = selectedView %4;
			break;

		case '6':
			//saveHistograms();
			break;

		case '7':
			//openHistograms();
			break;

		case ',':
			selectedMenu += 1; selectedMenu = selectedMenu%3;
			selectedItem = 0;

			if(selectedMenu == 2){

				for(int i = 0; i < 4; i++){colChannels[i].selectedItem = 0;
					colChannels[i].selected = true; }

			}else{
				for(int i = 0; i < 4; i++)colChannels[i].selected = false;
			}
			break;

		case '.':
			selectedItem += 1;
			if(selectedMenu == 0)selectedItem = selectedItem%4;
			if(selectedMenu == 1)selectedItem = selectedItem%6;
			if(selectedMenu == 2){
				for(int i = 0; i < 4; i++){
					colChannels[i].selectedItem +=1;
					colChannels[i].selectedItem = colChannels[i].selectedItem%10;
					selectedItem = selectedItem%10;
				}
			}

			break;

		case OF_KEY_UP:
			if(selectedMenu == 0){
				if(selectedItem == 0)zoom = zoom%10 + 1;
				if(selectedItem == 1)zx += 1; zx = min(zx, 320 - (320/zoom));
				if(selectedItem == 2)zy += 1; zy = min(zy, 240 - (240/zoom));
				if(selectedItem == 3)sample_view +=1; sample_view = sample_view %2;
				createZimg();
			}
			if(selectedMenu == 1){

				if(selectedItem == 0)bgSeg.setHighThreshold(bgSeg.high_scale + 0.25);
				if(selectedItem == 1)bgSeg.setLowThreshold(bgSeg.low_scale + 0.25);
				if(selectedItem == 2)bgSeg.erode +=1;
				if(selectedItem == 3)bgSeg.dilate +=1;
				if(selectedItem == 4)minBlob +=5;
				if(selectedItem == 5)maxBlob +=5;



			}
			if(selectedMenu == 2){
				if(selectedItem == 0){selectedColour +=1; selectedColour = selectedColour%4;}else if(selectedItem == 1){
					colChannels[selectedColour].recalcSampleImages(selImage);
					colChannels[selectedColour].keyPressed(key);
				}else{
					colChannels[selectedColour].keyPressed(key);
				}

			}
			break;

		case OF_KEY_DOWN:
			if(selectedMenu == 0){
				if(selectedItem == 0){zoom -= 1; zoom = max(1,zoom);zx = 0; zy = 0;}
				if(selectedItem == 1)zx -= 1; zx = max(zx, 0);
				if(selectedItem == 2)zy -= 1; zy = max(zy, 0);
				createZimg();
			}
			if(selectedMenu == 1){

				if(selectedItem == 0)bgSeg.setHighThreshold(bgSeg.high_scale - 0.25);
				if(selectedItem == 1)bgSeg.setLowThreshold(bgSeg.low_scale - 0.25);
				if(selectedItem == 2)bgSeg.erode -=1;
				if(selectedItem == 3)bgSeg.dilate -=1;
				if(selectedItem == 4)minBlob -=5;
				if(selectedItem == 5)maxBlob -=5;



			}
			if(selectedMenu == 2){
				colChannels[selectedColour].keyPressed(key);
			}
			break;

	}
}

//--------------------------------------------------------------
void capture::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void capture::mouseDragged(int x, int y, int button){

	y -= height + 15;
	x -=20;

    if(x < width && y < height){
		if(button == 0){
			if(x > selection.x && y > selection.y){
				selection.height = y - selection.y;
				selection.width = x - selection.x;
			}
		}
		if(button == 2){
			zx = x;
			zy = y;
			zx = min(zx, 320 - (320/zoom));
			zy = min(zy, 240 - (240/zoom));
			zx = max(zx, 0);
			zy = max(zy, 0);

		}
    }

}

//--------------------------------------------------------------
void capture::mousePressed(int x, int y, int button){

	y -= height + 15;
	x -=20;


    if(x < width && y < height && y > 0 && x > 0){

		if(button == 0){

			selection.x = x;
			selection.y = y;
		}
		if(button == 2){
			zoomSel = true;
			zx = 0;
			zy = 0;
			createZimg();
		}
    }

}

//--------------------------------------------------------------
void capture::mouseReleased(int x, int y, int button){

	y -= height + 15;
	x -=20;

	if(button == 0){
		if(x < width && y < height && y > 0 && x > 0){
			selectChanged = true;
		}
	}
	if(button == 2){
        zoomSel =false;
		createZimg();
		newZbp = true;
	}
}

//--------------------------------------------------------------
void capture::windowResized(int w, int h){

}




capture::~capture()
{
    //dtor
}
