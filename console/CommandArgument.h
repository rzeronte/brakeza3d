//
// Created by Edu on 10/10/2021.
//

#ifndef SIMPLECONSOLE_COMMANDARGUMENT_H
#define SIMPLECONSOLE_COMMANDARGUMENT_H


#include <string>

class CommandArgument {
private:
    std::string argument;
public:
    std::string getArgument();

    CommandArgument(std::string argument);
};


#endif //SIMPLECONSOLE_COMMANDARGUMENT_H
