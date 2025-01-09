// Tank Game (@kennedyengineering)

#pragma once

#include <map>
#include <set>
#include <stdexcept>

namespace TankGame {
using RegistryId = unsigned int;

template <typename T> class Registry {
public:
  Registry() : mNextId(0) {}
  ~Registry() = default;

  template <typename... Args> RegistryId emplace(Args &&...args) {
    RegistryId id = getNextId();

    mObjectMap.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                       std::forward_as_tuple(std::forward<Args>(args)...));

    return id;
  }

  template <typename... Args> RegistryId emplaceWithId(Args &&...args) {
    RegistryId id = getNextId();

    mObjectMap.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                       std::forward_as_tuple(id, std::forward<Args>(args)...));

    return id;
  }

  T &get(RegistryId id) {
    auto it = mObjectMap.find(id);
    if (it == mObjectMap.end()) {
      throw std::out_of_range("Invalid RegistryId");
    }

    return it->second;
  }

  void remove(RegistryId id) {
    auto count = mObjectMap.erase(id);
    if (count == 0) {
      throw std::out_of_range("Invalid RegistryId");
    }

    mFreeIds.insert(id);
  }

private:
  RegistryId getNextId() {
    RegistryId id;
    if (mFreeIds.empty()) {
      id = mNextId++;
    } else {
      id = *mFreeIds.begin();
      mFreeIds.erase(mFreeIds.begin());
    }

    return id;
  }

private:
  std::map<RegistryId, T> mObjectMap;
  RegistryId mNextId;
  std::set<RegistryId> mFreeIds;
};
} // namespace TankGame
