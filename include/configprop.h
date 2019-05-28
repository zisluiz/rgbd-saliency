#include <fstream>
#include <string>
#include <map>

using namespace std;

class ConfigProperties
{
    public:
        ConfigProperties(std::string configFilePath);
        std::map<std::string,std::string> config;
};