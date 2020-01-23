#pragma once

namespace Shirk::Core
{

class Config;
class NetworkDispatcher;
class UserManager;

struct Environment
{
    Config &config;
    NetworkDispatcher &networkDispatcher;
    UserManager &userManager;
};

}


