#ifndef __HTTPREQUEST_H__
#define __HTTPREQUEST_H__

#include <string>
#include <map>

#include "httputil.h"

namespace cppserv {

    class HttpRequest {
    public:
        HttpRequest() {}
        ~HttpRequest() {}

    private:
        HttpMethod m_Method;
        HttpVersion m_Version;
        std::string m_Path;
        std::map<std::string, std::string> m_Headers;
        std::string m_Body;
    };

} // namespace cppserv


#endif // __HTTPREQUEST_H__