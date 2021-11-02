//
// Created by Edu on 10/10/2021.
//

#ifndef SIMPLECONSOLE_COMMAND_H
#define SIMPLECONSOLE_COMMAND_H


#include <vector>
#include <string>
#include "CommandArgument.h"

class Command {
private:
    std::string command;
    std::vector<CommandArgument> arguments;
    std::string description;
public:
    Command(std::string command);
    std::string getCommand();
    std::string getDescription();
    std::vector<CommandArgument> getArguments();
    void setArguments(std::vector<CommandArgument> arguments);

    void show();
};


#endif //SIMPLECONSOLE_COMMAND_H
