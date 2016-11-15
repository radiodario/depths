#pragma once

#include "BinnedParticleSystem.h"
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxPostProcessing.h"

class ofApp : public ofBaseApp {
  public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

    ofxPanel gui;
    ofParameter<float> timeStep;
    ofParameter<float> particleNeighborhood, particleRepulsion;
    ofParameter<float> centerAttraction;
    ofParameter<float> minAlpha;
    ofParameter<float> maxAlpha;

    ofParameter<float> zoomCenterX;
    ofParameter<float> zoomCenterY;
    ofParameter<float> zoomExposure;
    ofParameter<float> zoomDecay;
    ofParameter<float> zoomDensity;
    ofParameter<float> zoomWeight;
    ofParameter<float> zoomClamp;

    ofParameter<float> dofFocus;
    ofParameter<float> dofAperture;
    ofParameter<float> dofMaxBlur;

    ofParameter<int> red;
    ofParameter<int> green;
    ofParameter<int> blue;

    float padding;

    int kBinnedParticles;
    BinnedParticleSystem particleSystem;
    bool isMousePressed, slowMotion;

    bool drawBalls;
    ofxPostProcessing post;
    shared_ptr<ZoomBlurPass> zbpass;
    shared_ptr<GodRaysPass> grpass;
    shared_ptr<DofPass> dfpass;
};