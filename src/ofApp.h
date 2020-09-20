#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "BirdBox.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
 
        void drawGrid();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void changeColor();
        void setupGrid();
        void savePlayerImages();
        void loadBirdText();
        void displayDebugText();
        void displayFrozenImage();
    
        Boolean colorComparison(ofImage orgImage, ofImage silImage, int grabX, int grabY, int grabWidth, int grabHeight);
    
        bool saveImage = false;

    // Bird content
        void loadImages();
        void loadSounds();
        void loadNewBirdImage();
    
        void displayTimeIndicator();
        void setupColorPalette();
    
    enum CurrentState{
        Passive,
        Playing
    };
    CurrentState state;
    
    // Kinect detection
    ofxOpenNI openNIDevice;
    
    //Aotea Roof hax fix to cut out the top third of the kinect image
    Boolean roofHax=true;
    
    //Turn on Debug mode
    Boolean debugMode = false;
    
    // Grid stuff
    int numRows=3;
    int numCols=4;
    int boxW;
    int boxH;
    
    ofColor colourBoxArray[12];
    ofImage birdImage[12];
    vector<BirdBox*> birdBoxes;
    
    int numImagesToKeep = 12;
    
    int totalPlayerImages = 0;
    int currentlyShowing  = 0;
    int roofRectHeight = 190;
    
    // Bird stuff
    ofImage birdCatalog[12];
    ofImage currentBirdImage;

    int birdCatalogSize = 12;
    int numOfThisBird[12];
    int currentImageNumber=0;
    
    ofSoundPlayer birdSoundCatalog[12];
    ofSoundPlayer currentBirdSound;

    string birdName[12];
   
    ofImage savedShape; // The pixels used
 //   ofImage depthImg; //depth hopefully
    vector <ofImage> previousPlayImages;
    
 //   vector<vector <ofImage> > previousPlayGrab; //2D array - Each type of bird has a storage for the previous plays of it.
    vector<vector <ofImage> > previousPlayBird;
    
 //   vector<vector <ofColor> > previousColBG;
 //   vector<vector <ofColor> > previousColDepth;

    
    Boolean lastFrameReady=false;
    //ready timer
    int readyTimer = 1000;
    int readyTimerElapsed = 0;
    Boolean readyToPlay=false;
        
    // Color detection
    ofImage beforeDepthTrigger; // Screengrab before the trigger capture
    ofImage grabImg; // Screengrab of both depth and silhouette
    
    ofImage actualOnScreenGrab;
    ofImage greyOnScreenGrab;
    ofColor randomCol;
    ofColor randomColDepth;
    
    float screenAdjust = 0.75;
    int dimWidth = 1366;
    int dimHeight = 768 * screenAdjust;
    
    int gameBirdW, gameBirdH;
    int perCount; //gets percent amount for image
    int totCount; //gets total percent amount for image
    int percentage;
  
    
    //timer
    int absolutePlayTime = 12000;
    float triggerTimer = 0;
    float triggerStartTime; // store when we start time timer
    Boolean playTrigger = false;
    
    ofRectangle birdRect;
    float imgScale;
    
    //kinect depth
    int minDepthSetter = 3500;
    int maxDepthSetter = 6000;
    
    int imgCounter=0;
    
    //colour palette - safest way to generate background colour
    int colorPaletteSize = 10;
    ofColor colorPalette[10];
    ofColor colorPaletteDepth[10];
    
    //text
    ofTrueTypeFont descriptionFont;
    int scrollingTextX = 0;
    
    Boolean flipStatus=false;

    bool gridOut = true;

};
