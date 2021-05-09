#pragma once

#include "../hlt/direction.hpp"
#include "../hlt/position.hpp"
#include "../hlt/game.hpp"

namespace BriceBot
{
	class MyShip
	{
		int m_id;
		hlt::Position m_destination;
		hlt::Game* m_game;
	public:
		MyShip();
		MyShip(hlt::Game* p_game, int p_id);
		hlt::Direction Move(std::vector<std::vector<float>> interest, std::vector<hlt::Position>* destinations);
		hlt::Position NewDestination(hlt::Position p_position, std::vector<std::vector<float>> interest, std::vector<hlt::Position>* destinations);
		hlt::Direction MoveAny();
	private:
		hlt::Position GoBase(hlt::Position p_position);
		hlt::Direction Navigate(hlt::Position p_position, hlt::Position p_destination);
	};
}

