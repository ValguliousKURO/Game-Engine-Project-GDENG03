

#pragma once
#include <DX3D/Core/Common.h>
#include <DX3D/Core/Base.h>
#include <DX3D/Math/Vec2.h>
#include <DX3D/Math/Rect.h>
#include <array>
#include <vector>

namespace dx3d
{
	class InputCommand
	{
	public:
		//
		virtual ~InputCommand() = default;
		virtual void execute(f32 deltaTime) = 0;
	};

	class InputListener
	{
	public:

		virtual ~InputListener() = default;

		virtual void onKeyPressed(KeyCode key) {}
		virtual void onKeyReleased(KeyCode key) {}
		virtual void onMouseMoved(const Vec2& mousePosition, const Vec2& mouseDelta) {}
	};

	enum class InputTrigger
	{
		Pressed = 0,
		Released,
		Held,
		MouseMoved
	};

	class InputSystem final : public Base
	{
	public:
		explicit InputSystem(const InputSystemDesc& desc);

		bool isKeyDown(KeyCode key) const;
		bool isKeyPressed(KeyCode key) const;
		bool isKeyReleased(KeyCode key) const;

		Vec2 getMousePosition() const noexcept;
		Vec2 getMouseDelta() const noexcept;

		bool isCursorLocked() const noexcept;

		void setCursorVisible(bool visible);
		void setCursorLocked(bool locked);
		void setCursorLockArea(const Rect& rect);

		//listener stuff
		void registerListener(InputListener& listener);
		void unregisterListener(InputListener& listener);

		void bindCommand(KeyCode key, InputTrigger trigger, UniquePtr<InputCommand> command);
		void clearCommands();

		void update(f32 deltaTime);
	private:
		short getInternalKeyCode(const KeyCode& key);
		void centerCursor();
	private:
		struct CommandBinding
		{
			KeyCode key{ KeyCode::Unknown };
			InputTrigger trigger{ InputTrigger::Pressed };
			UniquePtr<InputCommand> command{};
		};

		std::array<bool, static_cast<std::size_t>(KeyCode::Count)> m_currentKeys{};
		std::array<bool, static_cast<std::size_t>(KeyCode::Count)> m_previousKeys{};

		Vec2 m_mousePosition{};
		Vec2 m_previousMousePosition{};
		Vec2 m_mouseDelta{};

		Rect m_lockArea{};

		std::vector<InputListener*> m_listeners{};
		std::vector<CommandBinding> m_commandBindings{};

		bool m_cursorVisible{ true };
		bool m_cursorLocked{ false };
	};

}
