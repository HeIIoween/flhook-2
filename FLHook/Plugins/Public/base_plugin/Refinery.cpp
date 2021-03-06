#include "Main.h"

const char* RECIPES_NAMES[] =
{ "Unknown", "recipe_make_dockmodule",
	"recipe_make_jumpdrive_ii", "recipe_make_jumpdrive_iii", "recipe_make_jumpdrive_iv",
	"recipe_make_hypscanner1", "recipe_make_hypscanner2", "recipe_make_hypscanner3",
	"recipe_cloak_small", "recipe_cloak_medium", "recipe_cloak_large", "recipe_cloak_transport",
	"recipe_cloak_disruptor_1", "recipe_cloak_disruptor_2", "recipe_cloak_disruptor_3", "recipe_jdmatrix_1",
	"recipe_IPA", "recipe_refined_aluminium", "recipe_refined_beryllium", "recipe_refined_gold", "recipe_refined_iridium", 
	"recipe_refined_niobium", 0 };

const wchar_t* REFINERY_NAMES[] =
{L"Unknown", L"Unknown", L"Unknown", L"Unknown", L"Unknown", L"Unknown", L"Unknown", 
L"Unknown", L"Unknown", L"Unknown", L"Unknown", L"Unknown", L"Unknown", L"Ore Refinery", 0};

RefineryModule::RefineryModule(PlayerBase* the_base)
	: Module(0), base(the_base)
{
	active_recipe.nickname = 0;
}

// Find the recipe for this building_type and start construction.
RefineryModule::RefineryModule(PlayerBase* the_base, uint the_type)
	: Module(the_type), base(the_base)
{
	active_recipe.nickname = 0;
}

wstring RefineryModule::GetInfo(bool xml)
{
	wstring info;

	std::wstring Status = L"";
	if (Paused)	Status = L"(Paused) ";
	else Status = L"(Active) ";

	if (xml)
	{
		info += REFINERY_NAMES[type];
		info += L"</TEXT><PARA/><TEXT>      Pending " + stows(itos(build_queue.size())) + L" items</TEXT>";
		if (active_recipe.nickname)
		{
			info += L"<PARA/><TEXT>      Building " + Status + active_recipe.infotext + L". Waiting for:</TEXT>";

			for (map<uint, uint>::iterator i = active_recipe.consumed_items.begin();
				i != active_recipe.consumed_items.end(); ++i)
			{
				uint good = i->first;
				uint quantity = i->second;

				const GoodInfo* gi = GoodList::find_by_id(good);
				if (gi)
				{
					info += L"<PARA/><TEXT>      - " + stows(itos(quantity)) + L"x " + HkGetWStringFromIDS(gi->iIDSName);
					if (quantity > 0 && base->HasMarketItem(good) < active_recipe.cooking_rate)
						info += L" [Out of stock]";
					info += L"</TEXT>";
				}
			}
		}
		info += L"<TEXT>";
	}
	else
	{
		info += REFINERY_NAMES[type];
		info += L" - Pending " + stows(itos(build_queue.size())) + L" items ";
		if (active_recipe.nickname)
		{
			info = L" - Building " + Status + active_recipe.infotext + L". Waiting for:";

			for (map<uint, uint>::iterator i = active_recipe.consumed_items.begin();
				i != active_recipe.consumed_items.end(); ++i)
			{
				uint good = i->first;
				uint quantity = i->second;

				const GoodInfo* gi = GoodList::find_by_id(good);
				if (gi)
				{
					info += L" " + stows(itos(quantity)) + L"x " + HkGetWStringFromIDS(gi->iIDSName);
				}
			}
		}
	}

	return info;
}

// Every 10 seconds we consume goods for the active recipe at the cooking rate
// and if every consumed item has been used then declare the the cooking complete
// and convert this module into the specified type.	
bool RefineryModule::Timer(uint time)
{

	if ((time % set_tick_time) != 0)
		return false;

	// Get the next item to make from the build queue.
	if (!active_recipe.nickname && build_queue.size())
	{
		map<uint, RECIPE>::iterator i = recipes.find(build_queue.front());
		if (i != recipes.end())
		{
			active_recipe = i->second;
		}
		build_queue.pop_front();
	}

	// Nothing to do.
	if (!active_recipe.nickname)
		return false;

	if (Paused)
		return false;

	// Consume goods at the cooking rate.
	bool cooked = true;
	for (map<uint, uint>::iterator i = active_recipe.consumed_items.begin();
		i != active_recipe.consumed_items.end(); ++i)
	{
		uint good = i->first;
		uint quantity = i->second > active_recipe.cooking_rate ? active_recipe.cooking_rate : i->second;
		if (quantity)
		{
			cooked = false;
			map<uint, MARKET_ITEM>::iterator market_item = base->market_items.find(good);
			if (market_item != base->market_items.end())
			{
				if (market_item->second.quantity >= quantity)
				{
					i->second -= quantity;
					base->RemoveMarketGood(good, quantity);
					return false;
				}
			}
		}
	}

	// Do nothing if cooking is not finished
	if (!cooked)
		return false;

	int producedNum = 500;
	// Add the newly produced item to the market. If there is insufficient space
	// to add the item, wait until there is space.


	if (!base->AddMarketGood(active_recipe.produced_item, producedNum))
		return false;

	// Reset the nickname to load a new item from the build queue
	// next time around.
	active_recipe.nickname = 0;
	return false;
}

