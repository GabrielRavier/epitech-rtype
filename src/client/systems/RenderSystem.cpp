/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** RenderSystem
*/

#include "RenderSystem.hpp"
#include "ObjectsSystem.hpp"

extern Coordinator                    gCoordinator;
extern std::shared_ptr<ObjectsSystem> gObjectsSystem;

void RenderSystem::Init() {}

void RenderSystem::Update(const std::shared_ptr<WindowManager> &windowManager, bool online) const
{
    windowManager->Clear();
    std::vector<Entity> layer_one;
    for (auto const &entity : mEntities) {

        // Hide player if not alive.
        if (online) {
            if (entity == gObjectsSystem->GetMe() && gCoordinator.GetComponent<Player>(entity).life == 0)
                continue;
        }

        auto const &sprite = gCoordinator.GetComponent<Sprite>(entity);
        if (sprite.layer == 0) {
            auto const &transform = gCoordinator.GetComponent<Transform>(entity);
            windowManager->RenderSprite(sprite.sprite, transform.position);
        } else
            layer_one.push_back(entity);
    }
    for (auto const &entity : layer_one) {
        auto const &sprite    = gCoordinator.GetComponent<Sprite>(entity);
        auto const &transform = gCoordinator.GetComponent<Transform>(entity);
        windowManager->RenderSprite(sprite.sprite, transform.position);
    }
}
