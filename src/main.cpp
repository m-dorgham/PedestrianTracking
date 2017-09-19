#include "ConfigLoader/ConfigFile.h"
#include "TrackerInterface.h"
#include "IBackgroundSubtractor.h"
#include "YoloInterface.h"
#include "Properties.h"
#include "Utils.h"


static cv::Ptr<DetectorInterface> detector;
static cv::Ptr<TrackerInterface> tracker;
static cv::VideoWriter outputVideo;
static std::ofstream detectionsWriter;
static std::ifstream detectionsReader;
static bool exportDetectionsCoords;

void init();
void runDetectionAndTracking();
void loadConfigurations(std::string configFilePath);
std::list<cv::Rect> parseDetectionsCoordinates(std::ifstream* inputStream);


int main(int argc, char* argv[])
{
    double freq = cv::getTickFrequency();
    int64 t1 = cv::getTickCount();

    try {
        //initialize the application
        init();
        //run the main algorithm
        runDetectionAndTracking();
    } catch(std::exception &ex) {
        std::cerr << "Exception occurred while processing!" << std::endl;
        std::cerr << ex.what() << std::endl;
    }

    int64 t2 = cv::getTickCount();
    std::cout << "Total execution time = " << (t2-t1)/freq << " seconds." << std::endl;
    return EXIT_SUCCESS;
}

void init()
{
    //load the configuration file first
    loadConfigurations("config.cfg"); //CMakeLists file copies the config.cfg file from src directory to the build directory


    if(Properties::importDetectionsFromFiles)
    {
        std::cout << "Detections will be loaded from files." << std::endl;
    }
    else
    {   //construct the detector object
        if (Properties::detectionMethod==DetectionMethod::Yolo)
        {
            detector = new YoloInterface(Properties::yoloThreshold, Properties::hierThreshold);
            std::cout << "Detection method Yolo was selected." << std::endl;
        }
        else if(Properties::detectionMethod==DetectionMethod::BSI)
        {
            detector = new IBackgroundSubtractor(Properties::backgroundImg, Properties::useClassifier);
            std::cout << "Detection method BSI was selected." << std::endl;
        }

        //create subdirectory to export detections coordinates
        int nError = createDir((Properties::trackingExportDir+"detections").c_str());
        if (nError != 0) {
            std::cerr << "Could not create a subdirectory for detections coordinates. They will not be saved!"<<std::endl;
            exportDetectionsCoords = false;
        } else {
            std::cout << "Detections coordinates will be exported to the directory "
                      <<Properties::trackingExportDir+"detections"<<std::endl;
            exportDetectionsCoords = true;
        }
        if(Properties::exportBinaryImgs)
        {
            nError = createDir((Properties::trackingExportDir+"binary").c_str());
            if (nError != 0) {
                std::cerr << "Could not create a subdirectory for binary images. They will not be saved!"<<std::endl;
                Properties::exportBinaryImgs = false;
            } else {
                std::cout << "Binary images will be exported to the directory "
                          <<Properties::trackingExportDir+"binary"<<std::endl;
            }
        }
    }

    //construct the tracker object
    tracker = new TrackerInterface(Properties::fps, false);

    if(Properties::exportAsVideo) {
        outputVideo.open(Properties::trackingExportDir+"output.avi", CV_FOURCC('X','V','I','D'),
                         Properties::fps, cv::Size(Properties::frameWidth, Properties::frameHeight), true);
        if (!outputVideo.isOpened())
        {
            std::cerr  << "Could not open the output video for saving the result!" << std::endl;
            Properties::exportAsVideo=false;
        }
    }
}

