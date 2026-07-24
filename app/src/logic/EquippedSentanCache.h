#pragma once
#include <Character/Player/Sentan/SentanDefinition.h>
#include <vector>

class EquippedSentanCache
{
public:
    static EquippedSentanCache* GetInstance()
    {
        static EquippedSentanCache instance;
        return &instance;
    }

    void Reset()
    {
        equippedSentans_.clear();
    }

    void RecordEquippedSentan(SentanId id)
    {
        equippedSentans_.push_back(id);
    }

    bool IsEquipped(SentanId id) const
    {
        return std::find(equippedSentans_.begin(), equippedSentans_.end(), id) != equippedSentans_.end();
    }

    bool IsEquippedAll() const
    {
        return equippedSentans_.size() == 5; // Assuming there are 5 SentanIds
    }

private:
    std::vector<SentanId> equippedSentans_;
};