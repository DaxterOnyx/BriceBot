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
		hlt::log::log("PGIGKJKJHL");
		m_destination = hlt::Position(p_game->me->ships[p_id]->position);
	}

	hlt::Direction MyShip::Move()
	{
		auto l_log = "Ship" + std::to_string(m_id);
		auto l_ship = m_game->me->ships[m_id];
		auto l_pos = l_ship->position;
		auto l_halite = l_ship->halite;
		if (l_pos == m_destination)
		{
			//TODO m_dest is not modify !!!!!!!!!!!!!!!!!!!
			//Define destination
			if (l_halite > 100)
			{
				if (m_game->game_map->at(l_pos)->halite <= 100)
				{
					GoBase(l_pos, m_destination);
					l_log += " go base";
				}
			}
			else
			{
				NewDestination(l_pos, m_destination);
				l_log += " new dest: x= " + std::to_string(m_destination.x) + " y=" + std::to_string(m_destination.y);
			}
		}

		hlt::Direction l_dir;
		/*
		l_dir = m_game->game_map->naive_navigate(m_game->me->ships[m_id], m_destination);
		//*/
		//*
		l_dir = Navigate(l_pos, m_destination);
		//*/
		hlt::log::log(l_log + " go to " + (char)l_dir + " with " + std::to_string(l_halite));
		return l_dir;
	}

	void MyShip::NewDestination(hlt::Position p_position, hlt::Position& p_destination)
	{

		//GLOBAL RANDOM
		//*
		m_destination.x = rand() % m_game->game_map->width;
		m_destination.y = rand() % m_game->game_map->height;

		//*/

		//NEXT RANDOM BIS
		/*
		hlt::Position l_pos = m_game->me->ships[m_id]->position;
		l_destination = hlt::Position{l_pos.x + (rand() % 3) - 1, l_pos.y + (rand() % 3) - 1};
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
		l_destination = hlt::Position{p_position.x + mov.x, p_position.y + mov.y};
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
		l_destination = hlt::Position{p_position.x + l_move.x, p_position.y + l_move.y};
		//*/
	}

	void MyShip::GoBase(hlt::Position p_position, hlt::Position& p_destination)
	{
		p_destination = hlt::Position(m_game->me->shipyard->position);
		int l_dist = m_game->game_map->calculate_distance(p_position, p_destination);
		hlt::log::log("Ship ID " + std::to_string(m_id) + " x=" + std::to_string(p_destination.x) + " y=" + std::to_string(p_destination.y) + " dist=" + std::to_string(m_game->game_map->calculate_distance(p_position, p_destination)));

		for (const auto& l_dropoffs_iterator : m_game->me->dropoffs)
		{
			auto l_dropPos = l_dropoffs_iterator.second->position;
			auto l_tempDist = m_game->game_map->calculate_distance(p_position, l_dropPos);
			hlt::log::log(std::to_string(l_tempDist) + "<" + std::to_string(l_dist));
			if (l_tempDist < l_dist)
			{
				p_destination = hlt::Position(l_dropPos);
				l_dist = l_tempDist;
			}
		}
	}

	hlt::Direction MyShip::Navigate(hlt::Position p_pos, hlt::Position p_dest)
	{
		auto l_direction = hlt::Direction::STILL;
		auto l_map = m_game->game_map.get();
		if (p_pos == p_dest)
			return l_direction;

		auto l_cell = l_map->at(p_pos);
		int l_cost = l_cell->halite * 1 / hlt::constants::MOVE_COST_RATIO;
		auto l_ship = m_game->me->ships[m_id];

		if (l_cell->has_structure() || l_cost <= l_ship->halite)
		{
			//MOVE
			//CALCULATE DIRECTION OF DESTINATION
			auto l_width = l_map->width;
			auto l_distE = (p_pos.x - p_dest.x) + l_width;
			auto l_distW = (p_dest.x - p_pos.x) + l_width;
			auto l_dirH = l_direction;
			auto l_cellH = l_cell;
			if (l_distW < l_distE)
			{
				l_dirH = hlt::Direction::WEST;
			}
			else if (l_distW > l_distE)
			{
				l_dirH = hlt::Direction::EAST;
			}
			l_cellH = l_map->at(p_pos.directional_offset(l_dirH));

			auto l_height = l_map->height;
			auto l_distN = (p_dest.y - p_pos.y) + l_height;
			auto l_distS = (p_pos.y - p_dest.y) + l_height;
			auto l_dirV = l_direction;
			auto l_cellV = l_cell;
			if (l_distN < l_distS)
			{
				l_dirV = hlt::Direction::NORTH;
			}
			else if (l_distN > l_distS)
			{
				l_dirV = hlt::Direction::SOUTH;
			}
			l_cellV = l_map->at(p_pos.directional_offset(l_dirV));

			//DECIDE BETTER DIRECTION 
			if (l_ship->halite < 100)

			{
				//I MUST RELOAD
				//I GO TO THE BETTER CELL TO RELOAD
				if (l_cellH->halite > l_cellV->halite && l_dirH != hlt::Direction::STILL || l_dirV == hlt::Direction::STILL)
					l_direction = l_dirH;
				else
					l_direction = l_dirV;
			}
			else
			{
				//I CAN MOVE
				//I GO TO THE LESSER CELL TO COST LESS
				if (l_cellH->halite < l_cellV->halite && l_dirH != hlt::Direction::STILL || l_dirV == hlt::Direction::STILL)
					l_direction = l_dirH;
				else
					l_direction = l_dirV;
			}

			hlt::log::log("navigate at " + std::to_string(p_pos.x) + ";" + std::to_string(p_pos.y) + " to " + std::to_string(p_dest.x) + ";" + std::to_string(p_dest.y) + " by " + (char)l_direction);
			hlt::log::log("distN " + std::to_string(l_distN) + " distS " + std::to_string(l_distS) + " distE " + std::to_string(l_distE) + " distW " + std::to_string(l_distW));
		}
		return l_direction;
	}
}
