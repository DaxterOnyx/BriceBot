#include "MyShip.hpp"
#include "../hlt/log.hpp"
#include <random>

namespace BriceBot
{
	MyShip::MyShip() { hlt::log::log("ERROR"); }

	MyShip::MyShip(hlt::Game* p_game, int p_id)
	{
		m_id = p_id;
		m_game = p_game;
		m_destination = NewDestination(m_game->me->shipyard->position);
	}

	hlt::Direction MyShip::Move()
	{
		auto l_ship = m_game->me->ships[m_id];
		auto l_pos = l_ship->position;
		auto l_halite = l_ship->halite;
		if (l_pos.x == m_destination.x && l_pos.y == m_destination.y)
		{
			//hlt::log::log("Ship ID " + std::to_string(m_id) + " Pos x=" + std::to_string(l_pos.x) + " y=" + std::to_string(l_pos.y) + " dist=" + std::to_string(m_game->game_map->calculate_distance(l_pos, m_destination)));
			hlt::log::log(" Pos x=" + std::to_string(m_destination.x) + " y=" + std::to_string(m_destination.y));
			//Define destination
			if (l_halite > 0)
			{
				if (m_game->game_map->at(l_pos)->halite <= 100)
					m_destination = GoBase(l_pos);
			}
			else
			{
				m_destination = NewDestination(l_pos);
			}
			hlt::log::log(" Pos x=" + std::to_string(m_destination.x) + " y=" + std::to_string(m_destination.y));
		}
		return m_game->game_map->naive_navigate(m_game->me->ships[m_id], m_destination);
	}

	hlt::Position MyShip::NewDestination(hlt::Position p_position)
	{
		hlt::Position l_destination;
		//GLOBAL RANDOM
		//*
		l_destination = hlt::Position(rand() % 64, rand() % 64);
		//*/

		//Next RANDOM
		/*
		hlt::Direction dir = hlt::Direction(rand() % 4);
		hlt::Position mov(0, 0);
		switch (dir) {
		case hlt::Direction::NORTH:
			mov = hlt::Position(0, 1);
			break;
		case hlt::Direction::SOUTH:
			mov = hlt::Position(0, -1);
			break;
		case hlt::Direction::EAST:
			mov = hlt::Position(0, 1);
			break;
		case hlt::Direction::WEST:
			mov = hlt::Position(0, -1);
			break;
		}
		l_destination = hlt::Position(p_position.x + mov.x, p_position.y + mov.y);
		//*/

		//Next with more halite
		/*
		//North
		hlt::Position l_northPos(p_position.x, p_position.y + 1);
		auto l_northCell = m_game->game_map->at(l_northPos);
		int l_northCount = !l_northCell->is_occupied() ? l_northCell->halite : -1;
		//SOUTH
		hlt::Position l_southPos(p_position.x, p_position.y - 1);
		auto l_southCell = m_game->game_map->at(l_southPos);
		int l_southCount = !l_southCell->is_occupied() ? l_southCell->halite : -1;
		//WEST
		hlt::Position l_westPos(p_position.x + 1, p_position.y);
		auto l_westCell = m_game->game_map->at(l_westPos);
		int l_westCount = !l_westCell->is_occupied() ? l_westCell->halite : -1;
		//EAST
		hlt::Position l_eastPos(p_position.x - 1, p_position.y);
		auto l_eastCell = m_game->game_map->at(l_eastPos);
		int l_eastCount = !l_eastCell->is_occupied() ? l_eastCell->halite : -1;


		hlt::Position l_move(0, 0);
		if (l_eastCount > l_westCount && l_eastCount > l_northCount && l_eastCount > l_southCount)
		{
			l_move = l_eastPos;
		}
		else if (l_westCount > l_northCount && l_westCount > l_southCount)
		{
			l_move = l_westPos;
		}
		else if (l_northCount > l_southCount)
		{
			l_move = l_northPos;
		}
		else
		{
			l_destination = l_southPos;
		}
		l_destination = hlt::Position(p_position.x + l_move.x, p_position.y + l_move.y);
		//*/

		hlt::log::log("Ship ID " + std::to_string(m_id) + " x=" + std::to_string(l_destination.x) + " y=" + std::to_string(l_destination.y));
		return l_destination;
	}

	hlt::Position MyShip::GoBase(hlt::Position p_position)
	{
		auto l_destination = hlt::Position(m_game->me->shipyard->position);
		int l_dist = m_game->game_map->calculate_distance(p_position, l_destination);
		hlt::log::log("Ship ID " + std::to_string(m_id) + " x=" + std::to_string(l_destination.x) + " y=" + std::to_string(l_destination.y) + " dist=" + std::to_string(m_game->game_map->calculate_distance(p_position, l_destination)));

		for (const auto& l_dropoffs_iterator : m_game->me->dropoffs)
		{
			auto l_dropPos = l_dropoffs_iterator.second->position;
			auto l_tempDist = m_game->game_map->calculate_distance(p_position, l_dropPos);
			hlt::log::log(std::to_string(l_tempDist) + "<" + std::to_string(l_dist));
			if (l_tempDist < l_dist)
			{
				l_destination = hlt::Position(l_dropPos);
				l_dist = l_tempDist;
			}
		}

		return l_destination;
	}
}
