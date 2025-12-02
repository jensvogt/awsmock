

#include <awsmock/core/RandomUtils.h>

namespace AwsMock::Core {

    template<typename T = std::mt19937>
    auto RandomGenerator() -> T {
        auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
        auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
        auto seed = std::array<std::seed_seq::result_type, seed_len>();
        auto dev = std::random_device();
        std::generate_n(begin(seed), seed_len, std::ref(dev));
        auto seed_seq = std::seed_seq(begin(seed), end(seed));
        return T{seed_seq};
    }

    int RandomUtils::NextInt(const int min, const int max) {
        thread_local auto rng = RandomGenerator<>();
        auto dist = std::uniform_int_distribution{min, max};
        return dist(rng);
    }

    unsigned int RandomUtils::NextUInt(const unsigned int min, const unsigned int max) {
        thread_local auto rng = RandomGenerator<>();
        auto dist = std::uniform_int_distribution{min, max};
        return dist(rng);
    }

    std::vector<int> RandomUtils::IntArray(const int size, const int min, const int max) {
        std::vector<int> result;
        result.reserve(size);
        for (int i = 0; i < size; i++) {
            result.push_back(NextInt(min, max));
        }
        return result;
    }

    double RandomUtils::NextDouble(const double min, const double max) {
        thread_local auto rng = RandomGenerator<>();
        std::uniform_real_distribution<double> dist(min, max);
        return dist(rng) * (1.0 / std::numeric_limits<std::uint32_t>::max());
    }

    std::vector<double> RandomUtils::DoubleArray(const int size, const double min, const double max) {
        std::vector<double> result;
        result.reserve(size);
        for (int i = 0; i < size; i++) {
            result.push_back(NextDouble(min, max));
        }
        return result;
    }
}// namespace AwsMock::Core