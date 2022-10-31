/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** TextSystem
*/

#include "TextSystem.hpp"
#include "ObjectsSystem.hpp"

extern Coordinator                    gCoordinator;
extern std::shared_ptr<ObjectsSystem> gObjectsSystem;

void TextSystem::Init() {}

void TextSystem::Update(const std::shared_ptr<WindowManager> &windowManager) const
{
    for (auto const &entity : mEntities) {
        auto const &text = gCoordinator.GetComponent<Text>(entity);
        auto const &pos  = gCoordinator.GetComponent<Transform>(entity);
        windowManager->RenderText(text.text, pos.position);
    }
}
