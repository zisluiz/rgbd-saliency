#! /bin/sh
#./create_fill_gap/build/demo test_depth test_rgb test_fill test_gap .png 16
./build/get_results RGBDNet_test.prototxt RGBD1000.caffemodel test_rgb/ test_depth/ test_fill/ test_gap/ results/
