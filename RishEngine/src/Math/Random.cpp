#include <Rish/Math/Random.h>

namespace rl {

int Math::RandomInt(int lower, int upper)
{
    if(lower > upper)
        std::swap(lower, upper);
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<int> dis(lower, upper);
    return dis(gen);
}

float Math::RandomFloat(float lower, float upper)
{
    if(lower > upper)
        std::swap(lower, upper);
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<float> dis(lower, upper);
    return dis(gen);
}

double Math::RandomDouble(double lower, double upper)
{
    if(lower > upper)
        std::swap(lower, upper);
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<double> dis(lower, upper);
    return dis(gen);
}

} // end of namespace rl