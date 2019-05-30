#include <rgbdsaliency.h>

int main(int argc, char *argv[]) {
  if (argc == 8) {
    string net_proto_path(argv[1]);
    string net_binary_path(argv[2]);
    string rgb_dirpath = argv[3];
    string depth_dirpath = argv[4];
    string fill_dirpath = argv[5];
    string gap_dirpath = argv[6];
    string save_dirpath = argv[7];

    RgbdSaliency *rgbdSaliency = new RgbdSaliency(net_proto_path, net_binary_path, rgb_dirpath, depth_dirpath, fill_dirpath, gap_dirpath, save_dirpath);
    rgbdSaliency->detect();
  } else if (argc == 6) {
      string net_proto_path(argv[1]);
      string net_binary_path(argv[2]);
      string rgb_dirpath = argv[3];
      string depth_dirpath = argv[4];
      string save_dirpath = argv[5];

      RgbdSaliency *rgbdSaliency = new RgbdSaliency(net_proto_path, net_binary_path, rgb_dirpath, depth_dirpath, save_dirpath);
      Mat rgb_image = cv::imread(rgb_dirpath);
      Mat depth_image = cv::imread(depth_dirpath);
      /*rgbdSaliency->detectAndWriteSingle(depth_image, rgb_image);*/
      rgbdSaliency->detectSingle(depth_image, rgb_image);      
  } else
    return -1;  

  return 0;
}