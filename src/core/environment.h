#pragma once

namespace Core
{

class Config;
class NetworkDispatcher;

struct Environment
{
    Config &config;
    NetworkDispatcher &networkDispatcher;
};

}


