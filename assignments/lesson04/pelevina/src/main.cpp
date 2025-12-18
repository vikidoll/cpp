#include "AiAgent.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./ai_agent <code_file>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Cannot open file: " << argv[1] << "\n";
        return 2;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();

    AiAgent agent;
    std::string err;

    if (!agent.loadConfig("config.json", &err)) {
        std::cerr << "Config error: " << err << "\n";
        return 3;
    }

    if (!agent.loadPrompt("prompt.json", &err)) {
        std::cerr << "Prompt error: " << err << "\n";
        return 4;
    }

    std::string finalPrompt = agent.getPrompt();
    const std::string marker = "{{CODE}}";
    auto pos = finalPrompt.find(marker);
    if (pos != std::string::npos) {
        finalPrompt.replace(pos, marker.size(), code);
    }

    agent.setPrompt(finalPrompt);

    auto response = agent.ask(&err);
    if (!response) {
        std::cerr << "Request failed: " << err << "\n";
        return 5;
    }

    std::cout << "=== CODE REVIEW RESULT ===\n";
    std::cout << *response << "\n";

    return 0;
}
