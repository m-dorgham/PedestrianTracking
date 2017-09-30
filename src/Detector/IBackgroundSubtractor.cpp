#include "IBackgroundSubtractor.h"
#include "Properties.h"


IBackgroundSubtractor::IBackgroundSubtractor(std::string bgImgPath, bool useClassifier)
    : useClassifier{useClassifier}
{
    if(useClassifier)
        m_humanClassifier = new InceptionClassifierInterface(Properties::inceptionClassifierPath,
                                                             Properties::inceptionModelPath,
                                                             Properties::inceptionLabelsPath,
                                                             Properties::tmpImgPath);
    //construct Background Subtractor MOG2
    bs_MOG2 = cv::createBackgroundSubtractorMOG2(300, 24, true);

    structuringElement = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(2,2));

    //feed in the background image
    frameRGB = cv::imread(bgImgPath);
    if(frameRGB.empty()) {
        //error in opening the image
        std::cerr << "Unable to open background image frame." << std::endl;
        exit(EXIT_FAILURE);
    }
    applyBGSubtraction();
}

IBackgroundSubtractor::~IBackgroundSubtractor()
{

}

std::list<cv::Rect> IBackgroundSubtractor::processFrame(cv::Mat frameRGB, cv::Mat frameBinary)
{
    this->frameRGB = frameRGB;

    applyBGSubtraction();

    //delete shadows
    cv::threshold(fgMask, frameBinary, 150, 255, CV_THRESH_BINARY);

    //smoothing the binary image by erusion and dilation
    cv::morphologyEx(frameBinary, frameBinary, CV_MOP_OPEN, structuringElement);

    //Find contours
    cv::Mat ContourImg = frameBinary.clone();
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(ContourImg,
                     contours, // a vector of contours
                     CV_RETR_EXTERNAL, // retrieve the external contours
                     CV_CHAIN_APPROX_NONE); // all pixels of each contours

    auto it= contours.begin();
    std::list<cv::Rect> rects;
    while (it!=contours.end()) {
        //Create bounding rect of object
        cv::Rect rect= cv::boundingRect(cv::Mat(*it));
        if(rect.area()<Properties::neglegibleRectArea){ //ignore small rectangles, area is in pixels.
            it++;
            continue;
        }

        rects.push_back(rect);
        roiTotal++;
        it++;
    }

    if(Properties::doBkgdSubtractionPostProcessing) {

        //primitive nonmaximal supression of the rectangles
        rects = removeSubRectangles(rects);

        //Background subtraction sometimes produces separate body parts, so we need to connect them
        rects = connectSeparateBodyParts(rects);

        roiPostProcessed += rects.size();
    }

    if(useClassifier) {
        auto iterator1 = rects.begin();
        while (iterator1!=rects.end()) {
            //classify the rectangle using the CNN human classifier to eleminate false positives
            cv::Mat subFrame = frameRGB(*iterator1).clone();
            if(!m_humanClassifier->isHuman(subFrame)) {
                iterator1 = rects.erase(iterator1);
                continue;
            } else {
                roiFilteredByClassifier++;
                iterator1++;
            }
        }

    }

    return rects;
}

int IBackgroundSubtractor::getTotalROIsCount()
{
    return roiTotal;
}

int IBackgroundSubtractor::getClassifiedROIsCount()
{
    return roiFilteredByClassifier;
}

int IBackgroundSubtractor::getFinalROIsCount()
{
    return roiPostProcessed;
}

void IBackgroundSubtractor::applyBGSubtraction()
{
    blur(frameRGB, frameCpy, cv::Size(4,4));
    bs_MOG2->apply(frameCpy, fgMask, 0.001);
}

bool IBackgroundSubtractor::isRect2SubsetOfRect1(cv::Rect rect1, cv::Rect rect2) {
    return (rect2.x>=rect1.x) && (rect2.br().x<=rect1.br().x)
            && (rect2.y>=rect1.y) && (rect2.br().y<=rect1.br().y);
}

bool IBackgroundSubtractor::isRect2AboveRect1(cv::Rect rect1, cv::Rect rect2) {
    return (rect2.x>=rect1.x) && (rect2.br().x<=rect1.br().x) && abs(rect2.br().y-rect1.y)<Properties::neglegibleDistance;
}

bool IBackgroundSubtractor::isRect2PartialyAboveRect1(cv::Rect rect1, cv::Rect rect2) {
    int dxMax = abs(rect2.br().x-rect1.br().x);
    int dxMin = abs(rect2.x-rect1.x);
    return (dxMax<40 || dxMin<40) && abs(rect2.br().y-rect1.y)<Properties::neglegibleDistance;
}

std::list<cv::Rect> IBackgroundSubtractor::removeSubRectangles(std::list<cv::Rect> rects)
{
    auto iterator1 = rects.begin();
    while (iterator1!=rects.end()) {
        auto iterator2 = rects.begin();
        while (iterator2!=rects.end()) {
            if(iterator1!=iterator2) {
                //get rid of sub-rectangles
                if(isRect2SubsetOfRect1(cv::Rect(*iterator1), cv::Rect(*iterator2))) {
                    iterator2 = rects.erase(iterator2);
                    continue;
                }
            }
            iterator2++;
        }
        iterator1++;
    }
    return rects;
}

std::list<cv::Rect> IBackgroundSubtractor::connectSeparateBodyParts(std::list<cv::Rect> rects)
{
    auto iterator1 = rects.begin();
    while (iterator1!=rects.end()) {
        auto iterator2 = rects.begin();
        while (iterator2!=rects.end()) {
            if(iterator1!=iterator2) {
                //connect the head with lower body
                if(isRect2AboveRect1(cv::Rect(*iterator1), cv::Rect(*iterator2))) {
                    int yMax = std::max((*iterator1).br().y, (*iterator2).br().y);
                    (*iterator1).y = std::min((*iterator1).y, (*iterator2).y);
                    (*iterator1).height = yMax - (*iterator1).y;
                    iterator2 = rects.erase(iterator2);
                    continue;
                    //connect the upper body with the lower body and the arms with the body
                } else if(isRect2PartialyAboveRect1(cv::Rect(*iterator1), cv::Rect(*iterator2))) {
                    (*iterator1).height = (*iterator1).br().y-(*iterator2).y;
                    (*iterator1).y = (*iterator2).y;
                    int xMax = std::max((*iterator1).br().x, (*iterator2).br().x);
                    (*iterator1).x = std::min((*iterator1).x, (*iterator2).x);
                    (*iterator1).width = xMax - (*iterator1).x;
                    iterator2 = rects.erase(iterator2);
                    continue;
                }
            }
            iterator2++;
        }
        iterator1++;
    }
    return rects;
}
