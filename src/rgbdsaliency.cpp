#include "rgbdsaliency.h"

RgbdSaliency::RgbdSaliency() {
}

RgbdSaliency::RgbdSaliency(bool showDebug, string net_proto_path, string net_binary_path, string rgb_dirpath, string depth_dirpath, string fill_dirpath, string gap_dirpath, string save_dirpath)
{
  this->showDebug = showDebug;
  this->net_proto_path = net_proto_path;
  this->net_binary_path = net_binary_path;
  this->rgb_dirpath = rgb_dirpath;
  this->depth_dirpath = depth_dirpath;
  this->fill_dirpath = fill_dirpath;
  this->gap_dirpath = gap_dirpath;
  this->save_dirpath = save_dirpath;
  fillGapCalc = new FillGapCalc(showDebug);

  Caffe::set_mode(Caffe::Brew::GPU);
  Caffe::SetDevice(0);
  caffe_test_net.reset(new Net<float>(net_proto_path, caffe::TEST));
  caffe_test_net->CopyTrainedLayersFrom(net_binary_path);    
}

RgbdSaliency::RgbdSaliency(bool showDebug, string net_proto_path, string net_binary_path, string rgb_image_path, string depth_image_path, string save_dirpath)
{
  this->showDebug = showDebug;
  this->net_proto_path = net_proto_path;
  this->net_binary_path = net_binary_path;
  this->rgb_image_path = rgb_image_path;
  this->depth_image_path = depth_image_path;
  this->save_dirpath = save_dirpath;
  fillGapCalc = new FillGapCalc(showDebug);

  Caffe::set_mode(Caffe::Brew::GPU);
  Caffe::SetDevice(0);
  caffe_test_net.reset(new Net<float>(net_proto_path, caffe::TEST));
  caffe_test_net->CopyTrainedLayersFrom(net_binary_path);       
}

RgbdSaliency::RgbdSaliency(bool showDebug, string net_proto_path, string net_binary_path)
{
  this->showDebug = showDebug;
  this->net_proto_path = net_proto_path;
  this->net_binary_path = net_binary_path;
  fillGapCalc = new FillGapCalc(showDebug);

  Caffe::set_mode(Caffe::Brew::GPU);
  Caffe::SetDevice(0);
  caffe_test_net.reset(new Net<float>(net_proto_path, caffe::TEST));
  caffe_test_net->CopyTrainedLayersFrom(net_binary_path);       
}

void RgbdSaliency::detect() {
  // collect filenames
  fs::directory_iterator rgbdir_iter = fs::directory_iterator(rgb_dirpath);
  fs::directory_iterator rgb_end_iter;
  vector<string> rgb_filenames;
  while (rgbdir_iter != rgb_end_iter) {
    string rgbname = rgbdir_iter->path().string();
    rgb_filenames.push_back(rgbname);
    rgbdir_iter++;
  }
  std::sort(rgb_filenames.begin(), rgb_filenames.end());

  fs::directory_iterator depthdir_iter = fs::directory_iterator(depth_dirpath);
  fs::directory_iterator depth_end_iter;
  vector<string> depth_filenames;
  while (depthdir_iter != depth_end_iter) {
    string depthname = depthdir_iter->path().string();
    depth_filenames.push_back(depthname);
    depthdir_iter++;
  }
  std::sort(depth_filenames.begin(), depth_filenames.end());

  fs::directory_iterator filldir_iter = fs::directory_iterator(fill_dirpath);
  fs::directory_iterator fill_end_iter;
  vector<string> fill_filenames;
  while (filldir_iter != fill_end_iter) {
    string fillname = filldir_iter->path().string();
    fill_filenames.push_back(fillname);
    filldir_iter++;
  }
  std::sort(fill_filenames.begin(), fill_filenames.end());

  fs::directory_iterator gapdir_iter = fs::directory_iterator(gap_dirpath);
  fs::directory_iterator gap_end_iter;
  vector<string> gap_filenames;
  while (gapdir_iter != gap_end_iter) {
    string gapname = gapdir_iter->path().string();
    gap_filenames.push_back(gapname);
    gapdir_iter++;
  }
  std::sort(gap_filenames.begin(), gap_filenames.end());

  if (rgb_filenames.size() != depth_filenames.size()){
    std::cout << "error image num in loading image" << endl;
    return;
  }
  if (rgb_filenames.size() != fill_filenames.size()){
    std::cout << "error image num in loading image" << endl;
    return;
  }
  if (rgb_filenames.size() != gap_filenames.size()){
    std::cout << "error image num in loading image" << endl;
    return;
  }
  int total_num = rgb_filenames.size();

  for (int imgidx = 0; imgidx < total_num; imgidx++) {
    Mat rgb_image = cv::imread(rgb_filenames[imgidx]);
    Mat depth_image = cv::imread(depth_filenames[imgidx]);
    Mat fill_image = cv::imread(fill_filenames[imgidx]);
    Mat gap_image = cv::imread(gap_filenames[imgidx]);    
    processImageAndWrite(true, rgb_filenames[imgidx], rgb_image, depth_image, fill_image, gap_image);
  }

  cout <<  "AVG time : " << total_count / total_num << "s" << endl;    
}

