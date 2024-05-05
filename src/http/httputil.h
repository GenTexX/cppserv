#ifndef __HTTPUTIL_H__
#define __HTTPUTIL_H__

namespace cppserv {

    enum class HttpMethod {
        OPTIONS,
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        TRACE,
        CONNECT,
        PATCH,
        LINK,
        UNLINK,
        NOT_IMPLEMENTED
    };

    enum class HttpVersion {
        HTTP_1_0,
        HTTP_1_1,
        HTTP_2_0,
        HTTP_3_0,
        NOT_IMPLEMENTED
    };

} // namespace cppserv


#endif // __HTTPUTIL_H__