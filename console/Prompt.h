//
// Created by Edu on 10/10/2021.
//

#ifndef SIMPLECONSOLE_PROMPT_H
#define SIMPLECONSOLE_PROMPT_H


#include <string>

class Prompt {
private:
    std::string prompt;
public:
    Prompt(std::string prompt);

    Prompt();

    std::string getPrompt();
};


#endif //SIMPLECONSOLE_PROMPT_H
