//  main.cpp: demo of LBE saliency
//------------------------------------------------------------------
//  This code computes the LBE saliency maps given a set of depth
//  and RGB image pairs. Note that the RGB images are only required
//  for computing the superpixel segmentation.
//
//  The demo takes four command line arguments:
//    1. path to folder containing depth images
//    2. path to folder containing rgb images
//    3. path to output folder where computed lbe map will
//       be saved
//    4. (optional) image extension of depth and RGB files. Default
//       value is .jpg
//------------------------------------------------------------------
//  Copyright (c) 2016 David Feng NICTA/Data61. All rights reserved.
//  Email: firstname.lastname@data61.csiro.au
//------------------------------------------------------------------
#include <iostream>
#include "include/fillgapcalc.h"

int main(int argc, const char * argv[])
{
  if (argc<6)
    {
      std::cout << "Usage: ./" << argv[0] <<
        " [depth_path] [rgb_path] [output_fill_path] [output_gap_path] [img_ext(optional)] [bit]" << std::endl;
    }

  std::string depth_path = argv[1];
  std::string rgb_path = argv[2];
  std::string fill_path = argv[3];
  std::string gap_path = argv[4];
  std::string ext = argv[5];
  int bit_cnt = std::atoi(argv[6]);

  FillGapCalc *fillGapCalc = new FillGapCalc();
  fillGapCalc->generateFillGapImages(depth_path, rgb_path, fill_path, gap_path, ext, bit_cnt);

  return 0;
}
