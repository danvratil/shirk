#pragma once

namespace Core {

class Config;

class TeamsModel
{
public:
    TeamsModel(Config &config);


private:
    Config &mConfig;

};

} // namespace
