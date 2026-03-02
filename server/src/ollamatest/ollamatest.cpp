#include "ollama.hpp"
#include "ollamatest.hpp"
#include <string>
#include <iostream>

std::string GenerateSummary(std::string transcript) {

    try {
    
    std::string prompt = "The following is a transcript from a radio broadcast. "
                         "Place the transcript into typical radio broadcast categories "
                         "(i.e. sports, music, religion, talkshow, etc.) then write a "
                         "one to two sentence overview: " + transcript;


    ollama::response response = ollama::generate("phi3:mini", prompt);


    std::stringstream ss;
    ss << response;
    
    return ss.str();
    
} catch (const std::exception& e) {
    std::cerr << "\n[CRITICAL ERROR] Ollama failed: " << e.what() << std::endl;
} catch (...) {
    std::cerr << "\n[CRITICAL ERROR] Unknown failure occurred." << std::endl;
}
    return "Error generating summary.";
    
}