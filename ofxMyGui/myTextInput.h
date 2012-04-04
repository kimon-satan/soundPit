#ifndef MYTEXTINPUT_H
#define MYTEXTINPUT_H

#include "ofMain.h"

class myTextInput
{
    public:
        myTextInput() {

        isActive = false;
        workingText = "";
        outputTo = "";
        label = "";
        textArea.set(20,20,200,20);

        }

        virtual ~myTextInput() {}

        void draw(){

            ofDrawBitmapString(label,textArea.x,textArea.y - 5);

            ofFill();
            if(isActive){ofSetColor(255,255,255);}else{
            ofSetColor(50,50,50);
            }

            ofRect(textArea.x,textArea.y,textArea.width, textArea.height);
            ofPushStyle();
            if(isActive){ofSetColor(255,0,0);}else{ofSetColor(0,0,0);}
            ofNoFill();
            ofSetLineWidth(2);
            ofRect(textArea.x,textArea.y,textArea.width, textArea.height);
            ofDrawBitmapString(workingText,textArea.x,textArea.y + 17);
            ofPopStyle();

        }

        int keyPressed(int key){

            if(isActive){
                if(key == OF_KEY_RETURN){
                    isActive = false;
                    return 1;

                }else if(key == OF_KEY_BACKSPACE && workingText.size() > 0){

                    workingText = workingText.substr(0,workingText.size()-1);

                }else{

                    char letter =  static_cast<char>(key);
                    workingText.push_back(letter);
                }

            }

            return 0;
        }

        bool isActive;
        string workingText;
        string label;
        string outputTo;
        float  *outputFloat;
        string *outputString;
        int    *outputInt;
        ofRectangle textArea;


    protected:
    private:


};

#endif // MYTEXTINPUT_H
