//
// Created by jieming on 26.01.20.
//

#ifndef RGBD_DETECT_YOLORECOGNIZER_H
#define RGBD_DETECT_YOLORECOGNIZER_H
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include <vector>
#include <getopt.h>
#include <map>
#include <string>
#include <thread>
#include <stdlib.h>
#include <string>
#include <pthread.h>

#include <message_filters/subscriber.h>
#include <ros/publisher.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/time_synchronizer.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/LaserScan.h>

//#include <tf/transform_listener.h>
#include <nav_msgs/Odometry.h>
#include "ros/ros.h"

#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>

#include <Eigen/Dense>

#include "darknet.h"
#include "LaserRecognizer.h"

using namespace sensor_msgs;
using namespace nav_msgs;
using namespace message_filters;
using namespace std;



class YoloRecognizer {
private:
    std::ofstream outFile;

    ros::NodeHandle n_;
    ros::Rate rate;

    message_filters::Subscriber<sensor_msgs::Image> *rgbSub_;
    message_filters::Subscriber<sensor_msgs::Image> *depthSub_;
    message_filters::Subscriber<sensor_msgs::LaserScan> *scanSub_;
    message_filters::Subscriber<nav_msgs::Odometry> *odomSub_;
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image, sensor_msgs::LaserScan, nav_msgs::Odometry> SyncPolicy;
    message_filters::Synchronizer<SyncPolicy> *sync_;

//    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image> SyncPolicy;
//    message_filters::Synchronizer<SyncPolicy>* sync_;

//    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::LaserScan, sensor_msgs::LaserScan> SyncPolicy;
//    message_filters::Synchronizer<SyncPolicy>* sync_;

    cv_bridge::CvImagePtr cv_ptr;
    cv_bridge::CvImagePtr cv_ptr_depth;

    ros::Publisher pub_;
    cv::Mat detectionDepthImage;
    ros::Publisher lidarPub;


    std::vector<string> labelNames;
    std::string labelfile;
    char *cfgfile;
    char *weightfile;
    network *net;
    std::vector<std::vector<float>> boxes;
    std::vector<std::pair<std::string, float>> yoloRst;
    std::map<std::string, double> options;

    sensor_msgs::LaserScanConstPtr scan_ptr;
    LaserRecognizer laserRecognizer;

    nav_msgs::OdometryConstPtr odomPtr;
    thread *th1;
    thread *scanth2;
    thread *mainth0;

    cv::Mat showImg;
    bool sim_ = true;
public:
    cv::Mat detectionImage, depthImage, depthImage2;

    YoloRecognizer(ros::NodeHandle &, ros::Rate &);

    ~YoloRecognizer();

    friend class SORT;

    friend void mainThread(YoloRecognizer &);

    friend void recognizationThread(YoloRecognizer &);

    friend void laserRecogThread(YoloRecognizer &);

    void laserRecog();

    void test_yolo(char *cfgfile, char *weightfile, std::string labelfile, char *filename, float thresh);

    Eigen::Vector3d deProjection(const float &depth, const cv::Rect roi);

    std::vector<double> depthCalc(const Eigen::MatrixXd range);

    void combineCallback(const ImageConstPtr &rgb, const ImageConstPtr &depth, const LaserScanConstPtr &scan,
                         const OdometryConstPtr &odometry);

    void camCallback(const ImageConstPtr &rgb, const ImageConstPtr &rgb2);

    void laserCallback(const LaserScanConstPtr &rgb, const LaserScanConstPtr &rgb2);

    void pub(std::vector<std::vector<SamplePoint>> samples);

    Eigen::Vector3d transformGlobalCoord(float x, float y);
};

#endif //RGBD_DETECT_YOLORECOGNIZER_H
