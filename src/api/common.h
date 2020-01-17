#pragma once

namespace Core::API {

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
