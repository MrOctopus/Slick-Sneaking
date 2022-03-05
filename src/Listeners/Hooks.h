#pragma once

namespace Hooks
{
	class StealthMeterHandlerHook
	{
	public:
		std::atomic_bool valid_target{ false };
		std::uint32_t detection_level;

		[[nodiscard]] static StealthMeterHandlerHook* GetSingleton()
		{
			static StealthMeterHandlerHook singleton;
			return std::addressof(singleton);
		}

		static void Install()
		{
			REL::Relocation<std::uintptr_t> vtbl_player{ REL::ID{ 208040 } };
			REL::Relocation<std::uintptr_t> vtbl_ui{ REL::ID{ 215386 } };

			_func_update = vtbl_player.write_vfunc(0xAD, PlayerCharacterUpdate);
			_func_ui = vtbl_ui.write_vfunc(0x2, ProcessMessage);

			logger::info("Installed {}"sv, typeid(StealthMeterHandlerHook).name());
		}

		void SetEnabled(bool a_enable);

	protected:
		static void PlayerCharacterUpdate(RE::PlayerCharacter* a_player, float a_delta)
		{
			auto hook = GetSingleton();
			hook->_delta = a_delta;
			_func_update(a_player, a_delta);
		}

		static bool ProcessMessage(RE::StealthMeter* a_this, RE::UIMessage* a_message)
		{
			auto hook = GetSingleton();

			if (hook->_enabled && a_message) {
				hook->HookLogic(a_this, a_message);
			}

			return _func_ui(a_this, a_message);
		}

		bool CanStealthAttack();
		void UpdateCrosshair(bool a_state);
		void HookLogic(RE::StealthMeter* a_this, RE::UIMessage* a_message);

	private:
		static inline REL::Relocation<decltype(PlayerCharacterUpdate)> _func_update;
		static inline REL::Relocation<decltype(ProcessMessage)> _func_ui;
		
		double _delta;
		double _timer;
		
		double _x_original;
		double _y_original;
		double _x_start;
		double _y_start;
		double _x_end;
		double _y_end;

		bool _lerping;
		bool _enabled;

		StealthMeterHandlerHook() 
		{
			_x_original = -1.0f;
			_y_original = -1.0f;
			_lerping = false;
			_enabled = false;
		}
	};

	class AttackBlockHandlerHook
	{	
	public:
		bool _enabled;

		[[nodiscard]] static AttackBlockHandlerHook* GetSingleton()
		{
			static AttackBlockHandlerHook singleton;
			return std::addressof(singleton);
		}

		static void Install() 
		{
			REL::Relocation<std::uintptr_t> vtbl{ REL::ID{ 208719 } };
			_func = vtbl.write_vfunc(0x4, ProcessButton);
			logger::info("Installed {}"sv, typeid(AttackBlockHandlerHook).name());
		}

		void SetEnabled(bool a_enable);

	protected:
		static bool HookLogic(RE::ButtonEvent* a_event);

		static void ProcessButton(RE::AttackBlockHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
		{
			auto this_hook = GetSingleton();
			bool run_original = true;

			if (this_hook->_enabled) {
				run_original = HookLogic(a_event);
			}
			
			if (run_original) {
				_func(a_this, a_event, a_data);
			}
		}

	private:
		static inline REL::Relocation<decltype(ProcessButton)> _func;

		AttackBlockHandlerHook()
		{
			_enabled = false;
		}
	};

	class SneakHandlerHook
	{
	public:
		static void Install()
		{
			//REL::Relocation<std::uintptr_t> vtbl{ RE::RTTI_SneakHandler }; <-- This RTTI information is not updated
			REL::Relocation<std::uintptr_t> vtbl{ REL::ID{ 208733 } };
			_func = vtbl.write_vfunc(0x4, ProcessButton);
			logger::info("Installed {}"sv, typeid(SneakHandlerHook).name());
		}

	protected:
		static void HookLogic(RE::ButtonEvent* a_event);

		static void ProcessButton(RE::SneakHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
		{
			_func(a_this, a_event, a_data);
			HookLogic(a_event);
		}

	private:
		static inline REL::Relocation<decltype(ProcessButton)> _func;
	};

	inline void Initialize() 
	{
		auto player = RE::PlayerCharacter::GetSingleton();

		if (player) {
			auto stealth_hook = StealthMeterHandlerHook::GetSingleton();
			auto attack_hook = AttackBlockHandlerHook::GetSingleton();
			bool is_sneaking = player->IsSneaking();

			stealth_hook->SetEnabled(is_sneaking);
			attack_hook->SetEnabled(is_sneaking);
		}
	}

	inline void Install()
	{
		StealthMeterHandlerHook::Install();
		AttackBlockHandlerHook::Install();
		SneakHandlerHook::Install();
	}
}
