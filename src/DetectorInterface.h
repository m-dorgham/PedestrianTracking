#ifndef DETECTOR_INTERFACE_H
#define DETECTOR_INTERFACE_H

#include "opencv2/imgproc.hpp"
#include <iostream>
#include <list>

class DetectorInterface {

public:
    virtual ~DetectorInterface()=0;

    virtual std::list<cv::Rect> processFrame(cv::Mat frame, cv::Mat other)=0;

};

inline DetectorInterface::~DetectorInterface() {};

#endif  //DETECTOR_INTERFACE_H
