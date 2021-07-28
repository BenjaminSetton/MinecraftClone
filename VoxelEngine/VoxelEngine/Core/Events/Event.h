#ifndef _EVENT_H
#define _EVENT_H

// Event data classes
class KeyboardEvent : public Event
{
public:
	// Rule of three + overloaded constructor
	KeyboardEvent() = default;
	KeyboardEvent(const EventType eventType);
	KeyboardEvent(const KeyboardEvent& other) = default;
	~KeyboardEvent() = default;

	// Event functions
	virtual const EventCategory GetCategory() const override;

	// Keyboard data
	unsigned int m_key;
	bool m_isPressed;
};

class MouseEvent : public Event
{
public:
	// Rule of three + overloaded constructor
	MouseEvent() = default;
	MouseEvent(const EventType eventType);
	MouseEvent(const MouseEvent & other) = default;
	~MouseEvent() = default;

	// Event functions
	virtual const EventCategory GetCategory() const override;

	// Mouse data
	float m_x, m_y;
};

// Event type enumerations
enum class EventType
{
	None = 0,
	KeyboardKeyDown, KeyboardKeyUp,
	MouseButtonDown, MouseButtonUp
};

enum EventCategory
{
	Keyboard = 0,
	Mouse,
	Application,
};

class Event
{
public:

	Event() = default;
	Event(const EventType eventType);
	Event(const Event& other) = default;
	~Event() = default;

	const EventType GetType() const;
	virtual const EventCategory GetCategory() const = 0;

protected:

	EventType m_type;

};

#endif