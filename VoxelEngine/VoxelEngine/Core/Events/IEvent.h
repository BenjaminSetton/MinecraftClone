#ifndef _IEVENT_H
#define _IEVENT_H

#include <vector>
#include "Event.h"

// Observer receives messages from Subject through OnEvent()
class EventObserver
{
public:

	EventObserver(const EventCategory categories);

	const uint16_t GetSubscribedCategories() const;
	
	const bool IsSubscribedToCategory(EventCategory& cat);

	virtual void OnEvent(const Event& event) = 0;

private:

	uint16_t m_subscribedCategories = 0;

};

// Subject holds references to all observers and is able to broadcast messages
class EventSubject
{
public:

	EventSubject();

	~EventSubject();

	virtual void Subscribe(EventObserver* observer);
	virtual void Unsubscribe(EventObserver* observer);

	// This will only broadcast to observers who are subscribed to the event categories
	virtual void Broadcast(const Event& event);

protected:

	std::vector<EventObserver*> observerList;

};

extern EventSubject* GlobalSubject = nullptr;

#endif