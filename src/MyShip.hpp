#pragma once

#include "../hlt/direction.hpp"
#include "../hlt/position.hpp"

namespace BriceBot
{
	class MyShip
	{
		hlt::Position m_destination;

	public:
		MyShip();
		hlt::Direction Move(hlt::Position p_position,int p_halite, int p_mapHalite);
	private:
		void DefineDestination();
	};
}

