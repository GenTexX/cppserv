#ifndef __CPPSERVCORE_H__
#define __CPPSERVCORE_H__

#include <memory>

namespace cppserv {

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Ref<T> createRef(Args&& ... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

} // namespace cppserv


#endif // __CPPSERVCORE_H__