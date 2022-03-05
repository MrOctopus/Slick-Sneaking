#include "SneakAttack.h"
#include "../Listeners/Hooks.h"
#include "../Data.h"
#include "../Utility.h"

namespace SneakAttack
{
	bool DetermineSuccess(RE::Actor* a_player, RE::Actor* a_target)
	{
		std::uint32_t success_chance = a_player->GetLevel() - a_target->GetLevel();
		logger::info("Success chance {}"sv, success_chance);

		// Hardcoded 20 for now
		if (true) {
		//if (success_chance >= -5) {
			success_chance += static_cast<std::uint32_t>(a_player->GetActorValue(RE::ActorValue::kSneak) * 0.5f + a_player->GetActorValue(RE::ActorValue::kOneHanded));
			
			auto detected = Hooks::StealthMeterHandlerHook::GetSingleton()->detection_level > 0;
			logger::info("Is detected {}"sv, detected);
			
			success_chance += (detected) ? -15 : 60;

			auto helmet = a_target->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kHead);
			
			if (helmet) {
				auto rating = static_cast<std::uint32_t>(helmet->GetArmorRating());
				auto weight = static_cast<std::uint32_t>(helmet->GetWeight());
				logger::info("Rating is: {}, and weight is {}"sv, rating, weight);

				if (rating > 15 && weight < 2) {
					success_chance -= (weight == 0) ? (rating / 2) : (rating * 2);
					success_chance -= 10;
				}
			}

			if (Utility::RandomInt(0, 100) <= success_chance) {
				logger::info("Success!"sv);
				return true;
			}
		}

		logger::info("Failed!"sv);
		return false;
	}

	void Handler::NormalAssassination(RE::Actor* a_player, RE::Actor* a_target, std::uint32_t a_equipped_type)
	{
		if (true) {
		//if (DetermineSuccess(a_player, a_target)) {
			auto data_manager = Data::DataManager::GetSingleton();
			auto idle = (a_equipped_type == 0) ? data_manager->idle_choke_hold : data_manager->idle_slit_throat;

			if (!Utility::PlayIdle(a_player, idle, a_target)) {
				Utility::MoveInfrontOf(a_target, a_player, true);
				Utility::PlayIdle(a_player, idle, a_target);
			}
		}
	}
	
	void Handler::SleepAssassination(RE::Actor* a_player, RE::Actor* a_target, std::uint32_t a_equipped_type)
	{
		a_target->SetActorValue(RE::ActorValue::kHealth, 1.0);

		if (a_equipped_type == 0) {
			a_player->NotifyAnimationGraph("pa_KillMove1HMChaurusFlyerStomp");
		} 
		else if (a_equipped_type <= 2) {
			a_player->NotifyAnimationGraph("pa_KillMove1HMSpiderStabA");
		} 
		else {
			a_player->NotifyAnimationGraph("pa_KillMove1HMRieklingSlash");
		}
	}

	void Handler::SendEvent() 
	{
		auto target = Utility::GetValidTarget();

		if (!target) return;

		auto player = RE::PlayerCharacter::GetSingleton();
		auto equipped_type = Utility::GetEquippedItemType(player->GetEquippedObject(false));

		if (target->IsEssential()) {
			logger::info("Target is essential"sv);
			// Todo, better message
			// Maybe add overrride?
			Utility::ShowHUDMessage("Target is essential");
			return;
		}

		auto target_state = target->GetSitSleepState();

		switch (target_state) {
		case RE::SIT_SLEEP_STATE::kNormal:
		case RE::SIT_SLEEP_STATE::kWantToStand:
		case RE::SIT_SLEEP_STATE::kIsSitting:
			NormalAssassination(player, target, equipped_type);
			break;
		case RE::SIT_SLEEP_STATE::kIsSleeping:
		case RE::SIT_SLEEP_STATE::kWantToWake:
			SleepAssassination(player, target, equipped_type);
			break;
		}
	}
}