void RefineryModule::LoadState(INI_Reader& ini)
{
	active_recipe.nickname = 0;
	while (ini.read_value())
	{
		if (ini.is_value("type"))
		{
			type = ini.get_value_int(0);
		}
		else if (ini.is_value("nickname"))
		{
			active_recipe.nickname = ini.get_value_int(0);
		}
		else if (ini.is_value("paused"))
		{
			Paused = ini.get_value_bool(0);
		}
		else if (ini.is_value("produced_item"))
		{
			active_recipe.produced_item = ini.get_value_int(0);
		}
		else if (ini.is_value("cooking_rate"))
		{
			active_recipe.cooking_rate = ini.get_value_int(0);
		}
		else if (ini.is_value("infotext"))
		{
			active_recipe.infotext = stows(ini.get_value_string());
		}
		else if (ini.is_value("consumed"))
		{
			active_recipe.consumed_items[ini.get_value_int(0)] = ini.get_value_int(1);
		}
		else if (ini.is_value("build_queue"))
		{
			build_queue.push_back(ini.get_value_int(0));
		}
	}
}

void RefineryModule::SaveState(FILE* file)
{
	fprintf(file, "[RefineryModule]\n");
	fprintf(file, "type = %u\n", type);
	fprintf(file, "nickname = %u\n", active_recipe.nickname);
	fprintf(file, "paused = %d\n", Paused);
	fprintf(file, "produced_item = %u\n", active_recipe.produced_item);
	fprintf(file, "cooking_rate = %u\n", active_recipe.cooking_rate);
	fprintf(file, "infotext = %s\n", wstos(active_recipe.infotext).c_str());
	for (map<uint, uint>::iterator i = active_recipe.consumed_items.begin();
		i != active_recipe.consumed_items.end(); ++i)
	{
		fprintf(file, "consumed = %u, %u\n", i->first, i->second);
	}
	for (list<uint>::iterator i = build_queue.begin();
		i != build_queue.end(); ++i)
	{
		fprintf(file, "build_queue = %u\n", *i);
	}
}

bool RefineryModule::AddToQueue(uint equipment_type)
{
	if(type == Module::TYPE_O_REFINERY)
	{
		if (equipment_type == 17 ||
			equipment_type == 18 ||
			equipment_type == 19 ||
			equipment_type == 20 ||
			equipment_type == 21)
		{
			build_queue.push_back(CreateID(RECIPES_NAMES[equipment_type]));
			return true;
		}
	}	
	//else if (type == Module::TYPE_R_BERYLLIUM)
	//{
	//	if (equipment_type == 2
	//		|| equipment_type == 3
	//		|| equipment_type == 4)
	//	{
	//		build_queue.push_back(CreateID(RECIPE_NAMES[equipment_type]));
	//		return true;
	//	}
	//}
	//else if (type == Module::TYPE_M_HYPERSPACE_SCANNER)
	//{
	//	if (equipment_type == 5
	//		|| equipment_type == 6
	//		|| equipment_type == 7
	//		|| equipment_type == 15)
	//	{
	//		build_queue.push_back(CreateID(RECIPE_NAMES[equipment_type]));
	//		return true;
	//	}
	//}
	//else if (type == Module::TYPE_M_CLOAK)
	//{
	//	if (equipment_type == 8
	//		|| equipment_type == 9
	//		|| equipment_type == 10
	//		|| equipment_type == 11)
	//	{
	//		build_queue.push_back(CreateID(RECIPE_NAMES[equipment_type]));
	//		return true;
	//	}
	//}

	//else if (type == Module::TYPE_M_CLOAKDISRUPTOR)
	//{
	//	if (equipment_type == 12
	//		|| equipment_type == 13
	//		|| equipment_type == 14)
	//	{
	//		build_queue.push_back(CreateID(RECIPE_NAMES[equipment_type]));
	//		return true;
	//	}
	//}

	//else if (type == Module::TYPE_M_BREWERY)
	//{
	//	if (equipment_type == 16)
	//	{
	//		build_queue.push_back(CreateID(RECIPE_NAMES[equipment_type]));
	//		return true;
	//	}
	//}
	return false;
}

bool RefineryModule::ClearQueue()
{
	build_queue.clear();
	return true;
}

void RefineryModule::ClearRecipe()
{
	active_recipe.nickname = 0;
}

bool RefineryModule::ToggleQueuePaused(bool NewState)
{
	bool RememberState = Paused;
	Paused = NewState;
	return RememberState;
}