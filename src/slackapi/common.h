#pragma once

namespace Shirk::SlackAPI {

class UnauthenticatedRequest {};
class AuthenticatedRequest {};
class Response {};

enum class Method {
    GET,
    POST,
    PUT,
    DELETE
};


} // namespace
