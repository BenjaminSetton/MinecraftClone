#include "../../Misc/pch.h"

#include "Event.h"

KeyboardEvent::KeyboardEvent(const EventType eventType) : Event(eventType) {}
const EventCategory KeyboardEvent::GetCategory() const { return EventCategory::KEYBOARD; }

MouseEvent::MouseEvent(const EventType eventType) : Event(eventType) {}
const EventCategory MouseEvent::GetCategory() const { return EventCategory::MOUSE; }

Event::Event(const EventType eventType) : m_type(eventType) {}
const EventType Event::GetType() const { return m_type; }
