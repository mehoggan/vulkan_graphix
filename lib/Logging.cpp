#include "vulkan_graphix/Logging.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <thread>

namespace vulkan_graphix {
std::atomic<bool> Logging::s_init(false);
Logging::Dict Logging::s_loggers;
std::mutex Logging::s_loggers_mutex;
std::atomic<unsigned int> Logging::s_line_id(0);

namespace {
const char* severityName(SeverityLevel level) {
    switch (level) {
        case SeverityLevel::trace:
            return "trace";
        case SeverityLevel::debug:
            return "debug";
        case SeverityLevel::info:
            return "info";
        case SeverityLevel::warning:
            return "warning";
        case SeverityLevel::error:
            return "error";
        case SeverityLevel::fatal:
            return "fatal";
    }
    return "unknown";
}

// Matches boost::filesystem::unique_path()'s own substitution rule: each
// '%' in the model is replaced by one random hex digit. Purely a
// character-level substitution - like the boost call it replaces, the
// result is relative to the current directory unless the model itself
// supplies a directory component (mktmpdir() doesn't anchor it to the
// system temp directory).
std::filesystem::path substitutePercents(const std::filesystem::path& model) {
    static std::mt19937_64 engine{std::random_device{}()};
    static constexpr char c_hex_digits[] = "0123456789abcdef";
    std::uniform_int_distribution<int> distribution(0, 15);

    std::string result = model.string();
    for (char& character : result) {
        if (character == '%') {
            character = c_hex_digits[distribution(engine)];
        }
    }
    return std::filesystem::path(result);
}
}  // namespace

LogTag::LogTag(const ::std::string& tag) : tag_(tag) {}

const char* LogTag::tag() const { return tag_.c_str(); }

bool Logging::init() {
    if (!s_init.load()) {
        s_init.store(true);
    }
    return s_init.load();
}

std::filesystem::path Logging::mktmpdir(const std::filesystem::path& model) {
    std::filesystem::path tmppath = substitutePercents(model);
    std::filesystem::create_directories(tmppath);
    return tmppath;
}

bool Logging::addStdCoutLogger(const LogTag& tag, SeverityLevel level) {
    std::lock_guard<std::mutex> lock(Logging::s_loggers_mutex);
    if (Logging::s_loggers.find(tag) != Logging::s_loggers.end()) {
        return false;
    }
    Logging::s_loggers.emplace(tag, TextSink{&std::cout, level, nullptr});
    return true;
}

bool Logging::addStdCerrLogger(const LogTag& tag, SeverityLevel level) {
    std::lock_guard<std::mutex> lock(Logging::s_loggers_mutex);
    if (Logging::s_loggers.find(tag) != Logging::s_loggers.end()) {
        return false;
    }
    Logging::s_loggers.emplace(tag, TextSink{&std::cerr, level, nullptr});
    return true;
}

bool Logging::addStdLogLogger(const LogTag& tag, SeverityLevel level) {
    std::lock_guard<std::mutex> lock(Logging::s_loggers_mutex);
    if (Logging::s_loggers.find(tag) != Logging::s_loggers.end()) {
        return false;
    }
    Logging::s_loggers.emplace(tag, TextSink{&std::clog, level, nullptr});
    return true;
}

bool Logging::addFileLogger(const LogTag& tag,
                            const std::filesystem::path& log_path,
                            SeverityLevel level) {
    std::lock_guard<std::mutex> lock(Logging::s_loggers_mutex);
    if (Logging::s_loggers.find(tag) != Logging::s_loggers.end()) {
        return false;
    }
    auto file_stream = std::make_unique<std::ofstream>(log_path);
    std::ostream* stream = file_stream.get();
    Logging::s_loggers.emplace(
            tag, TextSink{stream, level, std::move(file_stream)});
    return true;
}

void Logging::writeSeverityLog(const LogTag& tag,
                               SeverityLevel level,
                               const std::string& message) {
    Logging::init();

    unsigned int line_id = ++Logging::s_line_id;

    std::lock_guard<std::mutex> lock(Logging::s_loggers_mutex);
    auto sink_it = Logging::s_loggers.find(tag);
    if (sink_it == Logging::s_loggers.end() || level < sink_it->second.level) {
        return;
    }

    std::time_t now_time =
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm local_tm{};
    localtime_r(&now_time, &local_tm);

    std::ostream& out_stream = *sink_it->second.stream;
    out_stream << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << " ["
               << std::this_thread::get_id() << "] " << tag.tag() << " "
               << severityName(level) << "(" << line_id << ")" << " - '"
               << message << "'\n";
    out_stream.flush();
}

bool Logging::clearAll() {
    std::lock_guard<std::mutex> lock(Logging::s_loggers_mutex);
    Logging::s_loggers.clear();
    return Logging::s_loggers.empty();
}
}  // namespace vulkan_graphix
