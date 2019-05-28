#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include "../slic/SLIC.h"
#include "../lbe/lbe.hpp"
#include <dirent.h>
#include <sys/stat.h>

class FillGapCalc
{
    public:
        FillGapCalc();
        void generateFillGapImages(std::string depth_dir, std::string rgb_dir, std::string fill_dir, std::string gap_dir, std::string ext, int bit_cnt);
        cv::Mat* generateFillGapSingle(cv::Mat depth_image, cv::Mat rgb_image);
    private:
        cv::Mat getSLICOSegmentation(cv::Mat &rgb_8UC3);
        std::vector<std::string> getExtFilesInDir ( std::string dir_path, std::string ext );
        cv::Mat loadDisparity(std::string image_path);
        cv::Mat loadDepth(std::string image_path);
        void runDemo(std::string depth_dir, std::string rgb_dir, std::string output_dir, std::string ext);
        std::vector<cv::Mat> fillListToImList(std::vector<std::vector<double> > fill_list, cv::Mat seg_32SC1);
        std::vector<cv::Mat> fillListToImageList(std::vector<std::vector<double> > fill_list, cv::Mat seg_32SC1);

        int n_partitions = 3;
        int n_out;
        // lbe computation object
        SaliencyLBE lbe;            
};

