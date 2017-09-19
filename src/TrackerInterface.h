#ifndef TRACKER_INTERFACE_H
#define TRACKER_INTERFACE_H

#include <iostream>
#include <memory>
#include <fstream>

#include <opencv2/imgproc.hpp>
#include "Tracker/Ctracker.h"

class TrackerInterface {

public:
    TrackerInterface(int fps, bool useLocalTracking);
    ~TrackerInterface();

    void processFrame(cv::Mat frame, std::string frameName, std::list<cv::Rect> rects, bool drawBBoxes=true);

private:
    void drawBoundingBoxes(cv::Mat frame, const CTrack& track);


    std::unique_ptr<CTracker> m_tracker;
    std::vector<cv::Scalar> m_colors;
    int m_fps;
    bool m_useLocalTracking;
    std::ofstream m_tracksOutputfile;

};


#endif //TRACKER_INTERFACE_H
