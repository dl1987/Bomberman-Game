#pragma once

#include "GameStage.hpp"
#include "GameStatus.hpp"
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"

enum class Option
{
	PlayAgain,
	ExitGame
};

class ExitStage : public GameStage
{
public:
	ExitStage(const GameStatus);
	bool update(const entityx::TimeDelta) override;
	void draw(sf::RenderWindow&) override;
	void handleEvent(sf::Event&) override;

private:
	void resizeWindow() const;
	void loadResources();
	void initSprites();
	void initTexts();
	void initLayout();

	void repaintOptions();
	void changeStage() const;

	ResourceHolder<sf::Texture, ResourceID> textures;
	ResourceHolder<sf::Font, ResourceID> fonts;
	sf::Sprite background;
	sf::Sprite bomberman;
	sf::Sprite creep;
	sf::Text bombermanStatus;
	sf::Text creepStatus;
	sf::Text playAgain;
	sf::Text exitGame;
	Option selectedOption;
};

