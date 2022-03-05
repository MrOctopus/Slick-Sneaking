#include "Data.h"

namespace Data 
{
	void DataManager::Initialize()
	{
		auto singleton = GetSingleton();

		// For modded forms we can use
		// RE::TESDataHandler::LookupFormID(RE::FormID(0x0001), "Skyrim.esm")

		// Idles
		singleton->idle_slit_throat = RE::TESForm::LookupByID<RE::TESIdleForm>(0x000F4679);
		singleton->idle_choke_hold = RE::TESForm::LookupByID<RE::TESIdleForm>(0x01000816);

		// Keywords
		singleton->keyword_cooking_pot = RE::TESForm::LookupByID<RE::BGSKeyword>(0x000A5CB3);

		logger::info("Initialized {}"sv, typeid(DataManager).name());
	}
}
