#include <thread>
#include "App.h"

namespace ME
{
App::App(u8 fpsTarget, s32 argc, char** argv) : m_tickRate(Time::Seconds(1.0f / fpsTarget))
{
#if ENABLED(DEBUG_LOGGING)
	LE::g_MinLogSeverity = LE::LogSeverity::Debug;
#endif
#if ENABLED(ASSERTS)
	std::string msg;
	msg.reserve(16);
	msg += "Invalid argc! ";
	msg += std::to_string(argc);
	AssertVar(argc > 0, msg.c_str());
#endif
	if (argc > 0)
	{
		std::string_view exePath = argv[0];
#ifdef TARGET_WIN64
		const char delim = '\\';
#else
		const char delim = '/';
#endif
		m_workingDir = exePath.substr(0, exePath.find_last_of(delim));
		LOG_I("Working dir: %s", m_workingDir.data());
	}
	g_pResources->s_resourcesPath = m_workingDir;
	g_pResources->s_resourcesPath += "/";
	g_pResources->s_resourcesPath += "Resources";
	if (g_pResources->Init({"Default-Mono.ttf", "Default-Serif.ttf"}))
	{
		m_state.flags[State::INIT] = true;
	}
}

App::~App()
{
	// Stop any active Worlds
	g_pContext->Stop();
	// Clear all primitives
	g_pRenderer->Clear();
	// Clear all resources
	g_pResources->Clear();
	LOG_I("[App] terminated and destroyed");
}

void App::CreateViewport(u32 width, u32 height, const std::string& title)
{
	Assert(!m_viewport.isOpen(), "Viewport already open!");
	ViewportData data;
	data.viewportSize.width = width;
	data.viewportSize.height = height;
	data.title = title;
	m_viewport.SetData(std::move(data));
	m_viewport.Create(s_AA);
}

void App::StartGame()
{
	m_services.StartGame();
}

void App::StartFrame()
{
	g_pContext->StartFrame();
}

void App::PollEvents()
{
	auto vpEvent = g_pInput->PollEvents(m_viewport);
	switch (vpEvent)
	{
	default:
		break;

	case ViewportEventType::Closed:
		m_state.flags[State::CLOSED] = true;
		break;
	}
}

void App::Tick(Time dt)
{
	g_pInput->TakeSnapshot();
	g_pInput->FireCallbacks();
	g_pContext->Tick(dt);
}

void App::Render()
{
	g_pRenderer->Render(m_viewport);
}

void App::Sleep(Time frameTime)
{
	Time residue = m_tickRate - frameTime;
	if (residue > Time::Zero)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(residue.AsMilliseconds()));
	}
}

void App::Quit()
{
	m_state.flags[State::Flag::CLOSED] = true;
}

bool App::IsInit() const
{
	return m_state.flags[State::INIT];
}

bool App::IsRunning() const
{
	return m_state.flags[State::INIT] && !m_state.flags[State::QUITTING] && !m_state.flags[State::CLOSED] && m_viewport.isOpen();
}

bool App::IsViewportOpen() const
{
	return m_viewport.isOpen();
}

std::string_view App::WorkingDir() const
{
	return m_workingDir;
}
} // namespace ME