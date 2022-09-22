#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include "System.hpp"
#include "Types.hpp"

/**
 * @brief Manager that manages Systems.
 */
class SystemManager
{
public:

    /**
     * @brief Register a new system in the manager.
     *
     * @tparam T                System
     * @return New System
     */
    template <typename T>
    std::shared_ptr<T> RegisterSystem()
    {
        assert((mSystems.find(typeid(T)) == mSystems.end()) && "Registering system more than once.");

        auto system = std::make_shared<T>();
        mSystems.insert({ typeid(T), system });
        return system;
    }

    /**
     * @brief Set required Signature to use the System.
     *
     * @tparam T                System
     * @param signature         Required Signature
     */
    template <typename T>
    void SetSignature(Signature signature)
    {
        assert((mSystems.find(typeid(T)) != mSystems.end()) && "System used before registered.");

        mSignatures.insert({ typeid(T), signature });
    }

    /**
     * @brief Remove the Entity from Systems that can manage it.
     *
     * @param entity            Entity ID
     */
    void EntityDestroyed(Entity entity)
    {
        for (auto const &pair : mSystems) {
            (pair.second)->mEntities.erase(entity);
        }
    }

    /**
     * @brief Sets the Systems that can manage the Entity with respect to its new signature.
     *
     * @param entity            Entity ID
     * @param entitySignature   New Entity Signature
     */
    void EntitySignatureChanged(Entity entity, Signature entitySignature)
    {
        for (auto const &pair : mSystems) {
            auto const &systemSignature = mSignatures[pair.first];

            if ((systemSignature & entitySignature) == systemSignature) {
                (pair.second)->mEntities.insert(entity);

            } else {
                (pair.second)->mEntities.erase(entity);
            }
        }
    }

private:
    std::unordered_map<std::type_index, Signature>                  mSignatures{};
    std::unordered_map<std::type_index, std::shared_ptr<System>>    mSystems{};
};
