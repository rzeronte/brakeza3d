//
// Created by Edu on 10/10/2021.
//

#include "CommandArgument.h"

std::string CommandArgument::getArgument() {
    return this->argument;
}

CommandArgument::CommandArgument(std::string argument) {
    this->argument = argument;
}
