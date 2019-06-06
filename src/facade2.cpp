#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API

#include <boost/python.hpp>
#include <pyboostcvconverter/pyboostcvconverter.hpp>
#include <rgbdsaliency.h>
#include <configprop.h>

namespace pbcvt {

    using namespace boost::python;



class Facade
{
    public:
        Facade(std::string configFilePath);
        ObjectSeg* segmentImage(const char* rgbFilePath, const char* depthFilePath, char *numObjects);
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

ObjectSeg* Facade::segmentImage(const char* rgbFilePath, const char* depthFilePath, char *numObjects) {
    if (showDebug) {
        cout << "RGB File to read: " << rgbFilePath << "\n";
        cout << "DEPTH File to read: " << depthFilePath << "\n";
    }

    cv::Mat kinect_rgb_img = cv::imread(rgbFilePath);
    cv::Mat kinect_depth_img_mm = cv::imread(depthFilePath);

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

/**
 * Example function. Basic inner matrix product using explicit matrix conversion.
 * @param left left-hand matrix operand (NdArray required)
 * @param right right-hand matrix operand (NdArray required)
 * @return an NdArray representing the dot-product of the left and right operands
 */
  /*  PyObject *dot(PyObject *left, PyObject *right) {

        cv::Mat leftMat, rightMat;
        leftMat = pbcvt::fromNDArrayToMat(left);
        rightMat = pbcvt::fromNDArrayToMat(right);
        auto c1 = leftMat.cols, r2 = rightMat.rows;
        // Check that the 2-D matrices can be legally multiplied.
        if (c1 != r2) {
            PyErr_SetString(PyExc_TypeError,
                            "Incompatible sizes for matrix multiplication.");
            throw_error_already_set();
        }
        cv::Mat result = leftMat * rightMat;
        PyObject *ret = pbcvt::fromMatToNDArray(result);
        return ret;
    }*/

//This example uses Mat directly, but we won't need to worry about the conversion
/**
 * Example function. Basic inner matrix product using implicit matrix conversion.
 * @param leftMat left-hand matrix operand
 * @param rightMat right-hand matrix operand
 * @return an NdArray representing the dot-product of the left and right operands
 */
  /*  cv::Mat dot2(cv::Mat leftMat, cv::Mat rightMat) {
        auto c1 = leftMat.cols, r2 = rightMat.rows;
        if (c1 != r2) {
            PyErr_SetString(PyExc_TypeError,
                            "Incompatible sizes for matrix multiplication.");
            throw_error_already_set();
        }
        cv::Mat result = leftMat * rightMat;

        return result;
    }*/

    Facade* Facade_new(const char* configFilePath) {return new Facade(std::string(configFilePath));}
    ObjectSeg* Facade_segmentImage(Facade* facade, const char* rgbFilePath, const char* depthFilePath, char *numObjects) {
        return facade->segmentImage(rgbFilePath, depthFilePath, numObjects);
    }

    void Facade_cleanupObjects(Facade* facade, ObjectSeg* objectToClean, char *numObjects) {
        facade->cleanupObjects(objectToClean, numObjects);
    }    


#if (PY_VERSION_HEX >= 0x03000000)

    static void *init_ar() {
#else
        static void init_ar(){
#endif
        Py_Initialize();

        import_array();
        return NUMPY_IMPORT_ARRAY_RETVAL;
    }

    BOOST_PYTHON_MODULE (pbcvt) {
        //using namespace XM;
        init_ar();

        //initialize converters
        to_python_converter<cv::Mat,
                pbcvt::matToNDArrayBoostConverter>();
        pbcvt::matFromNDArrayBoostConverter();

        //expose module-level functions
        def("Facade_new", Facade_new);
        def("Facade_segmentImage", Facade_segmentImage);
        def("Facade_cleanupObjects", Facade_cleanupObjects);

    }

} //end namespace pbcvt