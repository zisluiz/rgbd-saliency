#include <rgbdsaliency.h>
#include <configprop.h>

class Facade
{
    public:
        Facade(std::string configFilePath);
        ObjectSeg* segmentImage(const char* rgbFilePath, const char* depthFilePath, bool showDebug, char *numObjects);
        void cleanupObjects(ObjectSeg* objectToClean, char *numObjects);
    private:
    string net_proto_path;
    string net_binary_path;
    RgbdSaliency *rgbdSaliency;
};

Facade::Facade(std::string configFilePath)
{
    ConfigProperties *config = new ConfigProperties(configFilePath);
    net_proto_path = std::stod(config->config["net_proto_path"]);
    net_binary_path = std::stod(config->config["net_binary_path"]);
    delete config; 

    rgbdSaliency = new RgbdSaliency(net_proto_path, net_binary_path);
}

ObjectSeg* Facade::segmentImage(const char* rgbFilePath, const char* depthFilePath, bool showDebug, char *numObjects) {
    cv::Mat kinect_rgb_img = cv::imread(rgbFilePath);//,cv::IMREAD_UNCHANGED);
    cv::Mat kinect_depth_img_mm = cv::imread(depthFilePath,cv::IMREAD_UNCHANGED);// in mm

    int numResults = 1;
    ObjectSeg *results = new ObjectSeg[numResults];
    results[0] = rgbdSaliency->detectSingle(kinect_depth_img_mm, kinect_rgb_img);

    strcpy(numObjects, std::to_string(numResults).c_str());
 
    if (showDebug)
        printf("Resulting a array of %d\n", numResults);

    return results;
}

void Facade::cleanupObjects(ObjectSeg* objectToClean, char *numObjects) {
    if (objectToClean) {
        int numObjts = atoi(numObjects);

        for(std::size_t objs=0; objs < numObjts; ++objs) {
            if (objectToClean[objs].points) {
                delete[] objectToClean[objs].points;
            }
        }

        delete[] objectToClean;
    }      
} 

extern "C"
{   
    Facade* Facade_new(const char* configFilePath) {return new Facade(std::string(configFilePath));}
    ObjectSeg* Facade_segmentImage(Facade* facade, const char* rgbFilePath, const char* depthFilePath, bool showDebug, char *numObjects) {
        return facade->segmentImage(rgbFilePath, depthFilePath, showDebug, numObjects);
    }

    void Facade_cleanupObjects(Facade* facade, ObjectSeg* objectToClean, char *numObjects) {
        facade->cleanupObjects(objectToClean, numObjects);
    }      
}