//#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API
#include <rgbdsaliency.h>
#include <configprop.h>
#include <boost/python.hpp>
#include <pyboostcvconverter/pyboostcvconverter.hpp>

namespace pbcvt {
using namespace boost::python;

class Facade
{
    public:
        Facade(std::string configFilePath);
        ObjectSeg* segmentImage(PyObject *rgbImage, PyObject *depthImage, char *numObjects);
        void cleanupObjects(ObjectSeg* objectToClean, char *numObjects);
    private:
    string net_proto_path;
    string net_binary_path;
    bool showDebug;
    RgbdSaliency *rgbdSaliency;
};

Facade::Facade(std::string configFilePath)
{
    ConfigProperties *config = new ConfigProperties(configFilePath);
    net_proto_path = config->config["net_proto_path"];
    net_binary_path = config->config["net_binary_path"];
    showDebug = config->config["show_debug"] == "true";
    delete config; 

    rgbdSaliency = new RgbdSaliency(showDebug, net_proto_path, net_binary_path);
}

ObjectSeg* Facade::segmentImage(PyObject *rgbImage, PyObject *depthImage, char *numObjects) {
    if (showDebug) {
        cout << "Inside: segmentImage\n";
    }

    cv::Mat kinect_rgb_img = pbcvt::fromNDArrayToMat(rgbImage);
    cv::Mat kinect_depth_img_mm = pbcvt::fromNDArrayToMat(depthImage);

    if (showDebug) {
        cout << "Inside: segmentImage 2 \n";
    }    

    int numResults = 1;
    ObjectSeg *results = new ObjectSeg[numResults];

    if (showDebug)
        cout << "Starting detection" << "\n";

    results[0] = rgbdSaliency->detectSingle(kinect_depth_img_mm, kinect_rgb_img);
    if (showDebug)
        cout << "Preparing to result " << "\n";

    //std::stringstream ss;
    //ss << numResults;
    //string strNumResults = ss.str();

    //if (showDebug)
    //    cout << "Resulting a string of " << strNumResults << "\n";
   
    strcpy(numObjects, std::to_string(numResults).c_str());
    //strcpy(numObjects, "1");

 
    if (showDebug)
        printf("Resulting a array of %d\n", numResults);

    return results;
}

void Facade::cleanupObjects(ObjectSeg* objectToClean, char *numObjects) {
    if (objectToClean) {
        int numObjts = atoi(numObjects);

        for(std::size_t objs=0; objs < numObjts; ++objs) {
            if (showDebug)
                printf("Cleaning object %d\n", objs);            
            if (objectToClean[objs].points) {
                printf("Cleaning points of object %d\n", objs); 
                delete[] objectToClean[objs].points;
            }
        }

        if (showDebug)
            printf("Cleaning array of objects\n"); 

        delete[] objectToClean;
    }
} 
}


extern "C"
{   
    using namespace pbcvt;

    Facade* Facade_new(const char* configFilePath) {
            Py_Initialize();

            import_array();

            //initialize converters
            to_python_converter<cv::Mat,
                    pbcvt::matToNDArrayBoostConverter>();
            pbcvt::matFromNDArrayBoostConverter();  

        return new Facade(std::string(configFilePath));
    }
    ObjectSeg* Facade_segmentImage(Facade* facade, PyObject *rgbImage, PyObject *depthImage, char *numObjects) {
        return facade->segmentImage(rgbImage, depthImage, numObjects);
    }

    void Facade_cleanupObjects(Facade* facade, ObjectSeg* objectToClean, char *numObjects) {
        facade->cleanupObjects(objectToClean, numObjects);
    }
}