void RgbdSaliency::detectAndWriteSingle() {
  Mat rgb_image = cv::imread(rgb_image_path);
  Mat depth_image = cv::imread(depth_image_path);

  std::unique_ptr<cv::Mat[]> results = fillGapCalc->generateFillGapSingle(depth_image, rgb_image);

  processImageAndWrite(true, rgb_image_path, rgb_image, depth_image, results[0], results[1]);

  results.reset();

  cout <<  "AVG time : " << total_count << "s" << endl;    
}

void RgbdSaliency::detectAndWriteSingle(cv::Mat depth_image, cv::Mat rgb_image) {
  std::unique_ptr<cv::Mat[]> results = fillGapCalc->generateFillGapSingle(depth_image, rgb_image);
  processImageAndWrite(true, rgb_image_path, rgb_image, depth_image, results[0], results[1]);

  results.reset();
  cout <<  "AVG time : " << total_count << "s" << endl;    
}

ObjectSeg RgbdSaliency::detectSingle(cv::Mat depth_image, cv::Mat rgb_image) {
  std::unique_ptr<cv::Mat[]> results = fillGapCalc->generateFillGapSingle(depth_image, rgb_image);

  ObjectSeg obj = processImageAndWrite(false, "", rgb_image, depth_image, results[0], results[1]);
  results.reset();
  //cout <<  "AVG time : " << total_count << "s" << endl;  
  return obj;  
}

