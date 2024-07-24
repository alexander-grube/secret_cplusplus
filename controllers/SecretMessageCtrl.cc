#include "SecretMessageCtrl.h"

// Add definition of your processing function here
void SecretMessageCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, std::string p1)
{
    auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr &)>>(move(callback));
    auto db = drogon::app().getFastDbClient();

    *db << "Select * from secret_message where id = $1" << p1 >> [callbackPtr](const drogon::orm::Result &r)
    {
        if (r.size() == 0)
        {
            auto resp = HttpResponse::newHttpResponse(HttpStatusCode::k404NotFound, ContentType::CT_TEXT_PLAIN);
            resp->setBody("Message not found");
            (*callbackPtr)(resp);
            return;
        }
        auto secretMessage = std::make_shared<SecretMessage>(r[0]);
        auto resp = HttpResponse::newHttpResponse(HttpStatusCode::k200OK, ContentType::CT_APPLICATION_JSON);
        resp->setBody(secretMessage->toJson().toStyledString());
        (*callbackPtr)(resp);
    } >> [callbackPtr](const drogon::orm::DrogonDbException &e)
    {
        auto resp = HttpResponse::newHttpResponse(HttpStatusCode::k500InternalServerError, ContentType::CT_TEXT_PLAIN);
        resp->setBody("An error occurred while retrieving the message");
        (*callbackPtr)(resp);
    };
}

void SecretMessageCtrl::post(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr &)>>(move(callback));
    auto json = req->getJsonObject();
    auto secretMessage = std::make_shared<SecretMessage>(*json);
    if (secretMessage->getMessage() == nullptr)
    {
        auto resp = HttpResponse::newHttpResponse(HttpStatusCode::k400BadRequest, ContentType::CT_TEXT_PLAIN);
        resp->setBody("Message not found in request");
        (*callbackPtr)(resp);
        return;
    }
    auto db = drogon::app().getFastDbClient();
    
    auto id = std::make_shared<std::string>();
    *db << "Insert into secret_message (message) values ($1) returning id" << secretMessage->getValueOfMessage() >> [callbackPtr, id, secretMessage](const drogon::orm::Result &r)
    {
        auto resp = HttpResponse::newHttpResponse(HttpStatusCode::k200OK, ContentType::CT_APPLICATION_JSON);
        secretMessage->setId(r[0]["id"].as<std::string>());
        resp->setBody(secretMessage->toJson().toStyledString());
        (*callbackPtr)(resp);
    } >> [callbackPtr](const drogon::orm::DrogonDbException &e)
    {
        LOG_ERROR << e.base().what();
        auto resp = HttpResponse::newHttpResponse(HttpStatusCode::k500InternalServerError, ContentType::CT_TEXT_PLAIN);
        resp->setBody("An error occurred while saving the message");
        (*callbackPtr)(resp);
    };
}