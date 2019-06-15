#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API

#include <boost/python.hpp>
#include <pyboostcvconverter/pyboostcvconverter.hpp>
#include <rgbdsaliency.h>
#include <configprop.h>
#include <vector>

namespace pbcvt {

    using namespace boost::python;

class Facade
{
    public:
        Facade(std::string configFilePath);
        boost::python::list segmentImage(cv::Mat rgbImage, cv::Mat depthImage);
        void cleanupObjects(boost::python::list objectToClean);
        int numObjects;
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

boost::python::list Facade::segmentImage(cv::Mat rgbImage, cv::Mat depthImage) {
    this->numObjects = 1;
    boost::python::list results;

    if (showDebug)
        cout << "Starting detection" << "\n";

    results.append(rgbdSaliency->detectSingle(depthImage, rgbImage));

    if (showDebug)
        printf("Resulting a array of %d\n", this->numObjects);

    return results;
}

/*void Facade::cleanupObjects(boost::python::list objectToClean) {
    if (this->numObjects > 0) {
        //int numObjts = atoi(numObjects);

        for(std::size_t objs=0; objs < this->numObjects; ++objs) {
            if (showDebug)
                printf("Cleaning object %d\n", objs);  
            ObjectSeg obj = (boost::python::extract<ObjectSeg>((objectToClean)[objs]));             
            if (boost::python::len(obj.points) > 0) {
                printf("Cleaning points of object %d\n", objs); 
                             
                delete *obj.points;
            }
        }
    }

    if (showDebug)
        printf("Cleaning array of objects\n");

    delete *objectToClean;
} */

Facade *Facade_new(const char* configFilePath) {
    return new Facade(std::string(configFilePath));
}

boost::python::list Facade_segmentImage(Facade *facade, PyObject *rgbImage, PyObject *depthImage) {
    cv::Mat kinect_rgb_img = pbcvt::fromNDArrayToMat(rgbImage);
    cv::Mat kinect_depth_img_mm = pbcvt::fromNDArrayToMat(depthImage);

    return facade->segmentImage(kinect_rgb_img, kinect_depth_img_mm);
}

/*void Facade_cleanupObjects(Facade *facade, boost::python::list objectToClean) {               
    facade->cleanupObjects(objectToClean);
}*/

#if (PY_VERSION_HEX >= 0x03000000)

    static void *init_ar() {
#else
        static void init_ar(){
#endif
        Py_Initialize();

        import_array();
        return NUMPY_IMPORT_ARRAY_RETVAL;
    }

    BOOST_PYTHON_MODULE (librgbd_saliency) {
        //using namespace XM;
        init_ar();

        //initialize converters
        to_python_converter<cv::Mat,pbcvt::matToNDArrayBoostConverter>();
        matFromNDArrayBoostConverter();

        //expose module-level functions
        class_<Facade, boost::shared_ptr<Facade>>("Facade", init<const char*>())
            .def_readwrite("numObjects", &Facade::numObjects);   

        class_<PointSeg>("PointSeg")
            .def_readwrite("x", &PointSeg::x)
            .def_readwrite("y", &PointSeg::y)
            .def_readwrite("z", &PointSeg::z);

        class_<ObjectSeg>("ObjectSeg")
            .def_readwrite("id", &ObjectSeg::id)
            .def_readwrite("pointsLength", &ObjectSeg::pointsLength)            
            .def_readwrite("points", &ObjectSeg::points);
        class_<boost::python::list>("ObjectSeg_vector");                 
  
        //expose module-level functions
        def("Facade_new", &Facade_new, return_value_policy<manage_new_object>());
        def("Facade_segmentImage", &Facade_segmentImage);
        //def("Facade_cleanupObjects", Facade_cleanupObjects);
    }

} //end namespace pbcvt