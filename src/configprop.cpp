#include "configprop.h"

ConfigProperties::ConfigProperties(std::string configFilePath) {
    std::ifstream input(configFilePath); //The input stream

    while(input) //Keep on going as long as the file stream is good
    {
        std::string line; //The line
        std::getline(input, line, '\n'); //Read up to the newline into value

        if (line.size() == 0 || line.at(0) == '#')
            continue;

        std::string::size_type posDiv = line.find_first_of("="); //Find the comment in the value
        std::string::size_type posComment = line.find_first_of("#", posDiv); //Find the comment in the value
        std::string::size_type posTab = line.find_first_of("\t", posDiv); //Find the comment in the value
        std::string::size_type posEspace = line.find_first_of(" ", posDiv); //Find the comment in the value
        std::string key = line.substr(0, posDiv); //The key
        
        std::string::size_type endValue = (posComment != -1 ? posComment : line.size()); //Find the comment in the value
        
        std::string::size_type firstQuote = line.find_first_of("\"", posDiv); //Find the comment in the value

        /*printf("line.size(): '%d'\n", line.size());
        printf("posDiv: '%d'\n", posDiv);
        printf("firstQuote: '%d'\n", firstQuote);
        printf("posComment: '%d'\n", posComment);
        printf("posTab: '%d'\n", posTab);
        printf("posEspace: '%d'\n", posEspace);*/

        if (firstQuote != -1) {
            endValue = line.find_last_of("\"")-1;
            posDiv = firstQuote;
        } else {
            if (posTab != -1 && posTab < endValue)
                endValue = posTab-1;
            if (posEspace != -1 && posEspace < endValue)
                endValue = posEspace-1;                
        }
        
        //printf("endValue: '%d'\n", endValue);
        //printf("endLength: '%d'\n", (endValue-posDiv));

        std::string value = line.substr(posDiv+1, endValue-posDiv); //The value

        //printf("key: '%s'\n", key.c_str());
        //printf("value: '%s'\n", value.c_str());  
        config[key] = value; //Store the result in the map
    }

    input.close(); //Close the file stream
}

