#include "Utility.h"

namespace Utility
{
	using DefaultObject = RE::DEFAULT_OBJECTS::DEFAULT_OBJECT;

	namespace
	{
		// a_unk_flag sometimes varies in calls from false and true
		// a_unk_flag2 I've never seen vary from false
		bool PlayIdleRE(RE::AIProcess* a_actor_p, RE::Actor* a_actor, DefaultObject a_action, RE::TESIdleForm* a_form, bool a_unk_flag, bool a_unk_flag2, RE::TESObjectREFR* a_target)
		{
			using func_t = decltype(&PlayIdleRE);
#ifdef VERSION_SE
			REL::Relocation<func_t> func{ REL::ID{ 38290 } };
#else
			REL::Relocation<func_t> func{ REL::ID{ 39256 } };
#endif
			return func(a_actor_p, a_actor, a_action, a_form, a_unk_flag, a_unk_flag2, a_target);
		}

		// Haven't seen a_unk_flag vary from true
		void ShowHUDMessageRE(const char* a_text, const char* a_menu, bool a_unk_flag)
		{
			using func_t = decltype(&ShowHUDMessageRE);
#ifdef VERSION_SE
			REL::Relocation<func_t> func{ REL::ID{ 52050 } };
#else
			REL::Relocation<func_t> func{ REL::ID{ 52933 } };
#endif
			func(a_text, a_menu, a_unk_flag);
		}

		std::uint32_t GetEquippedItemTypeRE(RE::TESForm* a_equipped_item)
		{
			using func_t = decltype(&GetEquippedItemTypeRE);
#ifdef VERSION_SE
			REL::Relocation<func_t> func{ REL::ID{ 14125 } };
#else
			REL::Relocation<func_t> func{ REL::ID{ 14220 } };
#endif
			return func(a_equipped_item);
		}

		// a_unk_flag is usually always false
		// seems to be unused?
		bool GetHeadingAngleRE(RE::TESObjectREFR* a_object, RE::TESObjectREFR* a_target, bool a_unk_flag, double& a_result)
		{
			using func_t = decltype(&GetHeadingAngleRE);
#ifdef VERSION_SE
			REL::Relocation<func_t> func{ REL::ID{ 21050 } };
#else
			REL::Relocation<func_t> func{ REL::ID{ 21500 } };
#endif
			return func(a_object, a_target, a_unk_flag, a_result);
		}

		// Credits to po3
		void MoveToRE(RE::TESObjectREFR* a_object, const RE::ObjectRefHandle& a_targetHandle, RE::TESObjectCELL* a_targetCell, RE::TESWorldSpace* a_selfWorldSpace, const RE::NiPoint3& a_position, const RE::NiPoint3& a_rotation)
		{
			using func_t = decltype(&MoveToRE);
#ifdef VERSION_SE
			REL::Relocation<func_t> func{ REL::ID{ 56227 } };
#else
			REL::Relocation<func_t> func{ REL::ID{ 56626 } };
#endif
			func(a_object, a_targetHandle, a_targetCell, a_selfWorldSpace, a_position, a_rotation);
		}
	}

	bool PlayIdle(RE::Actor* a_actor, RE::TESIdleForm* a_form, RE::TESObjectREFR* a_target)
	{
		if (a_actor) {
			auto process = a_actor->currentProcess;

			if (process) {
				// The unknown flags are set based on the papyrus calls to this function
				return PlayIdleRE(process, a_actor, DefaultObject::kActionIdle, a_form, true, false, a_target);
			}
		}

		return false;
	}

	std::uint32_t RandomInt(std::uint32_t a_min, std::uint32_t a_max)
	{
		return SKSE::stl::RNG::GetSingleton()->Generate<std::uint32_t>(a_min, a_max);
	}

	RE::Actor* GetValidTarget()
	{
		auto ref = RE::CrosshairPickData::GetSingleton()->target.get();

		if (!ref) {
			return nullptr;
		}

		// Can also use: skyrim_cast<RE::Actor*>(ref);
		auto target = ref->As<RE::Actor>();

		if (!target) {
			return nullptr;
		}

		if (target->IsDead() || target->IsGhost()) {
			return nullptr;
		}

		return target;
	}

	double GetHeadingAngle(RE::TESObjectREFR* a_object, RE::TESObjectREFR* a_target, bool a_abs)
	{
		double heading;
		GetHeadingAngleRE(a_object, a_target, false, heading);
		return a_abs ? RE::NiAbs(heading) : heading;
	}

	std::uint32_t GetEquippedItemType(RE::TESForm* a_equipped_item)
	{
		return GetEquippedItemTypeRE(a_equipped_item);
	}

	void ShowHUDMessage(const char* a_text, const char* a_menu, bool a_unk_flag)
	{
		ShowHUDMessageRE(a_text, a_menu, a_unk_flag);
	}

	void MoveTo(RE::TESObjectREFR* a_object, RE::TESObjectREFR* a_target, float a_x_offset, float a_y_offset, float a_z_offset, bool a_match_rotation)
	{
		auto handle = a_target->GetHandle();
		auto cell = a_target->GetParentCell();
		auto worldspace = a_target->GetWorldspace();
		auto pos = a_target->GetPosition();
		auto angle = (a_match_rotation) ? a_target->GetAngle() : a_object->GetAngle();

		pos.x += a_x_offset;
		pos.y += a_y_offset;
		pos.z += a_z_offset;

		MoveToRE(a_object, handle, cell, worldspace, pos, angle);
	}

	void MoveInfrontOf(RE::TESObjectREFR* a_object, RE::TESObjectREFR* a_target, bool a_match_rotation)
	{
		auto angle_z = a_target->GetAngleZ();
		MoveTo(a_object, a_target, std::sin(angle_z) * 64, std::cos(angle_z) * 64, 0, a_match_rotation);
	}
}
