#pragma once
#include "Interfaces.hpp"
#include "Player.hpp"

template<class GameScene>
class Obstacle : public sf::CircleShape
{
	using GameSceneFunction = std::function<void(GameScene &)>;

	GameScene & m_base;
	std::shared_ptr<sf::CircleShape> m_intern;
	std::shared_ptr<Player> m_player;
	GameSceneFunction m_playerDeath;

public:
	Obstacle(int positionIndex, std::shared_ptr<Player> gamePlayer, GameScene & baseObject) 
		: m_base{ baseObject }, m_player { gamePlayer }, m_intern{std::make_shared<sf::CircleShape>()},
		m_playerDeath{ &GameScene::onDeath }
	{
		m_intern->setFillColor(sf::Color::White);

		setRadius(75.f);
		m_intern->setRadius(65.f);

		setOrigin(getGlobalBounds().height / 2.f, getGlobalBounds().width / 2.f);
		m_intern->setOrigin(m_intern->getGlobalBounds().height / 2.f, m_intern->getGlobalBounds().width / 2.f);

		setPosition(400.f, -350.f * positionIndex);

		float pX = getGlobalBounds().left + getGlobalBounds().width / 2.f;
		float pY = getGlobalBounds().top + getGlobalBounds().height / 2.f;
		m_intern->setPosition(pX, pY);
	}

	std::shared_ptr<sf::CircleShape> getInternalDrawable() { return m_intern; }

	void rotate(sf::Time deltaTime)
	{
		sf::CircleShape::rotate(60.f * deltaTime.asSeconds());
		m_intern->rotate(60.f * deltaTime.asSeconds());
	}

	void move(sf::Time deltaTime)
	{
		setPosition(getPosition().x, getPosition().y + 60.f * deltaTime.asSeconds());
		m_intern->setPosition(m_intern->getPosition().x, m_intern->getPosition().y + 60.f * deltaTime.asSeconds());

		while (getPosition().y > 750.f) setPosition(getPosition().x, getPosition().y - 750.f);
		while (m_intern->getPosition().y > 750.f) m_intern->setPosition(m_intern->getPosition().x, m_intern->getPosition().y - 750.f);
	}
	
	bool checkCollision()
	{
		sf::FloatRect playerBounds{ m_player->getGlobalBounds() };
		playerBounds.height -= 7.5f;
		playerBounds.top += 7.5f;
		if (playerBounds.intersects(sf::CircleShape::getGlobalBounds()))
		{
			auto color = [&]
			{
				if (m_player->getPosition().y > getGlobalBounds().top + getGlobalBounds().width)
				{
					float rotation{ sf::CircleShape::getRotation() };
					if (static_cast<int>(rotation) % 90 < 10) return -1;
					else return static_cast<int>(rotation / 90.f);
				}
				else
				{
					float rotation{ sf::CircleShape::getRotation() };
					if (static_cast<int>(rotation) % 90 < 10) return -1;
					else return (static_cast<int>(rotation / 90.f) + 2)%4;
				}
			}();
			if (m_player->getColor() != color && playerBounds.intersects(m_intern->getGlobalBounds()) == false)
			{ 
				m_playerDeath(m_base);
				return true;
			}
		}
		return false;
	}
};

template<class GameScene>
class Map : public ILogicProcessor
{
	using GameSceneFunction = std::function<void(GameScene &)>;
	using PlayerFunction = std::function<void(Player&)>;

	GameScene & m_base;
	GameSceneFunction m_pointGainedCallback;
	PlayerFunction m_playerCollorChanged;

	std::shared_ptr<Player> m_player;
	std::vector<std::shared_ptr<sf::Drawable>> m_drawables;
	std::vector<std::shared_ptr<Obstacle<GameScene>>> m_obstacles;

	std::shared_ptr<sf::Texture> m_circleTexture;

public:
	Map(std::vector<std::shared_ptr<sf::Drawable>> & drawables, std::shared_ptr<Player> playerPointer, GameScene & gameController)
		: m_base{ gameController }, m_player{ playerPointer }, m_circleTexture{ new sf::Texture() },
		m_pointGainedCallback{ &GameScene::onPoint }, m_playerCollorChanged{ &Player::onColorChange }
	{
		if (!m_circleTexture->loadFromFile("assets/data001.png")) std::cerr << "cannot load assets/data001.png";
		for (int i = 0; i < 1; i++)
		{
			std::shared_ptr<Obstacle<GameScene>> obstacle{ new Obstacle<GameScene>(i, playerPointer, gameController) };

	 		obstacle->setTexture(&(*m_circleTexture), true);

			drawables.push_back(obstacle);
			drawables.push_back(obstacle->getInternalDrawable());

			m_obstacles.push_back(obstacle);
		}
	}

	void move(sf::Time deltaTime)
	{
		for (std::shared_ptr<Obstacle<GameScene>> obstacle : m_obstacles)
			obstacle->move(deltaTime);
	}

	virtual void processLogic(sf::Time deltaTime) override
	{
		for (std::shared_ptr<Obstacle<GameScene>> obstacle : m_obstacles)
		{
			obstacle->rotate(deltaTime);
			if (obstacle->checkCollision()) break;
		}
	}
};

