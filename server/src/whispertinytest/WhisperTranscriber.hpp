// whisper_transcriber.hpp
#pragma once

#include <string>
#include <filesystem>
#include <array>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <cstdio>

namespace fs = std::filesystem;

class WhisperTranscriber {
public:
    // Constructor: optionally allow custom model path
    explicit WhisperTranscriber(const fs::path& model_path = "models/ggml-tiny.en.bin")
        : model_path_(model_path)
    {
        if (!fs::exists(model_path_)) {
            throw std::runtime_error("Model file does not exist: " + model_path_.string());
        }
    }

    // Transcribe a WAV file and return transcription as string
    std::string transcribe(const fs::path& audio_file) const {
        if (!fs::exists(audio_file)) {
            throw std::runtime_error("Audio file does not exist: " + audio_file.string());
        }

        std::ostringstream cmd;
        cmd << "./build/bin/whisper-cli"
            << " -m " << model_path_
            << " -f " << audio_file
            << " --output-txt=false"; // suppress writing files

        std::string result = exec_command(cmd.str());

        if (result.empty()) {
            throw std::runtime_error("Transcription failed or produced empty output.");
        }

        return result;
    }

private:
    fs::path model_path_;

    // Helper to execute a shell command and capture stdout
    static std::string exec_command(const std::string& cmd) {
        std::array<char, 128> buffer;
        std::string output;

        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("Failed to open pipe for command: " + cmd);
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            output += buffer.data();
        }

        return output;
    }
};