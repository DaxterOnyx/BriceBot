#include "../hlt/game.hpp"
#include "../hlt/constants.hpp"
#include "../hlt/log.hpp"
#include "MyShip.hpp"

#include <random>
#include <ctime>
#include <chrono>
#include <map>
#define RAND_MAX 1024

using namespace std;
using namespace hlt;

int main(int argc, char* argv[]) {
	Game game;
	unsigned int rng_seed;
	if (argc > 1) {
		rng_seed = static_cast<unsigned int>(stoul(argv[1]));
	}
	else {
		rng_seed = static_cast<unsigned int>(time(nullptr));
	}
	rng_seed *= game.my_id + 1;
	srand(rng_seed);


	map<int, BriceBot::MyShip*> ships;

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

		log::log("NB de ship" + std::to_string(me->ships.size()));
		for (const auto &ship_iterator : me->ships) {
			//GET Ship destination
			shared_ptr<Ship> ship = ship_iterator.second;
			if (ships.count(ship->id) == 0)
			{
				//Create MyShip
				BriceBot::MyShip* new_ship = new BriceBot::MyShip(&game, ship->id);
				ships.insert(std::pair<int, BriceBot::MyShip*>(ship->id, new_ship));
			}
			BriceBot::MyShip* l_ship = ships[ship->id];

			//Calculate move
			hlt::Direction direction = l_ship->Move();
			if (direction == hlt::Direction::STILL)
			{
				command_queue.push_back(ship->stay_still());
			}
			else
			{
				command_queue.push_back(ship->move(direction));
			}
		}


		if (me->halite >= constants::SHIP_COST &&
			!game_map->at(me->shipyard)->is_occupied())
		{
			command_queue.push_back(me->shipyard->spawn());
			log::log("Spawn new ship");
		}

		if (!game.end_turn(command_queue)) {
			break;
		}
	}

	return 0;
}
