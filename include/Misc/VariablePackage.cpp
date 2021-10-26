//
// Created by darkhead on 29/12/19.
//

#include "VariablePackage.h"

float &VariablePackage::numericValue(char *key) {
    return (float &) (numericData[key]);
}

bool &VariablePackage::booleanValue(char *key) {
    return (bool &) boolData[key];
}

std::string VariablePackage::stringValue(const char *key) {
    return (char *) (charData[key]);
}
