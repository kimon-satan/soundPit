#include "interface.h"

//--------------------------------------------------------------
void interface::setup(){

	//this->escapeEnabled = false;
	thisInterp.sendInitialise();
    displayOn = false;

    for(int col = 0; col < 4; col ++){
        for(int i = 0; i < 10; i++){

            trackingObject temp;
            t_objs [col][i] = temp;
        }
    }

	for(int i = 0; i < 4; i++){
		patch temp;
		temp.count = 0;
		temp.isLive = 1;
		temp.isFilled = false;
		temp.col = 0;
		transRects.push_back(temp);

		int mode = 0;
		colMode.push_back(mode);

	}

	fileNameInput.textArea = ofRectangle(500,500,200,40);

	fileOp = -1;

	thisFP.setTrPtrs(&transRects);
	thisInterp.setTrPtrs(&transRects, &colMode);
	thisDisplay.setTrPtrs(&transRects, &colMode);




}

//--------------------------------------------------------------
void interface::update(){

    thisInterp.checkForInit();

	bool newFrame = thisCapture.update(t_objs, 4);

	if(newFrame)thisFP.feedRawBlobs(t_objs, 4);
	thisInterp.feedObjects(t_objs, 4, newFrame);

	if(ofGetFrameNum() == 60){

        openConfig("default.xml");
        thisCapture.openHistograms("default.xml");
	    thisCapture.startLearnBg();

    } //for auto initialisation

}

//--------------------------------------------------------------
void interface::draw(){

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(displayOn){
        thisCapture.drawCap(thisDisplay.displayRect.x,thisDisplay.displayRect.y,
                            320 * thisDisplay.displayRect.width,
                            240 * thisDisplay.displayRect.height);
    }


	switch (viewMode){

        case 0:
			thisCapture.draw(t_objs, 4);
			break;

        case 1:
			thisCapture.drawCap(320,0);
			thisFP.drawMenus(thisCapture.getMasker());
			break;

        case 2:
			thisInterp.drawMenus();
			thisInterp.draw(t_objs, 4);
			break;

        case 3:
			thisCapture.drawCap(320,0,640,480);
			thisDisplay.drawMenus();
			break;

	}


	if(fileNameInput.isActive){

		ofFill();
		ofSetColor(200, 200,200);
		ofRect(450, 450, 300, 100);
		ofSetColor(50, 50, 50);
		switch(fileOp){
			case 0:
				ofDrawBitmapString("saveConfig", 490, 490); break;
			case 1:
				ofDrawBitmapString("saveHistogram", 490, 490); break;
			case 2:
				ofDrawBitmapString("openConfig", 490, 490); break;
			case 3:
				ofDrawBitmapString("openHistogram", 490, 490); break;
		}

		fileNameInput.draw();
	}

	switch (viewMode){

        case 0:
			thisFP.draw3DMask();
			break;

        case 1:
			thisFP.draw(t_objs, 4);
			break;

        case 2:
			thisFP.draw3DMask();
			break;

        case 3:
			thisFP.draw3DMask();
			thisDisplay.draw(t_objs, thisInterp.getCollisions(), thisFP.getMaskArray(), thisInterp.getColModes(), false);
			break;

	}

	if(displayOn){

        thisDisplay.draw(t_objs,thisInterp.getCollisions(), thisFP.getMaskArray(),thisInterp.getColModes(), true);

	}


}


void interface::openConfig(string fileName){

	string filePath = "config_xml/";
	filePath.append(fileName);

	ofxXmlSettings XML;

	if(XML.loadFile(filePath)){

		if( XML.pushTag("BMSETTINGS", 0) ){

			thisCapture.openConfig(XML);
			thisFP.openConfig(XML);
			thisInterp.openConfig(XML);
			thisDisplay.openConfig(XML);

			XML.popTag();
		}
	}

}
void interface::saveConfig(string fileName){

	string filePath = "config_xml/";
	filePath.append(fileName);

	ofxXmlSettings XML;

	int tagNum = XML.addTag("BMSETTINGS");
	if( XML.pushTag("BMSETTINGS", tagNum) ){

		thisCapture.saveConfig(XML, tagNum);
		thisFP.saveConfig(XML, tagNum);
		thisInterp.saveConfig(XML, tagNum);
		thisDisplay.saveConfig(XML, tagNum);

		XML.popTag();
	}
	XML.saveFile(filePath);

}

//--------------------------------------------------------------
void interface::keyPressed  (int key){

	if(!fileNameInput.isActive){

		if(key == '1'){viewMode +=1; viewMode = viewMode%4;

		}else if(key == '2'){

			if(displayOn){
				displayOn =false;
				ofToggleFullscreen();
			}else{
				displayOn = true;
				ofToggleFullscreen();
			}

		}else if(key == '6'){fileOp = 0; fileNameInput.isActive = true;
		}else if(key == '^'){fileOp = 1; fileNameInput.isActive = true;

		}else if(key == '7'){fileOp = 2; fileNameInput.isActive = true;
		}else if(key == '&'){fileOp = 3; fileNameInput.isActive = true;

		}else{

			switch (viewMode){

				case 0:
					thisCapture.keyPressed(key);
					break;

				case 1:
					thisFP.keyPressed(key,thisCapture.getMasker());
					break;

				case 2:
					thisInterp.keyPressed(key);
					break;

				case 3:
					thisDisplay.keyPressed(key);
					break;


			}
		}


	}else{

		if(key == OF_KEY_RETURN){

			fileNameInput.isActive = false;

			switch (fileOp) {
				case 0:
					saveConfig(fileNameInput.workingText);break;
				case 1:
					thisCapture.saveHistograms(fileNameInput.workingText);break;
				case 2:
					openConfig(fileNameInput.workingText);break;
				case 3:
					thisCapture.openHistograms(fileNameInput.workingText);break;
			}

			fileNameInput.workingText = "";
			fileOp = -1;

		}else{

			fileNameInput.keyPressed(key);

		}


	}



}




//--------------------------------------------------------------
void interface::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void interface::mouseDragged(int x, int y, int button){

    switch (viewMode){

        case 0:
			thisCapture.mouseDragged(x,y,button);
			break;

	}


}

//--------------------------------------------------------------
void interface::mousePressed(int x, int y, int button){

    switch (viewMode){

        case 0:
			thisCapture.mousePressed(x,y,button);
			break;

	}


}

//--------------------------------------------------------------
void interface::mouseReleased(int x, int y, int button){

    switch (viewMode){

        case 0:
			thisCapture.mouseReleased(x,y,button);
			break;

	}

}

//--------------------------------------------------------------
void interface::windowResized(int w, int h){

    thisFP.setProjChanged(true);

}

