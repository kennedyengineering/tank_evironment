// Tank Game (@kennedyengineering)

#pragma once

#include <map>
#include <set>

namespace TankGame
{
    using RegistryId = unsigned int;

    template <typename T>
    class Registry
    {
    public:
        Registry() : mNextId(0) { }
        ~Registry() = default;

        template <typename... Args>
        RegistryId emplace(Args&&... args)
        {
            RegistryId id;
            if (mFreeIds.empty())
            {
                id = mNextId++;
            }
            else
            {
                id = *mFreeIds.begin();
                mFreeIds.erase(mFreeIds.begin());
            }

            mObjectMap.emplace(id, T(std::forward<Args>(args)...));
            return id;
        }

        void remove(RegistryId id)
        {
            mObjectMap.erase(id);
            mFreeIds.insert(id);
        }

    private:
        std::map<RegistryId, T> mObjectMap;
        RegistryId mNextId;
        std::set<RegistryId> mFreeIds;
    };
}
