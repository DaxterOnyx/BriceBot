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
			//Define destination
			if (l_halite > 0.1f * hlt::constants::MAX_HALITE)
			{
				if (m_game->game_map->at(l_pos)->halite <= 100 || l_halite > 0.9f * hlt::constants::MAX_HALITE)
				{
					m_destination = GoBase(l_pos);
					l_log += " go base";
				}
			}
			else
			{
				m_destination = NewDestination(l_pos);
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

		m_game->game_map->at(l_pos.directional_offset(l_dir))->mark_unsafe(l_ship);

		return l_dir;
	}

	hlt::Position MyShip::NewDestination(hlt::Position p_position)
	{
		auto l_destination = p_position;
		//GLOBAL RANDOM
		/*
		l_destination.x = rand() % m_game->game_map->width;
		l_destination.y = rand() % m_game->game_map->height;

		//*/

		//NEXT RANDOM BIS
		/*
		hlt::Position l_pos = m_game->me->ships[m_id]->position;
		l_destination = hlt::Position{l_pos.x + (rand() % 3) - 1, l_pos.y + (rand() % 3) - 1};
		//*/

		//Next RANDOM
		/*
		auto dir = hlt::Direction(rand() % 4);

		l_destination = p_position.directional_offset(dir);
		//*/

		//Next with more halite
		//*
		auto l_map = m_game->game_map.get();

		auto l_possibleDest = std::vector<hlt::MapCell*>();
		//Still
		//l_possibleDest.push_back(l_map->at(p_position));
		//North
		auto l_cellNorth = l_map->at(p_position.directional_offset(hlt::Direction::NORTH));
		if (l_cellNorth->is_empty())
			l_possibleDest.push_back(l_cellNorth);
		//SOUTH
		auto l_cellSouth = l_map->at(p_position.directional_offset(hlt::Direction::SOUTH));
		if (l_cellSouth->is_empty())
			l_possibleDest.push_back(l_cellSouth);
		//WEST
		auto l_cellWest = l_map->at(p_position.directional_offset(hlt::Direction::WEST));
		if (l_cellSouth->is_empty())
			l_possibleDest.push_back(l_cellWest);
		//EAST
		auto l_cellEast = l_map->at(p_position.directional_offset(hlt::Direction::SOUTH));
		if (l_cellSouth->is_empty())
			l_possibleDest.push_back(l_cellEast);

		auto l_destCell = l_map->at(l_destination);
		auto l_destCost = l_destCell->halite;
		for (auto l_cell : l_possibleDest)
		{
			auto l_tempCost = l_cell->halite;
			if (l_tempCost > l_destCost)
			{
				l_destCell = l_cell;
				l_destination = l_cell->position;
				l_destCost = l_tempCost;
			}
		}
		//*/

		return l_destination;
	}

	hlt::Position MyShip::GoBase(hlt::Position p_position)
	{
		auto l_destination = hlt::Position(m_game->me->shipyard->position);
		int l_dist = m_game->game_map->calculate_distance(p_position, l_destination);
		for (const auto& l_dropoffs_iterator : m_game->me->dropoffs)
		{
			auto l_dropPos = l_dropoffs_iterator.second->position;
			auto l_tempDist = m_game->game_map->calculate_distance(p_position, l_dropPos);
			if (l_tempDist < l_dist)
			{
				l_destination = hlt::Position(l_dropPos);
				l_dist = l_tempDist;
			}
		}
		return l_destination;
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
			auto l_posV = p_pos.directional_offset(l_dirV);
			l_cellV = l_map->at(l_posV);

			//DECIDE BETTER DIRECTION 
			if (l_ship->halite - l_cost < l_cellH->halite / hlt::constants::MOVE_COST_RATIO
				&& l_ship->halite - l_cost < l_cellV->halite / hlt::constants::MOVE_COST_RATIO)
			{
				//I MUST RELOAD
				//I GO TO THE BETTER CELL TO RELOAD
				if (l_cellH->halite > l_cellV->halite && l_dirH != hlt::Direction::STILL || l_dirV == hlt::Direction::STILL)
				{
					if (!l_cellH->is_occupied())
						l_direction = l_dirH;
				}
				else {
					if (!l_cellV->is_occupied())
						l_direction = l_dirV;
				}
			}
			else
			{
				//I CAN MOVE
				//I GO TO THE LESSER CELL TO COST LESS
				if (l_cellH->halite < l_cellV->halite && l_dirH != hlt::Direction::STILL || l_dirV == hlt::Direction::STILL)
				{
					if (!l_cellH->is_occupied())
						l_direction = l_dirH;
				}
				else {
					if (!l_cellV->is_occupied())
						l_direction = l_dirV;
				}
			}

			//hlt::log::log("navigate at " + std::to_string(p_pos.x) + ";" + std::to_string(p_pos.y) + " to " + std::to_string(p_dest.x) + ";" + std::to_string(p_dest.y) + " by " + (char)l_direction);
			//hlt::log::log("distN " + std::to_string(l_distN) + " distS " + std::to_string(l_distS) + " distE " + std::to_string(l_distE) + " distW " + std::to_string(l_distW));
		}

		//TODO WARNIG COLLisION WITH ALLIES 
		//TODO WARNING COLLISION WITH ENNEMIES
		return l_direction;
	}
}