ObjectSeg RgbdSaliency::processImageAndWrite(bool write, string rgb_path, Mat rgb_image, Mat depth_image, Mat fill_image, Mat gap_image) {
    if (showDebug)
        cout << "Processing rgb, depth, fill and gap images: " << "\n";    
  //boost::chrono::system_clock::time_point start;
  //start = boost::chrono::system_clock::now(); 

    if (showDebug)
        cout << "Fixed size: " << fixed_size << "\n";    

  cv::Size original_size = cv::Size(rgb_image.cols, rgb_image.rows);
  cv::resize(rgb_image, rgb_image, cv::Size(fixed_size, fixed_size));
    if (showDebug)
        cout << "Resized rgb: " << "\n";    

  cv::resize(depth_image, depth_image, cv::Size(fixed_size, fixed_size));
    if (showDebug)
        cout << "Depth rgb: " << "\n";      
  cv::resize(fill_image, fill_image, cv::Size(fixed_size, fixed_size));
      if (showDebug)
        cout << "Fill rgb: " << "\n";    
  cv::resize(gap_image, gap_image, cv::Size(fixed_size, fixed_size));
    if (showDebug)
        cout << "Gap rgb: " << "\n";      

  vector<Datum> input_rgb_datum;
  input_rgb_datum.emplace_back();
  CustomCVMatToDatum(rgb_image, &(input_rgb_datum[0]));
  boost::shared_ptr<MemoryDataLayer<float>> input_rgb_layer =
    boost::static_pointer_cast<MemoryDataLayer<float>>(
              caffe_test_net->layer_by_name("data"));
  input_rgb_layer->AddDatumVector(input_rgb_datum);

  vector<Datum> input_depth_datum;
  input_depth_datum.emplace_back();
  CustomDepthCVMatToDatum(depth_image, &(input_depth_datum[0]));
  boost::shared_ptr<MemoryDataLayer<float>> input_depth_layer =
    boost::static_pointer_cast<MemoryDataLayer<float>>(
              caffe_test_net->layer_by_name("depthdata"));
  input_depth_layer->AddDatumVector(input_depth_datum);

  vector<Datum> input_fill_datum;
  input_fill_datum.emplace_back();
  CustomDepthCVMatToDatum(fill_image, &(input_fill_datum[0]));
  boost::shared_ptr<MemoryDataLayer<float>> input_fill_layer =
    boost::static_pointer_cast<MemoryDataLayer<float>>(
              caffe_test_net->layer_by_name("filldata"));
  input_fill_layer->AddDatumVector(input_fill_datum);

  vector<Datum> input_gap_datum;
  input_gap_datum.emplace_back();
  CustomDepthCVMatToDatum(gap_image, &(input_gap_datum[0]));
  boost::shared_ptr<MemoryDataLayer<float>> input_gap_layer =
    boost::static_pointer_cast<MemoryDataLayer<float>>(
              caffe_test_net->layer_by_name("gapdata"));
  input_gap_layer->AddDatumVector(input_gap_datum);

  vector<Blob<float>*> empty_vec;
  caffe_test_net->Forward(empty_vec);

  const boost::shared_ptr<Blob<float>> slic_blob =
    caffe_test_net->blob_by_name("slic");
  const boost::shared_ptr<Blob<float>> score_blob =
    caffe_test_net->blob_by_name("score");
  const float* score_ptr = score_blob->cpu_data();

  if (write) {
    fs::path rgb_path_p(rgb_path);
    fs::path savepath(save_dirpath);
    processWrite(rgb_path_p, savepath, original_size, slic_blob, score_ptr);
  } else
    return processObjectSeg(original_size, slic_blob, score_ptr);
}

void RgbdSaliency::processWrite(fs::path rgb_path_p, fs::path savepath, cv::Size original_size, const boost::shared_ptr<Blob<float>> slic_blob, const float* score_ptr) {
    if (showDebug)
        cout << "Processing write result: " << "\n";    
  Mat result(fixed_size, fixed_size, CV_8UC1);
  const float* slic_ptr = slic_blob->cpu_data();
  float score_sum = 0;
  int score_count = 0;

  for (int i = 0; i < fixed_size; i++) {
    for (int j = 0; j < fixed_size; j++) {
      const float index = slic_ptr[i*fixed_size + j];
      const float score = score_ptr[(int)(index)];
      result.at<uchar>(i, j) = static_cast<uchar>(score*255);
    }
  }

  resize(result, result, original_size);
  //boost::chrono::duration<double> sec2 = boost::chrono::system_clock::now() - start;
  //cout << "time : " << sec2.count() << "s" << endl;
  //total_count += (double)sec2.count();

  string outimg = rgb_path_p.filename().string();
  outimg.replace(outimg.end()-4, outimg.end(), ".png");
  fs::path filename(outimg);
  fs::path outputpath = savepath / outimg;
  imwrite(outputpath.string(), result);
  cout << outputpath << endl;
}

