#include "ofMain.h"
#include "interface.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main(int argc, char* argv[] ){

    string t_filename  = "default";
    stringstream ss;

    if(argc == 2){

        ss << argv[1];
        ss >> t_filename;
        cout << endl;
        cout << t_filename << endl;
    }

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1240,800, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new interface(t_filename));

}
