#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <caffe/net.hpp>
#include <caffe/util/io.hpp>
#include <caffe/layers/memory_data_layer.hpp>
#include <boost/filesystem.hpp>
#include <boost/chrono.hpp>
#include <ORUtils/gSLICr_defines.h>
#include <objectseg.h>
#include "fillgapcalc.h"

namespace fs = boost::filesystem;
using namespace std;
using namespace cv;
using namespace caffe;
using namespace gSLICr;

class RgbdSaliency
{
    public:
        RgbdSaliency();
        RgbdSaliency(bool showDebug, string net_proto_path, string net_binary_path);
        RgbdSaliency(bool showDebug, string net_proto_path, string net_binary_path, string rgb_image_path, string depth_image_path, string save_dirpath);
        RgbdSaliency(bool showDebug, string net_proto_path, string net_binary_path, string rgb_dirpath, string depth_dirpath, string fill_dirpath, string gap_dirpath, string save_dirpath);
        void detect();
        void detectAndWriteSingle();
        void detectAndWriteSingle(cv::Mat depth_image, cv::Mat rgb_image);
        ObjectSeg detectSingle(cv::Mat depth_image, cv::Mat rgb_image);
    private:
        ObjectSeg processImageAndWrite(bool write, string rgb_path, Mat rgb_image, Mat depth_image, Mat fill_image, Mat gap_image);
        void CustomResize(const Mat& img, Mat& output, cv::Size size);
        void CustomCVMatToDatum(const cv::Mat& cv_img, Datum* datum);
        void CustomDepthCVMatToDatum(const cv::Mat& cv_img, Datum* datum); 
        void processWrite(fs::path rgb_path_p, fs::path savepath, cv::Size original_size, const boost::shared_ptr<Blob<float>> slic_blob, const float* score_ptr);
        ObjectSeg processObjectSeg(cv::Size original_size, const boost::shared_ptr<Blob<float>> slic_blob, const float* score_ptr);

        float MEAN_BGR[3] = {104, 117, 123};
        const int fixed_size = 324;
        bool showDebug;
        string net_proto_path;
        string net_binary_path;
        string rgb_dirpath;
        string depth_dirpath;
        string rgb_image_path;
        string depth_image_path;        
        string fill_dirpath;
        string gap_dirpath;
        string save_dirpath;

        FillGapCalc *fillGapCalc;
        double total_count = 0;        
        std::unique_ptr<Net<float>> caffe_test_net;
};