ObjectSeg RgbdSaliency::processObjectSeg(cv::Size original_size, const boost::shared_ptr<Blob<float>> slic_blob, const float* score_ptr) {
    if (showDebug)
        cout << "Processing object result: " << "\n";  
  Mat result(fixed_size, fixed_size, CV_8UC1);
  const float* slic_ptr = slic_blob->cpu_data();
  float score_sum = 0;
  int score_count = 0;

  for (int i = 0; i < fixed_size; i++) {
    for (int j = 0; j < fixed_size; j++) {
      const float index = slic_ptr[i*fixed_size + j];
      const float score = score_ptr[(int)(index)];
      result.at<uchar>(i, j) = static_cast<uchar>(score*255);
    }
  }
  resize(result, result, original_size);

  cv::Size s = result.size();
  int rows = s.height;
  int cols = s.width;  

  //PointSeg *points = (PointSeg*) malloc((rows*cols) * sizeof(PointSeg));
  boost::python::list points;
  int numPoints = 0;

  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      const int color = static_cast<int>(result.at<uchar>(j,i));
      //cout << "Printing result" << j << "," << i << "value: " << color << "\n";
      
      if (color > 0) {
        //cout << "Pushing back element " << j << "," << i << "value: " << color << "\n";
        points.append(PointSeg(i, j, 0));
        numPoints++;
      }  
    }
  }

  //boost::chrono::duration<double> sec2 = boost::chrono::system_clock::now() - start;
  //cout << "time : " << sec2.count() << "s" << endl;
  //total_count += (double)sec2.count();

  //points = (PointSeg*) realloc(points, numPoints * sizeof(PointSeg));
  //return ObjectSeg(1, numPoints , points);
  return ObjectSeg(1, numPoints , points);
}

void RgbdSaliency::CustomResize(const Mat& img, Mat& output, cv::Size size) {

  Mat newmap(size, CV_8UC1);
  float mul_x = img.cols / (float) (size.width);
  float mul_y = img.rows / (float) (size.height);
  for (int i = 0; i < size.height; i++) {
    for (int j = 0; j < size.width; j ++) {
      newmap.at<uchar>(i, j) = static_cast<uchar>(img.at<float>((int)(i*mul_y),
          (int)(j*mul_x)) * 255);
    }
  }
  output = newmap.clone();
}

void RgbdSaliency::CustomCVMatToDatum(const cv::Mat& cv_img, Datum* datum) {
  CHECK(cv_img.depth() == CV_8U) << "image data type must be unsigned byte";
  datum->set_channels(cv_img.channels());
  datum->set_height(cv_img.rows);
  datum->set_width(cv_img.cols);
  datum->clear_data();
  datum->clear_float_data();
  datum->set_encoded(false);
  int datum_channels = datum->channels();
  int datum_height = datum->height();
  int datum_width = datum->width();
  int datum_size = datum_channels * datum_height * datum_width;

  for (int ch = 0; ch < datum_channels; ch++) {
    for (int r = 0; r < datum_height; r++) {
      for (int c = 0; c < datum_width; c++) {
        unsigned char mat_val = cv_img.at<Vec3b>(r, c)[ch];
        datum->add_float_data(static_cast<float>(mat_val) - MEAN_BGR[ch]);
      }
    }
  }
}

void RgbdSaliency::CustomDepthCVMatToDatum(const cv::Mat& cv_img, Datum* datum) {
  CHECK(cv_img.depth() == CV_8U) << "image data type must be unsigned byte";
  datum->set_channels(cv_img.channels());
  datum->set_height(cv_img.rows);
  datum->set_width(cv_img.cols);
  datum->clear_data();
  datum->clear_float_data();
  datum->set_encoded(false);
  int datum_channels = datum->channels();
  int datum_height = datum->height();
  int datum_width = datum->width();
  int datum_size = datum_channels * datum_height * datum_width;

  for (int ch = 0; ch < datum_channels; ch++) {
    for (int r = 0; r < datum_height; r++) {
      for (int c = 0; c < datum_width; c++) {
        unsigned char mat_val = cv_img.at<Vec3b>(r, c)[ch];
        datum->add_float_data(static_cast<float>(mat_val) - 127);
      }
    }
  }
}