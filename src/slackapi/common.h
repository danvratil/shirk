#pragma once

namespace Shirk::SlackAPI {

class UnauthenticatedRequest {};
class UserAuthenticatedRequest {};
class BotAuthenticatedRequest {};
class Response {};

enum class Method {
    GET,
    POST,
    PUT,
    DELETE
};


} // namespace
