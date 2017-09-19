#ifndef CNN_CLASSIFIER_INTERFACE_H
#define CNN_CLASSIFIER_INTERFACE_H

#include <stdio.h>
#include <cstddef>

#include <iostream>
#include <string>

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class InceptionClassifierInterface {

public:
    InceptionClassifierInterface(std::string classifierPath, std::string modelPath,
                           std::string labelsPath, std::string tmpImgPath);
    ~InceptionClassifierInterface();

    bool isHuman(cv::Mat roi);

private:
    std::string m_command;
    std::string m_tmpImgPath;
};


#endif //CNN_CLASSIFIER_INTERFACE_H
