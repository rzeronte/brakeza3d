//
// Created by Edu on 10/10/2021.
//

#include <iostream>
#include "ConsoleManager.h"

ConsoleManager::ConsoleManager() {
    this->cursorX = 0;
    this->cursorY = 0;
}

Prompt ConsoleManager::getPrompt() {
    return this->prompt;
}

size_t ConsoleManager::split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

std::string ConsoleManager::detectCommand(std::vector<std::string> parts) {
    return parts[0];
}

std::vector<CommandArgument> ConsoleManager::detectArguments(std::vector<std::string> parts) {
    std::vector<CommandArgument> arguments;
    for (int i = 1; i < parts.size(); i++) {
        CommandArgument argument(parts[i]);
        arguments.push_back(argument);
    }

    return arguments;
}

void ConsoleManager::mainLoop()
{
    std::cout << this->getPrompt().getPrompt();
    std::string line;

    while (true) {
        char c = (char) getchar();
        this->cursorX++;
        if (c == '\n' || c == EOF ) {
            this->cursorY++;
            std::vector<std::string> parts;
            this->split(line, parts, ' ');

            std::string command = this->detectCommand(parts);
            std::vector<CommandArgument> arguments = this->detectArguments(parts);

            Command cmd(command);
            cmd.setArguments(arguments);

            cmd.show();

            std::cout << this->getPrompt().getPrompt();
            line.clear();
            commands.push_back(cmd);
        } else {
            line.push_back(c);
        }
    }
}
