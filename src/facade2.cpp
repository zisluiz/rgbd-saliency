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
        boost::python::list segmentImage(cv::Mat rgbImage, cv::Mat depthImage, int numObjects);
        void cleanupObjects(boost::python::list objectToClean, int numObjects);
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

boost::python::list Facade::segmentImage(cv::Mat rgbImage, cv::Mat depthImage, int numObjects) {
    if (showDebug) {
        cout << "Entrou em segmentImage" << "\n";
    }

    //cv::Mat kinect_rgb_img = pbcvt::fromNDArrayToMat(rgbImage);
    //cv::Mat kinect_depth_img_mm = pbcvt::fromNDArrayToMat(depthImage);

    int numResults = 1;
    //ObjectSeg *results = new ObjectSeg[numResults];
    boost::python::list results;

    if (showDebug)
        cout << "Starting detection" << "\n";

    results.append(rgbdSaliency->detectSingle(depthImage, rgbImage));
    if (showDebug)
        cout << "Preparing to result " << "\n";

    //std::stringstream ss;
    //ss << numResults;
    //string strNumResults = ss.str();

    //if (showDebug)
    //    cout << "Resulting a string of " << strNumResults << "\n";
   
    //strcpy(numObjects, std::to_string(numResults).c_str());
    *&numObjects = numResults;
    //strcpy(numObjects, "1");
    boost::python::scope().attr("numObjects") = numResults;

    if (showDebug)
        cout << "Resulting a integer of " << numObjects << "\n";    

 
    if (showDebug)
        printf("Resulting a array of %d\n", numResults);

    return results;
}

void Facade::cleanupObjects(boost::python::list objectToClean, int numObjects) {
    if (boost::python::len(objectToClean) > 0) {
        //int numObjts = atoi(numObjects);

        for(std::size_t objs=0; objs < numObjects; ++objs) {
            if (showDebug)
                printf("Cleaning object %d\n", objs);  
            ObjectSeg obj = (boost::python::extract<ObjectSeg>((objectToClean)[objs]));             
            if (boost::python::len(obj.points) > 0) {
                printf("Cleaning points of object %d\n", objs); 
                             
                delete &obj.points;
            }
        }

        if (showDebug)
            printf("Cleaning array of objects\n"); 

        delete &objectToClean;
    }
}  

/**
 * @brief Example function. Basic inner matrix product using explicit matrix conversion.
 * @param left left-hand matrix operand (NdArray required)
 * @param right right-hand matrix operand (NdArray required)
 * @return an NdArray representing the dot-product of the left and right operands
 */
    PyObject *dot(PyObject *left, PyObject *right) {

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
    }
/**
 * @brief Example function. Simply makes a new CV_16UC3 matrix and returns it as a numpy array.
 * @return The resulting numpy array.
 */

	PyObject* makeCV_16UC3Matrix(){
		cv::Mat image = cv::Mat::zeros(240,320, CV_16UC3);
		PyObject* py_image = pbcvt::fromMatToNDArray(image);
		return py_image;
	}

//
/**
 * @brief Example function. Basic inner matrix product using implicit matrix conversion.
 * @details This example uses Mat directly, but we won't need to worry about the conversion in the body of the function.
 * @param leftMat left-hand matrix operand
 * @param rightMat right-hand matrix operand
 * @return an NdArray representing the dot-product of the left and right operands
 */
    cv::Mat dot2(cv::Mat leftMat, cv::Mat rightMat) {
        auto c1 = leftMat.cols, r2 = rightMat.rows;
        if (c1 != r2) {
            PyErr_SetString(PyExc_TypeError,
                            "Incompatible sizes for matrix multiplication.");
            throw_error_already_set();
        }
        cv::Mat result = leftMat * rightMat;

        return result;
    }

    /**
     * \brief Example function. Increments all elements of the given matrix by one.
     * @details This example uses Mat directly, but we won't need to worry about the conversion anywhere at all,
     * it is handled automatically by boost.
     * \param matrix (numpy array) to increment
     * \return
     */
    cv::Mat increment_elements_by_one(cv::Mat matrix){
        matrix += 1.0;
        return matrix;
    }

    Facade *Facade_new(const char* configFilePath) {
        return new Facade(std::string(configFilePath));
    }

    boost::python::list Facade_segmentImage(Facade *facade, PyObject *rgbImage, PyObject *depthImage, int numObjects) {
        //Facade *facade;
        cout << "Entrou em Facade_segmentImage" << "\n";
        
        cv::Mat kinect_rgb_img = pbcvt::fromNDArrayToMat(rgbImage);
        cout << "Converteu 1" << "\n";
        cv::Mat kinect_depth_img_mm = pbcvt::fromNDArrayToMat(depthImage);

        cout << "Retornando de Facade_segmentImage" << "\n";

        return facade->segmentImage(kinect_rgb_img, kinect_depth_img_mm, numObjects);
    }

    void Facade_cleanupObjects(Facade *facade, boost::python::list objectToClean, int numObjects) {
        //Facade *facade;
        //ObjectSeg *objectToClean;
        /*if (!PyArg_Parse(facadePy, &facade))
        {
            return;
        }  
        if (!PyArg_Parse(objectToCleanPy, &objectToClean))
        {
            return;
        }*/                    
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

    BOOST_PYTHON_MODULE (librgbd_saliency) {
        //using namespace XM;
        init_ar();

        //initialize converters
        to_python_converter<cv::Mat,pbcvt::matToNDArrayBoostConverter>();
        matFromNDArrayBoostConverter();

        //expose module-level functions
        class_<Facade, boost::shared_ptr<Facade>>("Facade", init<const char*>());   

        class_<PointSeg>("PointSeg")
            .def_readwrite("x", &PointSeg::x)
            .def_readwrite("y", &PointSeg::y)
            .def_readwrite("z", &PointSeg::z);

        class_<ObjectSeg>("ObjectSeg")
            .def_readwrite("id", &ObjectSeg::id)
            .def_readwrite("pointsLength", &ObjectSeg::pointsLength)            
            .def_readwrite("points", &ObjectSeg::points);
        class_<boost::python::list>("ObjectSeg_vector");
            //.def(boost::python::vector_indexing_suite<std::vector<ObjectSeg>>());
                    
  
        //expose module-level functions
        def("Facade_new", &Facade_new, return_value_policy<manage_new_object>());
        def("Facade_segmentImage", &Facade_segmentImage);
        def("Facade_cleanupObjects", Facade_cleanupObjects);
		def("makeCV_16UC3Matrix", makeCV_16UC3Matrix);

		//from PEP8 (https://www.python.org/dev/peps/pep-0008/?#prescriptive-naming-conventions)
        //"Function names should be lowercase, with words separated by underscores as necessary to improve readability."
        def("increment_elements_by_one", increment_elements_by_one);
    }

} //end namespace pbcvt