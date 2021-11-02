//
// Created by Edu on 10/10/2021.
//

#ifndef SIMPLECONSOLE_CONSOLEMANAGER_H
#define SIMPLECONSOLE_CONSOLEMANAGER_H


#include <vector>
#include "Command.h"
#include "Prompt.h"

class ConsoleManager {
private:
    int cursorX;
    int cursorY;
    std::vector<Command> commands;
    Prompt prompt;
    size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);
public:
    ConsoleManager();
    Prompt getPrompt();

    std::string detectCommand(std::vector<std::string> parts);
    std::vector<CommandArgument> detectArguments(std::vector<std::string> parts);

    void mainLoop();
};

#endif //SIMPLECONSOLE_CONSOLEMANAGER_H
