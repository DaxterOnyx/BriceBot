#include "../hlt/game.hpp"
#include "../hlt/constants.hpp"
#include "../hlt/log.hpp"
#include "MyShip.hpp"

#include <random>
#include <ctime>
#include <chrono>
#include <map>

using namespace std;
using namespace hlt;

int main(int argc, char* argv[]) {
	unsigned int rng_seed;
	if (argc > 1) {
		rng_seed = static_cast<unsigned int>(stoul(argv[1]));
	}
	else {
		rng_seed = static_cast<unsigned int>(time(nullptr));
	}
	mt19937 rng(rng_seed);

	Game game;

	map<int, BriceBot::MyShip> ships;

	// At this point "game" variable is populated with initial map data.
	// This is a good place to do computationally expensive start-up pre-processing.
	// As soon as you call "ready" function below, the 2 second per turn timer will start.
	game.ready("BriceMyCppBot");

	log::log("Successfully created bot! My Player ID is " + to_string(game.my_id) + ". Bot rng seed is " + to_string(rng_seed) + ".");

	for (;;) {
		game.update_frame();
		shared_ptr<Player> me = game.me;
		unique_ptr<GameMap>& game_map = game.game_map;

		vector<Command> command_queue;

		if (me->halite >= constants::SHIP_COST &&
			!game_map->at(me->shipyard)->is_occupied())
		{
			command_queue.push_back(me->shipyard->spawn());
		}

		for (const auto& ship_iterator : me->ships) {
			shared_ptr<Ship> ship = ship_iterator.second;
			BriceBot::MyShip l_ship = ships[ship->id];
			int l_cellHalite = game_map->at(ship)->halite;

			//if ((l_cellHalite / 10 < ship->halite && l_cellHalite <= constants::MAX_HALITE) || ship->is_full())
			{
				hlt::Direction direction = l_ship.Move(ship->position, ship->halite, l_cellHalite);
				if (direction == hlt::Direction::STILL)
				{
					command_queue.push_back(ship->stay_still());
				}
				else 
				{
					command_queue.push_back(ship->move(direction));
				}
			}
		}

		if (!game.end_turn(command_queue)) {
			break;
		}
	}

	return 0;
}
