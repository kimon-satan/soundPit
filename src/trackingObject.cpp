#include "trackingObject.h"



trackingObject::trackingObject()
{
    ofxCvMyBlob();
    reset();
    hist_size = 5;
    lThresh = 1.0;
    uThresh = 0.2;

}

trackingObject::~trackingObject()
{
    //dtor
}

void trackingObject::reset(){


present = false;
assigned = false;

projPos.set(0,0);
avPos.set(0,0);
direction = 0;
true_direction = 0;
speed = 0;
collision = false;
history.clear();
drawhistory.clear();
ghostReset();

}


void trackingObject::updateHistory(){

history.push_back(projPos);

    if (history.size() > hist_size){

    history.erase(history.begin());

    }

    updateSpeed();
    updateDirection();

}

void trackingObject::updateDrawHistory(){

drawhistory.push_back(projPos);

    if (drawhistory.size() > 10){

    drawhistory.erase(drawhistory.begin());

    }

    updateAvPos();

}


void trackingObject::updateSpeed(){

    float tempTotal = 0;
    float tempDiff;
    int counter = 1;

    if(history.size()> 1){
        for(int i = 0; i < history.size()-1; i++){

            for(int j = i+1; j < history.size()-1; j++){

            tempDiff = history.at(i).distance(history.at(j));
            tempTotal += tempDiff;

            }

    counter++;

        }

    speed = tempTotal/counter;

    }

    if(speed >= lThresh){
        moving = true;
    }else if(speed <= uThresh){
        moving = false;
	}

}

void trackingObject::updateAvPos(){

    float x = 0;
    float y = 0;
    ofxVec2f pos;


    for(int i =0; i < drawhistory.size(); i++){

            pos = drawhistory[i];

            x += pos.x;
            y += pos.y;

    }
    //printf("%f,%f \n", x,y);
    x = x/drawhistory.size();
    y = y/drawhistory.size();

    avPos.x = x;
    avPos.y = y;


}

void trackingObject::updateDirection(){

ofxVec2f axisY;
ofxVec2f trip;

axisY.set(50,0);

//find a way to work out angle from y axis

    if(history.size()>1){

                trip = history[0] - history[history.size() -1];
                true_direction = trip.angle(axisY);
                direction = abs(trip.angle(axisY));

        }


}



void trackingObject::ghostIncrement(){

ghost += 1;

}

void trackingObject::ghostReset(){

ghost = 0;

}


