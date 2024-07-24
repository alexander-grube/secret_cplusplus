#pragma once

#include <drogon/HttpController.h>

#include "SecretMessage.h"
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::secret;

class SecretMessageCtrl : public drogon::HttpController<SecretMessageCtrl>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    ADD_METHOD_TO(SecretMessageCtrl::get, "/drogon/secret/{1}", Get);
    ADD_METHOD_TO(SecretMessageCtrl::post, "/drogon/secret", Post);
    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, std::string p1);
    void post(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
};
