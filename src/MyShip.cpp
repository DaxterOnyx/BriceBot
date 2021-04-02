#include "MyShip.hpp"

namespace BriceBot
{
	MyShip::MyShip()
	{
		DefineDestination();
	}

	hlt::Direction MyShip::Move(hlt::Position p_position, int p_halite, int p_mapHalite)
	{
		if (p_position == m_destination)
		{
			//Define destination
			if (p_halite >= 800)
			{
				DefineDestination();
			}
		}

		if (p_position.x < m_destination.x)
		{
			return hlt::Direction::EAST;
		}
		if (p_position.x > m_destination.x)
		{
			return hlt::Direction::WEST;
		}
		if (p_position.y < m_destination.y)
		{
			return hlt::Direction::NORTH;
		}
		if (p_position.y > m_destination.y)
		{
			return hlt::Direction::SOUTH;
		}
		return hlt::Direction::STILL;
	}

	void MyShip::DefineDestination()
	{
		m_destination.x =
			0;
		//rand() % 64;
		m_destination.y =
			0;
		//rand() % 64;
	}
}