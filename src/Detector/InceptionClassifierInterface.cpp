#include "InceptionClassifierInterface.h"
#include "Properties.h"

InceptionClassifierInterface::InceptionClassifierInterface(std::string classifierPath, std::string modelPath,
                                               std::string labelsPath, std::string tmpImgPath)
{
    m_tmpImgPath = tmpImgPath;
    m_command = classifierPath + " --graph=" + modelPath + " --labels="
            + labelsPath + " --output_layer=final_result:0 --image="+tmpImgPath;
}

InceptionClassifierInterface::~InceptionClassifierInterface()
{
}

bool InceptionClassifierInterface::isHuman(cv::Mat roi)
{
    float scorePos = 0.0f, scoreNeg = 0.0f;
    char buffer[128];
    bool cmdExecuted=false;

    double freq = cv::getTickFrequency();
    int64 t1 = cv::getTickCount();

    //save the cv::Rect as image to be fed to the inception network
    cv::imwrite(m_tmpImgPath, roi);

    FILE* pipe = popen(m_command.c_str(), "r");

    if (!pipe)
    {  // If fpipe is NULL
      perror("Problems with pipe");
      return true;
    }

    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL){
                cmdExecuted=true;
                if(strncmp(buffer, "human", strlen("human"))==0) {
                    std::string temp(buffer);
                    size_t pos = temp.find_last_of(" ");
                    std::string scoreStr = temp.substr(pos+1, 6);
                    scorePos = stof(scoreStr);
                    std::cout << "human: " <<scorePos*100 <<"%"<< std::endl;
                } else if(strncmp(buffer, "nonhuman", strlen("nonhuman"))==0) {
                    std::string temp(buffer);
                    size_t pos = temp.find_last_of(" ");
                    std::string scoreStr = temp.substr(pos+1, 6);
                    scoreNeg = stof(scoreStr);
                    std::cout << "nonhuman: " <<scoreNeg*100 <<"%" << std::endl;
                }
            } else if(!cmdExecuted) {
                pclose(pipe);
                std::cerr << "Could not run the CNN classification!" << std::endl;
                //if couldn't run the classifier consider the rect as positive in order not to ruin the flow of the program.
                //act as it is configured without the classifier.
                return true;
            }
        }
    } catch (...) {
        pclose(pipe);
        exit(1);
    }
    pclose(pipe);

    int64 t2 = cv::getTickCount();
    std::cout << "ROI classification time: " << (t2-t1)/freq << " seconds." << std::endl;

    return (scorePos>scoreNeg) && (scorePos>Properties::minimumAcceptedConfidence);
}
