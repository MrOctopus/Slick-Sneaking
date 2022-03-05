#pragma once

namespace Utility
{
	[[maybe_unused]] bool PlayIdle(RE::Actor* a_actor, RE::TESIdleForm* a_form, RE::TESObjectREFR* a_target = nullptr);
	[[nodiscard]] std::uint32_t RandomInt(std::uint32_t a_min, std::uint32_t a_max);
	[[nodiscard]] RE::Actor* GetValidTarget();
	[[nodiscard]] double GetHeadingAngle(RE::TESObjectREFR* a_object, RE::TESObjectREFR* a_target, bool a_abs = true);
	[[nodiscard]] std::uint32_t GetEquippedItemType(RE::TESForm* a_equipped_item);
	void ShowHUDMessage(const char* a_text, const char* a_menu = "", bool a_unk_flag = true);
	void MoveTo(RE::TESObjectREFR* a_object, RE::TESObjectREFR* a_target, float a_x_offset = 0.0, float a_y_offset = 0.0, float a_z_offset = 0.0, bool a_match_rotation = true);
	void MoveInfrontOf(RE::TESObjectREFR* a_object, RE::TESObjectREFR* a_target, bool a_match_rotation = true);
}
