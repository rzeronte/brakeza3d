//
// Created by Edu on 10/10/2021.
//

#include <iostream>
#include "Command.h"

Command::Command(std::string command) {
    this->command = command;
}

std::vector<CommandArgument> Command::getArguments() {
    return this->arguments;
}

std::string Command::getCommand() {
    return this->command;
}

std::string Command::getDescription() {
    return this->description;
}

void Command::setArguments(std::vector<CommandArgument> arguments) {
    this->arguments = arguments;
}

void Command::show() {
    std::cout << "Command: " << getCommand() << std::endl;
    std::cout << "Arguments:";
    for (int i = 0; i < getArguments().size(); i++) {
        std::cout << " " << getArguments()[i].getArgument();
    }
}
