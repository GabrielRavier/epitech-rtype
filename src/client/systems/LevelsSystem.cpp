/*
** EPITECH PROJECT, 2022
** B-CPP-500-STG-5-1-rtype-gabriel.ravier
** File description:
** LevelsSystem
*/

#include "LevelsSystem.hpp"

extern Coordinator gCoordinator;

void LevelsSystem::Init()
{
    std::vector<std::string> filenames;

    auto assetsLevelsContents = std::filesystem::directory_iterator("./assets/levels");
    std::transform(begin(assetsLevelsContents), end(assetsLevelsContents), std::back_inserter(filenames),
                   [](auto &i) { return i.path().filename().string(); });
    for (const auto &filename : filenames)
        std::sort(filenames.begin(), filenames.end());
    for (const auto &filename : filenames)
        this->createLevel(filename);
}

SCENE LevelsSystem::Update(sf::Vector2i mousePosition, bool clicked, std::string *pathLevel)
{
    if (!clicked)
        return (SCENE::LEVELSMENU);
    for (auto const &entity : mEntities) {
        auto const &transform = gCoordinator.GetComponent<Transform>(entity);
        auto const &sprite    = gCoordinator.GetComponent<Sprite>(entity);
        auto const &level     = gCoordinator.GetComponent<Level>(entity);

        if (mousePosition.x >= transform.position.x &&
            mousePosition.x <= (transform.position.x + sprite.rectSize.x * transform.scale.x) &&
            mousePosition.y >= transform.position.y &&
            mousePosition.y <= (transform.position.y + sprite.rectSize.y * transform.scale.y)) {
            *pathLevel = level.path;
            return (SCENE::SOLO);
        }
    }
    return (SCENE::LEVELSMENU);
}

void LevelsSystem::createLevel(const std::string &name)
{
    if (name.length() <= 6 || name.compare(name.length() - 6, 6, ".level") != 0)
        return;
    auto level = gCoordinator.CreateEntity();
    gCoordinator.AddComponent(level, Level{"./assets/levels/" + name});
    const std::shared_ptr<sf::Texture> mtexture = std::make_shared<sf::Texture>();
    const std::shared_ptr<sf::Sprite>  msprite  = std::make_shared<sf::Sprite>();
    mtexture->loadFromFile("./assets/play_button.png");
    msprite->setTexture(*mtexture, false);
    msprite->setScale(sf::Vector2f(2, 2));
    gCoordinator.AddComponent(level, Sprite{mtexture, msprite, sf::Vector2i(39, 39), sf::Vector2i(0, 0), 1});
    gCoordinator.AddComponent(
        level, Transform{sf::Vector2f{float(200 * (_nbOfLevel % 6 + 1)), float(200 * (int(_nbOfLevel / 6) + 1))},
                         sf::Vector2f{2, 2}, 0});

    auto                            title = gCoordinator.CreateEntity();
    const std::shared_ptr<sf::Text> text  = std::make_shared<sf::Text>();
    const std::shared_ptr<sf::Font> font  = std::make_shared<sf::Font>();
    font->loadFromFile("./assets/font/font.ttf");
    text->setFont(*font);
    text->setString(name.substr(0, name.size() - 6));
    if (isLevelFinished("./assets/levels/" + name))
        text->setFillColor(sf::Color::Green);
    else
        text->setFillColor(sf::Color::White);
    text->setCharacterSize(10);
    gCoordinator.AddComponent(
        title, Transform{sf::Vector2f{float(200 * (_nbOfLevel % 6 + 1)), float(200 * (int(_nbOfLevel / 6) + 1) + 85)},
                         sf::Vector2f{1, 1}, 0});
    gCoordinator.AddComponent(title, Text{text, font});
    _nbOfLevel += 1;
}

bool LevelsSystem::isLevelFinished(const std::string &path)
{
    std::ifstream file;
    std::string   line;
    int           i = 0;

    file.open(path);
    while (file) {
        std::getline(file, line);
        if (i == 1) {
            if (line == "1")
                return (true);
            return (false);
        }
        i += 1;
    }
    return (false);
}
