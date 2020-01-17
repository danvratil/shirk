#pragma once

namespace Shirk::Core
{

class Config;
class NetworkDispatcher;

struct Environment
{
    Config &config;
    NetworkDispatcher &networkDispatcher;
};

}


