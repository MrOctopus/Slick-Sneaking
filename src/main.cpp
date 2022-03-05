#include "Data.h"
#include "Listeners/Hooks.h"

namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
		const auto level = spdlog::level::trace;
#else
		const auto level = spdlog::level::info;
#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
	}

	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type) {
		case SKSE::MessagingInterface::kDataLoaded:
			Data::Initialize();
			break;
		case SKSE::MessagingInterface::kNewGame:
		case SKSE::MessagingInterface::kPostLoadGame:
			Hooks::Initialize();
			break;
		}
	}
}

#ifdef VERSION_SE
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) Sleep(10);
	Sleep(3000);
#endif

	InitializeLog();

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();

	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"sv), ver.string());
		return false;
	}

	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	return true;
}
#else
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) Sleep(10);
	Sleep(3000);
#endif

	InitializeLog();
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);

	auto message_interface = SKSE::GetMessagingInterface();
	if (!message_interface->RegisterListener(MessageHandler)) return false;

	Hooks::Install();

	return true;
}
#endif
