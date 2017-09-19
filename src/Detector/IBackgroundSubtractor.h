#ifndef BACKGROUND_SUBTRACTOR_H
#define BACKGROUND_SUBTRACTOR_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "opencv2/bgsegm.hpp"
#include "opencv2/video/background_segm.hpp"

#include <string>

#include "InceptionClassifierInterface.h"
#include "DetectorInterface.h"

class IBackgroundSubtractor : public DetectorInterface {
public:
    IBackgroundSubtractor(std::string bgImgPath, bool useClassifier=true);
    ~IBackgroundSubtractor();

    std::list<cv::Rect> processFrame(cv::Mat frameRGB, cv::Mat frameBinary);

    int getTotalROIsCount();
    int getClassifiedROIsCount();
    int getFinalROIsCount();


private:
    void applyBGSubtraction();

    bool isRect2SubsetOfRect1(cv::Rect rect1, cv::Rect rect2);
    bool isRect2AboveRect1(cv::Rect rect1, cv::Rect rect2);
    bool isRect2PartialyAboveRect1(cv::Rect rect1, cv::Rect rect2);
    std::list<cv::Rect> removeSubRectangles(std::list<cv::Rect> rects);
    std::list<cv::Rect> connectSeparateBodyParts(std::list<cv::Rect> rects);


    cv::Mat frameRGB, frameCpy; //holding the current frame
    cv::Mat fgMask;
    cv::Mat structuringElement;  //morphology element
    cv::Ptr<cv::BackgroundSubtractor> bs_MOG2;
    bool useClassifier;
    cv::Ptr<InceptionClassifierInterface> m_humanClassifier;
    int roiTotal = 0;
    int roiFilteredByClassifier = 0;
    int roiPostProcessed = 0;

};

#endif //BACKGROUND_SUBTRACTOR_H
