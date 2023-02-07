#include "Interface/IGameLogic.hpp"

class GameLogic : public IGameLogic {
public:
    virtual bool Initialize() override { return true; }
    virtual void Finalize() override {}

    virtual void Tick() override {}
};