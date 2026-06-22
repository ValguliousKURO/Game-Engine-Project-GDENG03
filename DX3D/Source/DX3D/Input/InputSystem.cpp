/*MIT License

C++ 3D Game Tutorial Series (https://github.com/PardCode/CPP-3D-Game-Tutorial-Series)

Copyright (c) 2019-2026, PardCode

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <DX3D/Input/InputSystem.h>
#include <ranges>
#include <algorithm>
#include <utility>
#include <Windows.h>

dx3d::InputSystem::InputSystem(const InputSystemDesc& desc) : Base(desc.base)
{
}

bool dx3d::InputSystem::isKeyDown(KeyCode key) const
{
	return m_currentKeys[static_cast<std::size_t>(key)];
}

bool dx3d::InputSystem::isKeyPressed(KeyCode key) const
{
	return m_currentKeys[static_cast<std::size_t>(key)] &&
		!m_previousKeys[static_cast<std::size_t>(key)];
}

bool dx3d::InputSystem::isKeyReleased(KeyCode key) const
{
	return !m_currentKeys[static_cast<std::size_t>(key)] &&
		m_previousKeys[static_cast<std::size_t>(key)];
}

dx3d::Vec2 dx3d::InputSystem::getMousePosition() const noexcept
{
	return m_mousePosition;
}

dx3d::Vec2 dx3d::InputSystem::getMouseDelta() const noexcept
{
	return m_mouseDelta;
}

bool dx3d::InputSystem::isCursorLocked() const noexcept
{
	return m_cursorLocked;
}

void dx3d::InputSystem::setCursorVisible(bool visible)
{
	m_cursorVisible = visible;

	while (ShowCursor(visible) < 0 && visible) {}
	while (ShowCursor(visible) >= 0 && !visible) {}
}

void dx3d::InputSystem::setCursorLocked(bool locked)
{
	m_cursorLocked = locked;
	if (locked) centerCursor();
}

void  dx3d::InputSystem::setCursorLockArea(const Rect& rect)
{
	m_lockArea = rect;
}

void dx3d::InputSystem::registerListener(InputListener& listener)
{
	if (std::find(m_listeners.begin(), m_listeners.end(), &listener) != m_listeners.end())
		return;

	m_listeners.push_back(&listener);
}

void dx3d::InputSystem::unregisterListener(InputListener& listener)
{
	std::erase(m_listeners, &listener);
}

void dx3d::InputSystem::bindCommand(KeyCode key, InputTrigger trigger, UniquePtr<InputCommand> command)
{
	if (!command) return;
	m_commandBindings.push_back(CommandBinding{ key, trigger, std::move(command) });
}

void dx3d::InputSystem::clearCommands()
{
	m_commandBindings.clear();
}

void dx3d::InputSystem::centerCursor()
{
	const auto centerX = m_lockArea.left + (m_lockArea.width / 2);
	const auto centerY = m_lockArea.top + (m_lockArea.height / 2);

	SetCursorPos(centerX, centerY);

	m_mousePosition.x = static_cast<f32>(centerX);
	m_mousePosition.y = static_cast<f32>(centerY);
}

void dx3d::InputSystem::update(f32 deltaTime)
{
	m_previousKeys = m_currentKeys;

	for (auto i : std::views::iota(0u, static_cast<std::size_t>(KeyCode::Count)))
	{
		const auto vk = getInternalKeyCode(static_cast<KeyCode>(i));
		m_currentKeys[i] = (GetAsyncKeyState(vk) & 0x8000) != 0;
	}

	auto notifyKeyPressed = [this](KeyCode key)
	{
		const auto listeners = m_listeners;
		for (auto* listener : listeners)
		{
			if (listener) listener->onKeyPressed(key);
		}
	};

	auto notifyKeyReleased = [this](KeyCode key)
	{
		const auto listeners = m_listeners;
		for (auto* listener : listeners)
		{
			if (listener) listener->onKeyReleased(key);
		}
	};

	auto notifyMouseMoved = [this](const Vec2& position, const Vec2& delta)
	{
		const auto listeners = m_listeners;
		for (auto* listener : listeners)
		{
			if (listener) listener->onMouseMoved(position, delta);
		}
	};

	auto executeCommands = [this, deltaTime](KeyCode key, InputTrigger trigger)
	{
		for (auto& binding : m_commandBindings)
		{
			if (!binding.command) continue;
			if (binding.trigger != trigger) continue;

			if (trigger != InputTrigger::MouseMoved && binding.key != key)
				continue;

			binding.command->execute(deltaTime);
		}
	};

	m_previousMousePosition = m_mousePosition;

	POINT point{};
	GetCursorPos(&point);

	m_mousePosition.x = static_cast<f32>(point.x);
	m_mousePosition.y = static_cast<f32>(point.y);

	m_mouseDelta.x = m_mousePosition.x - m_previousMousePosition.x;
	m_mouseDelta.y = m_mousePosition.y - m_previousMousePosition.y;

	for (auto i : std::views::iota(0u, static_cast<std::size_t>(KeyCode::Count)))
	{
		const auto key = static_cast<KeyCode>(i);

		if (m_currentKeys[i] && !m_previousKeys[i])
		{
			notifyKeyPressed(key);
			executeCommands(key, InputTrigger::Pressed);
		}

		if (m_currentKeys[i])
		{
			executeCommands(key, InputTrigger::Held);
		}

		if (!m_currentKeys[i] && m_previousKeys[i])
		{
			notifyKeyReleased(key);
			executeCommands(key, InputTrigger::Released);
		}
	}

	if (m_mouseDelta.x != 0.0f || m_mouseDelta.y != 0.0f)
	{
		notifyMouseMoved(m_mousePosition, m_mouseDelta);
		executeCommands(KeyCode::Unknown, InputTrigger::MouseMoved);
	}

	if (m_cursorLocked) centerCursor();
}

short dx3d::InputSystem::getInternalKeyCode(const KeyCode& key)
{
	const auto value = static_cast<int>(key);
	// A-Z
	if (key >= KeyCode::A && key <= KeyCode::Z) return 'A' + (value - static_cast<int>(KeyCode::A));
	// 0-9
	if (key >= KeyCode::Num0 && key <= KeyCode::Num9) return '0' + (value - static_cast<int>(KeyCode::Num0));

	switch (key)
	{
	case KeyCode::Shift: return VK_SHIFT;
	case KeyCode::Escape: return VK_ESCAPE;
	case KeyCode::Backspace: return VK_BACK;
	case KeyCode::Delete: return VK_DELETE;
	case KeyCode::Space: return VK_SPACE;
	case KeyCode::Enter: return VK_RETURN;
	case KeyCode::MouseLeft: return VK_LBUTTON;
	case KeyCode::MouseMiddle: return VK_MBUTTON;
	case KeyCode::MouseRight: return VK_RBUTTON;
	case KeyCode::Up: return VK_UP;
	case KeyCode::Down: return VK_DOWN;
	case KeyCode::Left: return VK_LEFT;
	case KeyCode::Right: return VK_RIGHT;
	default: return 0;
	}
}
