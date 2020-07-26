#pragma once

namespace rl {

template<typename T>
T gcd(T a, T b)
{
    return a%b?gcd(b,a%b):b;
}

template<typename T>
T lcm(T a, T b)
{
    return a * b / gcd(a, b);
}

} // end of namespace rl
