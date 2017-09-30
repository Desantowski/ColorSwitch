#pragma once
#include "Interfaces.h"

template<class T>
class MenuState : public IState<T>
{
	sf::CircleShape x;
public:
	MenuState(Game & baseGame, GameCallback processFunction) : IState(baseGame,processFunction) 
	{
		m_drawables.push_back(std::make_shared<sf::CircleShape>(x));
		// no i w konstruktorze generalnie dodajemy wszystkie obiekty, kt�re maj� si� wy�wietla� (teksty, sprite'y, kszta�ty)
		// tak jak w 11, ewentualnie m_drawables.push_back(x); je�eli x jest shared_ptr'em
		// tyle je�li chodzi o grafik�
	}

	virtual void handleInput(sf::Event & event) override
	{
		if (event.KeyPressed && event.key.code == sf::Keyboard::Return) 
			m_postProcess(m_base,GameStart);
	};

	inline virtual void processLogic(sf::Time deltaTime) override 
	{ 
		x.move(-1.f, -2.f);
		// w logice mo�na se robi� co si� chce i na czym si� chce, rysowanie jest niezale�ne i interface-provided
	};
};