#include "Hooks.h"
#include "../Data.h"
#include "../Utility.h"
#include "../Handler/SneakAttack.h"

namespace Hooks
{
	namespace
	{
		bool IsValidEvent(RE::ButtonEvent* a_event)
		{
			auto control_map = RE::ControlMap::GetSingleton();

			if (!control_map) {
				return false;
			}

			auto keycode = control_map->GetMappedKey("Right Attack/Block", a_event->GetDevice());

			if (a_event->GetIDCode() != keycode || !a_event->IsDown()) {
				return false;
			}

			return true;
		}

		bool IsValidPlayerState(RE::PlayerCharacter* a_player)
		{
			if (!a_player) {
				return false;
			}

			if (a_player->IsInCombat() || a_player->GetWeaponState() != RE::WEAPON_STATE::kDrawn) {
				return false;
			}

			return true;
		}

		bool IsValidHeading(RE::PlayerCharacter* a_player, RE::Actor* a_target)
		{
			double heading = Utility::GetHeadingAngle(a_target, a_player);

			if (heading >= 130.0) {
				return true;
			}
			return false;
		}


		bool IsValidFurniture(RE::Actor* a_target)
		{
			auto data_manager = Data::DataManager::GetSingleton();
			auto furniture_base = a_target->GetOccupiedFurniture().get()->GetBaseObject()->As<RE::TESFurniture>();

			if (Utility::HasKeywords(furniture_base, data_manager->furniture_keywords, false)) {
				return true;
			}

			auto furniture_name = std::string_view(furniture_base->GetModel());

			if (furniture_name.find("Marker") != std::string::npos) {
				return true;
			}

			return false;
		}

		bool IsValidAttackState(RE::PlayerCharacter* a_player, RE::Actor* a_target)
		{
			// Valid weapon types:
			// 0: One-handed bare hand
			// 1: One-handed sword
			// 2: One-handed dagger
			// 3: One-handed axe
			// 4: One-handed mace

			auto target_state = a_target->GetSitSleepState();
			auto equipped = a_player->GetEquippedObject(false);
			auto equipped_right_type = Utility::GetEquippedItemType(equipped);

			switch (target_state) {
			case RE::SIT_SLEEP_STATE::kWantToStand:
			case RE::SIT_SLEEP_STATE::kIsSitting:
				if (!IsValidFurniture(a_target)) return false;
			case RE::SIT_SLEEP_STATE::kNormal:
				if (equipped_right_type > 2) return false;
				return IsValidHeading(a_player, a_target);
			case RE::SIT_SLEEP_STATE::kIsSleeping:
			case RE::SIT_SLEEP_STATE::kWantToWake:
				return (equipped_right_type <= 4);
			default:
				return false;
			}
		}
	}

	bool StealthMeterHandlerHook::CanStealthAttack()
	{
		auto target = Utility::GetValidTarget();

		if (!target) {
			return false;
		}

		auto player = RE::PlayerCharacter::GetSingleton();

		if (!IsValidPlayerState(player)) {
			return false;
		}

		if (!IsValidAttackState(player, target)) {
			return false;
		}

		return true;
	}

	void StealthMeterHandlerHook::UpdateCrosshair(bool a_active, double a_delta)
	{
		auto UI = RE::UI::GetSingleton();
		auto HUD = UI ? UI->GetMenu<RE::HUDMenu>() : nullptr;
		auto view = HUD ? HUD->uiMovie : nullptr;

		if (view) {
			RE::GFxValue instance;
			RE::GFxValue::DisplayInfo display;
			
			view->GetVariable(std::addressof(instance), "_root.HUDMovieBaseInstance.StealthMeterInstance");
			instance.GetDisplayInfo(std::addressof(display));

			if (a_active) {
				// Set base values
				if (_x_original == -1.0) {
					_x_original = display.GetXScale();
					_y_original = display.GetYScale();
					
					_x_start = _x_original;
					_y_start = _y_original;
					_x_end = _x_original * 1.2;
					_y_end = _y_original * 1.2;
				}

				// Lerping.
				if (_lerping) {
					_timer += a_delta;
					auto percentage = std::clamp(_timer / 0.3, 0.0, 1.0);

					double x_lerp = std::lerp(_x_start, _x_end, percentage);
					double y_lerp = std::lerp(_y_start, _y_end, percentage);

					if (x_lerp == _x_end) {
						_timer = 0.0;

						std::swap(_x_start, _x_end);
						std::swap(_y_start, _y_end);

						if (_x_start == _x_original) {
							_lerping = false;
						}
					}

					display.SetScale(x_lerp, y_lerp);
				} 
				// Check if valid target
				else if (valid_target) {
					_lerping = true;
				}
			} 
			else {
				display.SetScale(_x_original, _y_original);
				_timer = 0.0;
				_lerping = false;
				valid_target = false;
			}

			instance.SetDisplayInfo(display);
		}
	}

	void StealthMeterHandlerHook::SetEnabled(bool a_enable)
	{
		_enabled = a_enable;
		if (!a_enable) {
			valid_target = false;

			SKSE::GetTaskInterface()->AddUITask([this]() {
				this->UpdateCrosshair(false);
			});
		}
	}

	void StealthMeterHandlerHook::HookLogic(RE::StealthMeter* a_this, RE::UIMessage* a_message) 
	{
		auto data = static_cast<RE::HUDData*>(a_message->data);

		if (data->type == RE::HUDData::Type::kCrosshairSneak) {
			// Unk88 is current detection level
			detection_level = a_this->unk88;
			valid_target = CanStealthAttack();

			// Send delta as a variable to avoid race conditions
			auto delta = this->_delta;
			SKSE::GetTaskInterface()->AddUITask([this, delta]() {
				this->UpdateCrosshair(true, _delta);
			});
		}
	}

	void AttackBlockHandlerHook::SetEnabled(bool a_enable)
	{
		_enabled = a_enable;
	}

	bool AttackBlockHandlerHook::HookLogic(RE::ButtonEvent* a_event)
	{
		if (IsValidEvent(a_event)) {
			auto stealh_hook = StealthMeterHandlerHook::GetSingleton();

			if (stealh_hook->valid_target) {
				auto control_map = RE::ControlMap::GetSingleton();

				if (control_map && control_map->IsFightingControlsEnabled()) {
					SneakAttack::TriggerEvent();
					return false;
				}
			}
		}
		return true;
	}

	void SneakHandlerHook::HookLogic(RE::ButtonEvent* a_event)
	{
		if (a_event->IsDown()) {
			auto player = RE::PlayerCharacter::GetSingleton();
			if (player) {
				bool is_sneaking = player->IsSneaking();
				StealthMeterHandlerHook::GetSingleton()->SetEnabled(is_sneaking);
				AttackBlockHandlerHook::GetSingleton()->SetEnabled(is_sneaking);
			}
		}
	}
}
