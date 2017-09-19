#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <string>

enum class DetectionMethod {
    BSI, Yolo
};

class Properties {

public:

static std::string inceptionClassifierPath;
static std::string inceptionModelPath;
static std::string inceptionLabelsPath;
static std::string tmpImgPath;

static std::string backgroundImg;
static std::string datasetDir;
static std::string trackingExportDir;
static std::string detectionsCoordsImportDir;

static DetectionMethod detectionMethod;

static int fps;
static bool exportAsVideo;
static bool importDetectionsFromFiles;
static int frameWidth;
static int frameHeight;

static int neglegibleRectArea;
static int neglegibleDistance;
static bool useClassifier;
static bool exportBinaryImgs;
static bool exportDetectionsOnly;
static bool doBkgdSubtractionPostProcessing;

static float accelNoiseMag;
static float distanceThreshold;
static float minimumAcceptedConfidence;
static float maxSkippedFrames;

static float yoloThreshold;
static float hierThreshold;
};


#endif //PROPERTIES_H
