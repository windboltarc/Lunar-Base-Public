#include "tp_handler.hpp"

bool game_loaded()
{
	uintptr_t datamodel = task_scheduler::get_datamodel();
	if (!datamodel)
		return false;

	return *(int64_t*)(datamodel + update::offsets::datamodel::game_loaded) == 31;
}

void teleport_loop()
{
	uintptr_t last_datamodel = 0;

	while (true)
	{
		uintptr_t current_datamodel = task_scheduler::get_datamodel();
		if (!current_datamodel)
			continue;

		uintptr_t datamodel_instance = current_datamodel;

		while (!game_loaded())
		{
			current_datamodel = task_scheduler::get_datamodel();
			if (!current_datamodel)
				break;

			datamodel_instance = current_datamodel;
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}

		if (!game_loaded())
			continue;

		scheduler::initialize_scheduler();
		last_datamodel = current_datamodel;

		while (game_loaded())
		{
			globals::loaded = true;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		globals::loaded = false;
		last_datamodel = 0;

		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}
}

void teleport_handler::initialize()
{
	std::thread(teleport_loop).detach();
}