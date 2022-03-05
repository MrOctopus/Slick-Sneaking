#pragma once

namespace Data
{
	class DataManager
	{
	public:
		[[nodiscard]] static DataManager* GetSingleton()
		{
			static DataManager singleton;
			return std::addressof(singleton);
		}

		static void Initialize();

		RE::TESIdleForm* idle_slit_throat;
		RE::TESIdleForm* idle_choke_hold;
		RE::BGSKeyword* keyword_cooking_pot;
	};

	inline void Initialize()
	{
		DataManager::Initialize();
	}
}
