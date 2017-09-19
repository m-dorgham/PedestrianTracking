#include "TrackerInterface.h"
#include "Properties.h"

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

TrackerInterface::TrackerInterface(int fps, bool useLocalTracking) :
    m_fps{fps}, m_useLocalTracking{useLocalTracking}
{
    m_tracksOutputfile.open(Properties::trackingExportDir+"TracksResults.txt");

    m_colors.push_back(cv::Scalar(255, 0, 0));
    m_colors.push_back(cv::Scalar(0, 255, 0));
    m_colors.push_back(cv::Scalar(0, 0, 255));
    m_colors.push_back(cv::Scalar(255, 255, 0));
    m_colors.push_back(cv::Scalar(0, 255, 255));
    m_colors.push_back(cv::Scalar(255, 0, 255));
    m_colors.push_back(cv::Scalar(255, 127, 255));
    m_colors.push_back(cv::Scalar(127, 0, 255));
    m_colors.push_back(cv::Scalar(127, 0, 127));
    m_colors.push_back(cv::Scalar(0, 0, 127));
    m_colors.push_back(cv::Scalar(0, 127, 127));
    m_colors.push_back(cv::Scalar(127, 127, 0));
    m_colors.push_back(cv::Scalar(127, 127, 127));

    m_tracker = make_unique<CTracker>( m_useLocalTracking,
                                       tracking::DistJaccard,
                                       tracking::KalmanUnscented,
                                       tracking::FilterCenter,
                                       tracking::TrackNone,
                                       tracking::MatchHungrian,
                                       1.0/Properties::fps,                     // Delta time for Kalman filter
                                       Properties::accelNoiseMag,
                                       Properties::distanceThreshold,
                                       Properties::maxSkippedFrames * m_fps,
                                       4 * m_fps                                // Maximum trace length
                                       );
}

TrackerInterface::~TrackerInterface()
{
    m_tracksOutputfile.close();
}

void TrackerInterface::processFrame(cv::Mat frame, std::string frameName, std::list<cv::Rect> rects, bool drawBBoxes)
{
    std::vector<Point_t> centers;
    regions_t regions;

    m_tracksOutputfile << frameName + ": ";


    for (auto rect : rects)
    {
        centers.push_back(Point_t((rect.tl().x + rect.br().x) / 2, (rect.tl().y + rect.br().y) / 2));
        regions.push_back(rect);
    }

    m_tracker->Update(centers, regions, frame);


    //draw the final bounding boxes
    for (const auto& track : m_tracker->tracks)
    {
        if (track->IsRobust(m_fps / 2,                   // Minimal trajectory size
                            0.5f,                        // Minimal ratio raw_trajectory_points / trajectory_lenght
                            cv::Size2f(0.1f, 8.0f))      // Min and max ratio: width / height
                )
        {
            //log the final bounding boxes to the results file
            m_tracksOutputfile <<track->m_trackID+1<<" "<<track->GetLastRect().x<<" "<<track->GetLastRect().y<<" "
                              <<track->GetLastRect().width<<" "<<track->GetLastRect().height<<", ";
            if(drawBBoxes) {
                drawBoundingBoxes(frame, *track);
            }
        }
    }
    m_tracksOutputfile << std::endl;

}

void TrackerInterface::drawBoundingBoxes(cv::Mat frame, const CTrack &track)
{
    cv::Scalar cl = m_colors[track.m_trackID % m_colors.size()];

    cv::rectangle(frame, track.GetLastRect(), cl, 1, CV_AA);


//        for (size_t j = 0; j < track.m_trace.size() - 1; ++j)
//        {
//            const TrajectoryPoint& pt1 = track.m_trace.at(j);
//            const TrajectoryPoint& pt2 = track.m_trace.at(j + 1);

//            cv::line(frame, pt1.m_prediction, pt2.m_prediction, cl, 1, CV_AA);
////            if (!pt2.m_hasRaw)
////            {
////                cv::circle(frame, pt2.m_prediction, 4, cl, 1, CV_AA);
////            }
//        }
}
