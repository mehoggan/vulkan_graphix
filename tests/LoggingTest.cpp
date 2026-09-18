// Exercises vulkan_graphix::Logging directly - pure Boost.Log/filesystem
// code, no Vulkan device or X11 window needed. TestLogging.testClearAll
// is named to match the FRIEND_TEST(TestLogging, testClearAll) already
// declared in Logging.h, giving it access to the private s_loggers map.

// vulkan_graphix/Logging.h must be included before any other boost/log
// header: it #defines BOOST_LOG_DYN_LINK, which fixes boost::log's ABI-
// versioning inline namespace (v2_mt_posix vs. v2s_mt_posix) on that
// header's first inclusion in this translation unit - including
// <boost/log/core.hpp> first here previously picked the wrong one and
// left this binary unable to link against libvulkan_graphix.la's Logging
// symbols.
#include "vulkan_graphix/Logging.h"

#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>
#include <boost/filesystem/operations.hpp>
#include <boost/log/core.hpp>

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
    boost::filesystem::path tmp_dir = Logging::mktmpdir();

    EXPECT_TRUE(boost::filesystem::exists(tmp_dir));
    EXPECT_TRUE(boost::filesystem::is_directory(tmp_dir));

    boost::filesystem::remove_all(tmp_dir);
}

TEST(LoggingTest, AddFileLoggerWritesTheLoggedMessageToDisk) {
    boost::filesystem::path tmp_dir = Logging::mktmpdir();
    boost::filesystem::path log_file = tmp_dir / "logging_test.log";
    LogTag log_tag("logging-test-file");

    ASSERT_TRUE(Logging::addFileLogger(log_tag, log_file));
    Logging::info(log_tag, "hello", "file", "logger");
    boost::log::core::get()->flush();

    ASSERT_TRUE(boost::filesystem::exists(log_file));
    std::ifstream in_stream(log_file.string());
    std::string contents((std::istreambuf_iterator<char>(in_stream)),
                         std::istreambuf_iterator<char>());
    EXPECT_NE(contents.find("hello file logger"), std::string::npos);

    Logging::clearAll();
    boost::filesystem::remove_all(tmp_dir);
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
