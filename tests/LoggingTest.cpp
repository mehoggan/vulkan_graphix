// Exercises vulkan_graphix::Logging directly - pure std::filesystem/
// ostream code, no Vulkan device or X11 window needed. TestLogging.
// testClearAll is named to match the FRIEND_TEST(TestLogging, testClearAll)
// already declared in Logging.h, giving it access to the private
// s_loggers map.
#include "vulkan_graphix/Logging.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

using vulkan_graphix::Logging;
using vulkan_graphix::LogTag;

// FRIEND_TEST(TestLogging, testClearAll) in Logging.h is declared inside
// namespace vulkan_graphix, so the friended class it names is
// vulkan_graphix::TestLogging_testClearAll_Test (unqualified friend names
// resolve into the nearest enclosing namespace) - this TEST() must live in
// that same namespace, not the global one, to actually match it.
namespace vulkan_graphix {

TEST(TestLogging, testClearAll) {
    LogTag log_tag("logging-test-clear-all");
    ASSERT_TRUE(Logging::addStdCoutLogger(log_tag));
    EXPECT_FALSE(Logging::s_loggers.empty());

    EXPECT_TRUE(Logging::clearAll());

    EXPECT_TRUE(Logging::s_loggers.empty());
}

}  // namespace vulkan_graphix

TEST(LoggingTest, InitIsIdempotentAndReturnsTrue) {
    EXPECT_TRUE(Logging::init());
    EXPECT_TRUE(Logging::init());
}

TEST(LoggingTest, AddStdCoutLoggerRejectsADuplicateTag) {
    LogTag log_tag("logging-test-dup-cout");
    EXPECT_TRUE(Logging::addStdCoutLogger(log_tag));
    EXPECT_FALSE(Logging::addStdCoutLogger(log_tag));
    Logging::clearAll();
}

TEST(LoggingTest, AddStdCerrLoggerRejectsADuplicateTag) {
    LogTag log_tag("logging-test-dup-cerr");
    EXPECT_TRUE(Logging::addStdCerrLogger(log_tag));
    EXPECT_FALSE(Logging::addStdCerrLogger(log_tag));
    Logging::clearAll();
}

TEST(LoggingTest, AddStdLogLoggerRejectsADuplicateTag) {
    LogTag log_tag("logging-test-dup-stdlog");
    EXPECT_TRUE(Logging::addStdLogLogger(log_tag));
    EXPECT_FALSE(Logging::addStdLogLogger(log_tag));
    Logging::clearAll();
}

TEST(LoggingTest, LogTagEqualityComparesByTagString) {
    LogTag first("same-value");
    LogTag second("same-value");
    LogTag different("different-value");

    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first == different);
}

TEST(LoggingTest, LogTagStreamsItsTagStringToAnOstream) {
    LogTag log_tag("streamed-value");
    std::ostringstream out_stream;
    out_stream << log_tag;
    EXPECT_EQ(out_stream.str(), "streamed-value");
}

TEST(LoggingTest, LogTagStreamsItsTagStringToAStringstream) {
    LogTag log_tag("stringstream-value");
    std::stringstream sstream;
    sstream << log_tag;
    EXPECT_EQ(sstream.str(), "stringstream-value");
}

TEST(LoggingTest, LogTagForThisIsStableForTheSameInstance) {
    int value = 0;
    LogTag first = Logging::logTagForThis(value);
    LogTag second = Logging::logTagForThis(value);
    EXPECT_TRUE(first == second);
}

TEST(LoggingTest, MktmpdirCreatesARealDirectory) {
    std::filesystem::path tmp_dir = Logging::mktmpdir();

    EXPECT_TRUE(std::filesystem::exists(tmp_dir));
    EXPECT_TRUE(std::filesystem::is_directory(tmp_dir));

    std::filesystem::remove_all(tmp_dir);
}

TEST(LoggingTest, AddFileLoggerWritesTheLoggedMessageToDisk) {
    std::filesystem::path tmp_dir = Logging::mktmpdir();
    std::filesystem::path log_file = tmp_dir / "logging_test.log";
    LogTag log_tag("logging-test-file");

    ASSERT_TRUE(Logging::addFileLogger(log_tag, log_file));
    Logging::info(log_tag, "hello", "file", "logger");

    ASSERT_TRUE(std::filesystem::exists(log_file));
    std::ifstream in_stream(log_file.string());
    std::string contents((std::istreambuf_iterator<char>(in_stream)),
                         std::istreambuf_iterator<char>());
    EXPECT_NE(contents.find("hello file logger"), std::string::npos);

    Logging::clearAll();
    std::filesystem::remove_all(tmp_dir);
}

TEST(LoggingTest, SeverityLevelLoggingCallsDoNotCrash) {
    LogTag log_tag("logging-test-severity");
    ASSERT_TRUE(Logging::addStdCoutLogger(log_tag, VULKAN_GRAPHIX_TRACE));

    Logging::trace(log_tag, "trace", 1);
    Logging::debug(log_tag, "debug", 2);
    Logging::info(log_tag, "info", 3);
    Logging::warn(log_tag, "warn", 4);
    Logging::error(log_tag, "error", 5);
    Logging::fatal(log_tag, "fatal", 6);

    Logging::clearAll();
}

TEST(LoggingTest, AddStdCoutStdErrLoggerForTypeInstanceRegistersBothSinks) {
    int value = 0;
    LogTag log_tag =
            vulkan_graphix::addStdCoutStdErrLoggerForTypeInstance(value);

    Logging::info(log_tag, "registered");

    Logging::clearAll();
}
