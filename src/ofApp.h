#pragma once

#include "BinnedParticleSystem.h"
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxPostProcessing.h"

class ofApp : public ofBaseApp {
  public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void handleOSCMessages();
    ofxPanel gui;
    ofParameter<float> timeStep;
    ofParameter<float> particleNeighborhood, particleRepulsion;
    ofParameter<float> centerAttraction;
    ofParameter<float> dampingForce;
    ofParameter<bool> absoluteValues;

    ofParameter<bool> zoomEnabled;
    ofParameter<float> zoomCenterX;
    ofParameter<float> zoomCenterY;
    ofParameter<float> zoomExposure;
    ofParameter<float> zoomDecay;
    ofParameter<float> zoomDensity;
    ofParameter<float> zoomWeight;
    ofParameter<float> zoomClamp;

    ofParameter<bool> dofEnabled;
    ofParameter<float> dofFocus;
    ofParameter<float> dofAperture;
    ofParameter<float> dofMaxBlur;

    ofParameter<bool> grEnabled;
    ofParameter<bool> fxaaEnabled;

    ofParameter<int> red;
    ofParameter<int> green;
    ofParameter<int> blue;
    ofParameter<float> minAlpha;
    ofParameter<float> maxAlpha;

    ofParameter<float> attractorCenterX;
    ofParameter<float> attractorCenterY;
    ofParameter<float> attractorRadius;

    float padding;

    int kBinnedParticles;
    BinnedParticleSystem particleSystem;
    bool isMousePressed, slowMotion;

    bool drawBalls;
    bool drawGui;
    ofxPostProcessing post;
    shared_ptr<ZoomBlurPass> zbpass;
    shared_ptr<GodRaysPass> grpass;
    shared_ptr<DofPass> dfpass;
    shared_ptr<FxaaPass> fxpass;

    ofxOscReceiver receive;
    ofxOscSender send;

};
