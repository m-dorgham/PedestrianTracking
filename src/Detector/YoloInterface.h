#ifndef YOLO_INTERFACE_H
#define YOLO_INTERFACE_H

extern "C" {
#include "darknet.h"
}
#include "DetectorInterface.h"
#include "opencv2/imgproc.hpp"

class YoloInterface : public DetectorInterface {
public:
    YoloInterface(float thresh, float hier_thresh);
    ~YoloInterface();

    std::list<cv::Rect> processFrame(cv::Mat frameRGB, cv::Mat other);

private:

    std::list<cv::Rect> getHumansBoundingBoxes(box *boxes, float **probs, int total, int classes);
    image ipl_to_image(IplImage* src);
    void ipl_into_image(IplImage* src, image im);

    network net;
    char **labels;
    float thresh, hier_thresh;
};

#endif  //YOLO_INTERFACE_H
