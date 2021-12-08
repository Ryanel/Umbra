#pragma once

template <typename T>
constexpr T const& max(T const& a, T const& b) {
    return a > b ? a : b;
}

template <typename T>
constexpr T const& min(T const& a, T const& b) {
    return a < b ? a : b;
}
