#include "Properties.h"


std::string Properties::inceptionClassifierPath="";
std::string Properties::inceptionModelPath="";
std::string Properties::inceptionLabelsPath="";
std::string Properties::tmpImgPath="";

std::string Properties::backgroundImg="";
std::string Properties::datasetDir="";
std::string Properties::trackingExportDir="";
std::string Properties::detectionsCoordsImportDir="";

DetectionMethod Properties::detectionMethod;

int Properties::frameWidth;
int Properties::frameHeight;

int Properties::fps;
bool Properties::exportAsVideo=true;
bool Properties::importDetectionsFromFiles=false;
int Properties::neglegibleRectArea;
int Properties::neglegibleDistance=30;
bool Properties::useClassifier=true;
bool Properties::exportBinaryImgs=false;
bool Properties::exportDetectionsOnly=false;
bool Properties::doBkgdSubtractionPostProcessing=true;

float Properties::accelNoiseMag= 0.1f;
float Properties::distanceThreshold = 0.8f;
float Properties::minimumAcceptedConfidence = 0.9f;
float Properties::maxSkippedFrames = 1.0f;

float Properties::yoloThreshold=0.5f;
float Properties::hierThreshold=0.5f;
