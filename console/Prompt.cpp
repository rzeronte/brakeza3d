//
// Created by Edu on 10/10/2021.
//

#include <iostream>
#include "Prompt.h"

Prompt::Prompt() {
    this->prompt = "\n:>  ";
}

std::string Prompt::getPrompt() {
    return this->prompt;
}

Prompt::Prompt(std::string prompt) {
    this->prompt = prompt;
}