void runDetectionAndTracking()
{
    struct dirent **namelist;
    int i,n;

    //list all files in the directory and sort them alphabetically
    n = scandir(Properties::datasetDir.c_str(), &namelist, 0, alphasort);
    if (n < 0)
        perror("No images found in the directory.");
    else {
        for (i = 0; i < n; i++) {
            if(strcmp(namelist[i]->d_name, ".")==0 || strcmp(namelist[i]->d_name, "..")==0)
                continue;

            ///////////////////////////////////////////////////////////////////////////////////
            /// Reading the input images
            ///
            ///////////////////////////////////////////////////////////////////////////////////

            std::cout << "Processing image frame: " << namelist[i]->d_name << std::endl;

            cv::Mat frameRGB = cv::imread(Properties::datasetDir+namelist[i]->d_name);
            cv::Mat frameBinary(frameRGB.rows, frameRGB.cols, CV_8U);
            if(frameRGB.empty()) {
                //error in opening the image
                std::cerr << "Unable to open image frame: " << namelist[i]->d_name << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string imgName(namelist[i]->d_name);
            imgName = imgName.substr(0, imgName.find_last_of("."));

            ///////////////////////////////////////////////////////////////////////////////////
            /// Detection part
            ///
            ///////////////////////////////////////////////////////////////////////////////////

            std::list<cv::Rect> rects;

            if(Properties::importDetectionsFromFiles) {
                detectionsReader.open(Properties::detectionsCoordsImportDir+imgName+".det");
                if(!detectionsReader)
                    throw std::runtime_error("Could not open file " + Properties::detectionsCoordsImportDir+imgName+".det" );

                rects = parseDetectionsCoordinates(&detectionsReader);
                detectionsReader.close();
            } else {
                rects = detector->processFrame(frameRGB, frameBinary);

                //export detected regions coordinates
                if(exportDetectionsCoords) {
                    detectionsWriter.open(Properties::trackingExportDir+"detections/"+imgName+".det");
                    if(detectionsWriter.is_open() && rects.size()>0) {
                        for (auto rect : rects)
                            detectionsWriter <<rect.x<<", "<<rect.y<<", "<<rect.width<<", "<<rect.height<<";";
                    }
                    detectionsWriter.close();
                }
            }

            ///////////////////////////////////////////////////////////////////////////////////
            /// Tracking part
            ///
            ///////////////////////////////////////////////////////////////////////////////////

            tracker->processFrame(frameRGB, imgName, rects, !Properties::exportDetectionsOnly);


            ///////////////////////////////////////////////////////////////////////////////////
            /// Exporting the result
            ///
            ///////////////////////////////////////////////////////////////////////////////////

            if(Properties::exportDetectionsOnly) { //draw the detected bounding boxes on the image
                for (auto rect : rects)
                {
                    cv::rectangle(frameRGB, rect, cv::Scalar(255, 0, 0));
                }
            }

            if(Properties::exportAsVideo)
                outputVideo << frameRGB;
            else
                cv::imwrite(Properties::trackingExportDir+namelist[i]->d_name, frameRGB);

            if(Properties::exportBinaryImgs &&
                    Properties::detectionMethod==DetectionMethod::BSI &&
                    !Properties::importDetectionsFromFiles)
                cv::imwrite(Properties::trackingExportDir+"binary/"+namelist[i]->d_name, frameBinary);

            free(namelist[i]);
        }
    }
    free(namelist);

}

void loadConfigurations(std::string configFilePath)
{
    bool allPropertiesExist = true;
    try {
        ConfigFile config( configFilePath );


        if(!config.readInto( Properties::datasetDir, "datasetDir" ))
            allPropertiesExist = false;
        if(!config.readInto( Properties::backgroundImg, "backgroundImg" ))
            allPropertiesExist = false;
        if(!config.readInto( Properties::trackingExportDir, "trackingExportDir" ))
            allPropertiesExist = false;
        if(!config.readInto( Properties::detectionsCoordsImportDir, "detectionsCoordsImportDir" ))
            allPropertiesExist = false;

        if(!config.readInto( Properties::inceptionClassifierPath, "inceptionClassifierPath" ))
            allPropertiesExist = false;
        if(!config.readInto( Properties::inceptionModelPath, "inceptionModelPath" ))
            allPropertiesExist = false;
        if(!config.readInto( Properties::inceptionLabelsPath, "inceptionLabelsPath" ))
            allPropertiesExist = false;
        if(!config.readInto( Properties::tmpImgPath, "tmpImgPath" ))
            allPropertiesExist = false;

        Properties::frameWidth = config.read<int>("frameWidth");
        Properties::frameHeight = config.read<int>("frameHeight");

        if(Properties::frameWidth==0 || Properties::frameHeight==0)
            allPropertiesExist = false;

        std::string temp;
        config.readInto(temp, "DetectionMethod");
        if(strcasecmp(temp.c_str(), "yolo")==0)
            Properties::detectionMethod = DetectionMethod::Yolo;
        else
            Properties::detectionMethod = DetectionMethod::BSI;

        Properties::fps = config.read<int>("fps");
        Properties::exportAsVideo = config.read("exportAsVideo", true);
        Properties::neglegibleRectArea = config.read<int>("neglegibleRectArea", 100);
        Properties::useClassifier = config.read( "useClassifier", true );
        Properties::exportBinaryImgs = config.read( "exportBinaryImgs", false );
        Properties::doBkgdSubtractionPostProcessing = config.read( "doBkgdSubtractionPostProcessing", true );
        Properties::exportDetectionsOnly = config.read( "exportDetectionsOnly", false );
        Properties::importDetectionsFromFiles = config.read( "importDetectionsFromFiles", false );

        Properties::accelNoiseMag = config.read<float>("accelNoiseMag", 0.1f);
        Properties::distanceThreshold = config.read<float>("distanceThreshold", 0.8f);
        Properties::minimumAcceptedConfidence = config.read<float>("minimumAcceptedConfidence", 0.9f);
        Properties::neglegibleDistance = config.read<int>("neglegibleDistance", 30);
        Properties::maxSkippedFrames = config.read<float>("maxSkippedFrames", 1.0f);

        Properties::yoloThreshold = config.read<float>("yoloThreshold", 0.5);
        Properties::hierThreshold = config.read<float>("hierThreshold", 0.5);


        //fix directories paths
        if(allPropertiesExist != false) {
            if(!endsWith(Properties::datasetDir, "/"))
                Properties::datasetDir += "/";
            if(!endsWith(Properties::detectionsCoordsImportDir, "/"))
                Properties::detectionsCoordsImportDir += "/";
            if(!endsWith(Properties::trackingExportDir, "/"))
                Properties::trackingExportDir += "/";

            if(!dirExists(Properties::datasetDir.c_str())){
                std::cerr << "dataset directory does not exist! Program will exit now."<<std::endl;
                exit(EXIT_FAILURE);
            }
            if(!dirExists(Properties::trackingExportDir.c_str())) {
                std::cerr << "tracking export directory does not exist! Program will exit now."<<std::endl;
                exit(EXIT_FAILURE);
            }
            if(!dirExists(Properties::detectionsCoordsImportDir.c_str())) {
                std::cerr << "detections coordinates import directory does not exist! Program will exit now."<<std::endl;
                exit(EXIT_FAILURE);
            }
        }

    } catch(ConfigFile::file_not_found& ) {
        std::cerr << "Configuration file not found! Program will exit now."<<std::endl;
        exit(EXIT_FAILURE);
    }
    if(!allPropertiesExist) {
        std::cerr << "Some properties were not written in Configuration file! Program will exit now."<<std::endl;
        exit(EXIT_FAILURE);
    }
}

std::list<cv::Rect> parseDetectionsCoordinates(std::ifstream* inputStream)
{
    std::list<cv::Rect> rects;
    if(inputStream==nullptr || !(*inputStream) || !(*inputStream).is_open())
        return rects;
    std::string bbox;
    while (std::getline(*inputStream, bbox, ';')) {
        if(!bbox.empty()) {
            std::vector<std::string> coords = split(bbox, ',');
            if(coords.size()!=4)
                throw std::runtime_error("Wrong coordinates of detections. There must be exactly 4 coords!");
            rects.push_back(cv::Rect(std::stoi(coords[0]), std::stoi(coords[1]),
                    std::stoi(coords[2]), std::stoi(coords[3])));
        }
    }
    return rects;
}

