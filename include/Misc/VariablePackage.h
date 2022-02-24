//
// Created by darkhead on 29/12/19.
//

#ifndef BRAKEDA3D_VARIABLEPACKAGE_H
#define BRAKEDA3D_VARIABLEPACKAGE_H

#include <any>
#include <iostream>
#include <map>

class VariablePackage {
public:
    std::map<std::string, float> numericData;
    std::map<std::string, bool> boolData;
    std::map<std::string, char *> charData;

    float &numericValue(char *key);

    bool &booleanValue(char *key);

    std::string stringValue(const char *key);
};


#endif //BRAKEDA3D_VARIABLEPACKAGE_H
