#pragma once

#include <array>
#include <cassert>
#include <queue>
#include "Types.hpp"

/**
 * @brief Manager that manages available Entity IDs and signatures.
 */
class EntityManager
{
public:

    /**
     * @brief Construct a new Entity Manager.
     */
    EntityManager()
    {
        for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
            mAvailableEntities.push(entity);
        }
    }

    /**
     * @brief Get an available Entity ID.
     *
     * @return Entity ID
     */
    Entity CreateEntity()
    {
        assert((mLivingEntityCount < MAX_ENTITIES) && "Too many entities in existence.");

        Entity id = mAvailableEntities.front();

        mAvailableEntities.pop();
        mLivingEntityCount++;
        return id;
    }

    /**
     * @brief Release an Entity ID.
     *
     * @param entity Entity ID
     */
    void DestroyEntity(Entity entity)
    {
        assert((entity < MAX_ENTITIES) && "Entity out of range.");

        mSignatures[entity].reset();
        mAvailableEntities.push(entity);
        mLivingEntityCount--;
    }

    /**
     * @brief Set the Signature of the Entity.
     *
     * @param entity        Entity ID
     * @param signature     Entity Signature
     */
    void SetSignature(Entity entity, Signature signature)
    {
        assert((entity < MAX_ENTITIES) && "Entity out of range.");

        mSignatures[entity] = signature;
    }

    /**
     * @brief Get the Signature of the Entity.
     *
     * @param   entity      Entity ID
     * @return  Entity Signature
     */
    Signature GetSignature(Entity entity)
    {
        assert((entity < MAX_ENTITIES) && "Entity out of range.");

        return mSignatures[entity];
    }

private:
    uint32_t                                mLivingEntityCount{};
    std::queue<Entity>                      mAvailableEntities{};
    std::array<Signature, MAX_ENTITIES>     mSignatures{};
};
