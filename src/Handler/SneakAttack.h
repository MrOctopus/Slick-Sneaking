#pragma once

namespace SneakAttack
{
	class Handler
	{
	public:
		[[nodiscard]] static Handler* GetSingleton()
		{
			static Handler singleton;
			return std::addressof(singleton);
		}

		void SendEvent();

	protected:
		void NormalAssassination(RE::Actor* a_player, RE::Actor* a_target, std::uint32_t a_equipped_type);
		void SleepAssassination(RE::Actor* a_player, RE::Actor* a_target, std::uint32_t a_equipped_type);
	};

	inline void TriggerEvent()
	{
		Handler::GetSingleton()->SendEvent();
	}
}
