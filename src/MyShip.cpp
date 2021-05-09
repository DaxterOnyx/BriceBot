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

	hlt::Direction MyShip::Move(std::vector<std::vector<float>> interest, std::vector<hlt::Position>* destinations)
	{
		auto l_log = "Ship" + std::to_string(m_id);
		auto l_ship = m_game->me->ships[m_id];
		auto l_pos = l_ship->position;
		auto l_halite = l_ship->halite;
		if (true)
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
				m_destination = NewDestination(l_pos, interest, destinations);
				l_log += " new dest: x= " + std::to_string(m_destination.x) + " y=" + std::to_string(m_destination.y);
			}
		}

		hlt::Direction l_dir;
		/*
		l_dir = m_game->game_map->naive_navigate(m_game->me->ships[m_id], m_destination);
		//*/
		//*
		l_dir = Navigate(l_pos, m_destination);
		if (m_destination != l_pos && l_dir == hlt::Direction::STILL) {
			hlt::Direction random_dir = MoveAny();
			bool exist = false;
			for (hlt::Position p : *destinations) {
				if (p == l_pos.directional_offset(l_dir)) {
					exist = true;
				}
			}
			if (!exist) {
				(*destinations).push_back(l_pos.directional_offset(l_dir));
				l_dir = random_dir;
			}
		}
		//*/
		hlt::log::log(l_log + " go to " + (char)l_dir + " with " + std::to_string(l_halite));

		m_game->game_map->at(l_pos.directional_offset(l_dir))->mark_unsafe(l_ship);

		return l_dir;
	}

	hlt::Position MyShip::NewDestination(hlt::Position p_position, std::vector<std::vector<float>> interest, std::vector<hlt::Position>* destinations)
	{
		auto l_destination = p_position;

		int destX, destY;
		float tmp_interest, destInterest = 0;
		int dist = 0, dist_base = 0;
		//go through the map of interests (here just the halite)
		//using the distance to the base (could use dropoff too is implemented) and from the ship to scale the interest of each case
		//the most interesting case for the ship is the new destination
		for (int i = 0; i < 64; i++) {
			for (int j = 0; j < 64; j++) {
				dist = std::abs(p_position.x - j) + std::abs(p_position.y - i);
				dist_base = std::abs(m_game->me->shipyard->position.x - j) + std::abs(m_game->me->shipyard->position.y - i);
				tmp_interest = std::powf(0.95f, dist) * interest.at(i).at(j) * std::powf(0.98f, dist_base);
				bool exist = false;
				for (hlt::Position p : *destinations) {
					if (p==hlt::Position(j, i)) {
						exist = true;
					}
				}
				//if the current case is more interesting and no other ship is going to it yet
				if (tmp_interest > destInterest && !exist) {

					//hlt::log::log("i : "+ std::to_string(i) + ", j: " + std::to_string(j) + ", at: " + std::to_string(interest.at(i).at(j)));
					//hlt::log::log("dist : " + std::to_string(dist) + ", pow: " + std::to_string(std::powf(0.8f, dist)) + ", result: " + std::to_string(tmpInterest));
					destX = j;
					destY = i;
					destInterest = tmp_interest;
				}

			}
		}
		l_destination = hlt::Position(destX, destY);
		(*destinations).push_back(l_destination);
		return l_destination;
		
	}

	hlt::Direction MyShip::MyShip::MoveAny() {
		auto l_map = m_game->game_map.get();
		auto l_ship = m_game->me->ships[m_id];
		auto l_pos = l_ship->position;

		auto l_possibleDest = std::vector<hlt::Direction>();
		//Still
		//l_possibleDest.push_back(l_map->at(p_position));
		//North
		auto l_cellNorth = l_map->at(l_pos.directional_offset(hlt::Direction::NORTH));
		if (l_cellNorth->is_empty())
			l_possibleDest.push_back((hlt::Direction::NORTH));
		//SOUTH
		auto l_cellSouth = l_map->at(l_pos.directional_offset(hlt::Direction::SOUTH));
		if (l_cellSouth->is_empty())
			l_possibleDest.push_back(hlt::Direction::SOUTH);
		//WEST
		auto l_cellWest = l_map->at(l_pos.directional_offset(hlt::Direction::WEST));
		if (l_cellWest->is_empty())
			l_possibleDest.push_back(hlt::Direction::WEST);
		//EAST
		auto l_cellEast = l_map->at(l_pos.directional_offset(hlt::Direction::EAST));
		if (l_cellEast->is_empty())
			l_possibleDest.push_back(hlt::Direction::SOUTH);

		if (l_possibleDest.size() > 0) {
			
			return l_possibleDest.at((int)((std::rand()/(float)RAND_MAX)* ((float)l_possibleDest.size()-1)));
		}
		else {
			return hlt::Direction::STILL;
		}
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
