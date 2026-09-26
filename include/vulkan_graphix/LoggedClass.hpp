#ifndef VULKAN_GRAPHIX_LOGGEDCLASS_HPP
#define VULKAN_GRAPHIX_LOGGEDCLASS_HPP

#include "vulkan_graphix/Logging.h"

namespace vulkan_graphix {
/**
 * @brief A class that sets up stderror and stdout loggers for a derived
 *        class.
 *
 * If you inherit from this class you will get a logger added to logging
 * for specific instance of this class.
 *
 * @tparam DerivedType Used in RTTI for an instance of \tref DerivedType.
 */
template <typename DerivedType> class LoggedClass {
public:
    /**
     * @brief ctor
     *
     * @param[in] d An instance of \tref DerivedType used to generate a
     *              \ref LogTag.
     * @param[in] cout_level The level at which to log to std::cout.
     * @param[in] cerr_level The level at which to log to std::cerr.
     */
    explicit LoggedClass(const DerivedType& derived,
                        SeverityLevel cout_level = VULKAN_GRAPHIX_INFO,
                        SeverityLevel cerr_level = VULKAN_GRAPHIX_ERROR)
            : LOG_TAG(Logging::logTagForThis(derived)) {
        Logging::addStdCoutLogger(LOG_TAG, cout_level);
        Logging::addStdCerrLogger(LOG_TAG, cerr_level);
    }

    LoggedClass(const LoggedClass& other) = delete;
    LoggedClass& operator=(const LoggedClass& rhs) = delete;
    LoggedClass(LoggedClass&& rhs) = delete;
    LoggedClass& operator=(LoggedClass&& rhs) = delete;

    /**
     * @brief dtor
     */
    virtual ~LoggedClass() = default;

protected:
    LogTag LOG_TAG;  // NOLINT(readability-identifier-naming)
};
}  // namespace vulkan_graphix
#endif
