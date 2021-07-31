#ifndef _MOUSE_EVENT_H
#define _MOUSE_EVENT_H

#include "Event.h"

// Base MouseEvent class, all mouse-related events derive functionality from this
class MouseMovedEvent : public Event
{
public:
	// Rule of three + overloaded constructor
	MouseMovedEvent(const float x, const float y) : xPos(x), yPos(y) {}
	MouseMovedEvent(const MouseMovedEvent& other) = default;
	~MouseMovedEvent() = default;

	// Base class overrides
	virtual const EventCategory GetCategory() const override { return EventCategory::MOUSE; }
	virtual const EventType GetType() const override { return EventType::MOUSE_MOVED_T; }

	// Getters for the data
	const float GetX() const { return xPos; }
	const float GetY() const { return yPos; }

private:

	// Mouse data
	const float xPos, yPos;
};

class MouseScrolledEvent : public Event
{
public:
	// Rule of three + overloaded constructor
	MouseScrolledEvent(const float dist) : scrollDist(dist) {}
	MouseScrolledEvent(const MouseScrolledEvent& other) = default;
	~MouseScrolledEvent() = default;

	// Base class overrides
	virtual const EventCategory GetCategory() const override { return EventCategory::MOUSE; }
	virtual const EventType GetType() const override { return EventType::MOUSE_SCROLLED_T; }

	// Getters for the data
	const float GetScrollDist() const { return scrollDist; }

private:

	const float scrollDist;
};

class MouseButtonEvent : public Event
{
public:
	// Rule of three + overloaded constructor
	MouseButtonEvent(const float button) : mouseButton(button) {}
	MouseButtonEvent(const MouseButtonEvent& other) = default;
	~MouseButtonEvent() = default;

	// Keep the methods pure virtual so that instancing this class is not possible
	virtual const EventCategory GetCategory() const = 0;
	virtual const EventType GetType() const = 0; 

	// Getters for the data
	const uint16_t GetButton() const { return mouseButton; }

private:

	const uint16_t mouseButton;
};

class MouseButtonDownEvent : public MouseButtonEvent
{
public:
	// Rule of three + overloaded constructor
	MouseButtonDownEvent(const float button) : MouseButtonEvent(button) {}
	MouseButtonDownEvent(const MouseButtonDownEvent& other) = default;
	~MouseButtonDownEvent() = default;

	// Base class overrides
	virtual const EventCategory GetCategory() const override { return EventCategory::MOUSE; }
	virtual const EventType GetType() const override { return EventType::MOUSE_BUTTON_DOWN_T; }

private:

	// No extra data necessary
};

class MouseButtonUpEvent : public MouseButtonEvent
{
public:
	// Rule of three + overloaded constructor
	MouseButtonUpEvent(const float button) : MouseButtonEvent(button) {}
	MouseButtonUpEvent(const MouseButtonUpEvent& other) = default;
	~MouseButtonUpEvent() = default;

	// Base class overrides
	virtual const EventCategory GetCategory() const override { return EventCategory::MOUSE; }
	virtual const EventType GetType() const override { return EventType::MOUSE_BUTTON_UP_T; }

private:

	// No extra data necessary

};

#endif