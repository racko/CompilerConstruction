#include "FunctionalEnvironment2.h"

namespace Functional {
std::ostream& operator<<(std::ostream& s, const Environment2& env_) {
    const auto& env = env_.get();
    s << "[";
    if (!env.empty()) {
        auto e = env.begin();
        s << e->first << ": " << *e->second;
        auto end = env.end();
        for (++e; e != end; ++e)
            s << ", " << e->first << ": " << *e->second;
    }
    return s << "]";
}
} // namespace Functional
