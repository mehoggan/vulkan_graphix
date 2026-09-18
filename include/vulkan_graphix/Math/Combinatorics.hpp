#ifndef VULKAN_GRAPHIX_COMBINATORICS_HPP
#define VULKAN_GRAPHIX_COMBINATORICS_HPP

#include <algorithm>
#include <cstddef>

namespace vulkan_graphix::Math {

// Advances `values` (an N-element indexable container, sorted ascending) to
// the next combination of its first select_count elements in lexicographic
// order. Returns false and resets `values` to its first combination once the
// sequence wraps.
template <typename CI, std::size_t N, typename T>
bool nextCombination(CI& values, T select_count) {
    if (select_count >= N || select_count == 0 || N == 1) {
        return false;
    }

    T index = select_count;
    T swap_index = static_cast<T>(N - 1);

    while (index != 0) {
        if (values[--index] < values[swap_index]) {
            T search_index = static_cast<T>(select_count);
            while (!(values[index] < values[search_index])) {
                ++search_index;
            }
            std::swap(values[index], values[search_index]);
            ++index;
            ++search_index;
            swap_index = select_count;
            // search_index can legitimately reach N here (one past the
            // last element), so it must be turned into a pointer via
            // arithmetic on &values[0] rather than &values[search_index]:
            // vector::operator[] on an out-of-range (== size()) index is
            // undefined behavior, silently tolerated at -O2 but caught by
            // libstdc++'s hardened assertions at -O0 (the -O0 coverage
            // build correctly caught this real bug).
            std::rotate(&values[index],
                        (&values[0] + search_index),
                        (&values[0] + N));
            while (search_index != N) {
                ++search_index;
                ++swap_index;
            }
            std::rotate(&values[select_count],
                        &values[swap_index],
                        (&values[0] + N));
            return true;
        }
    }
    std::rotate(&values[0], &values[select_count], (&values[0] + N));
    return false;
}

}  // namespace vulkan_graphix::Math

#endif
