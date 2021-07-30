#ifndef _IEVENT_H
#define _IEVENT_H

#include <vector>
#include "Event.h"

// Observer receives messages from Subject through OnEvent()
class EventObserver
{
public:


	EventObserver(const EventCategory categories) : m_subscribedCategories(static_cast<uint16_t>(categories)) {}


	const uint16_t GetSubscribedCategories() const { return m_subscribedCategories; }
	
	const bool IsSubscribedToCategory(EventCategory& cat) { return m_subscribedCategories & static_cast<uint16_t>(cat); }

	virtual void OnEvent(const Event& event) = 0;

private:

	uint16_t m_subscribedCategories = 0;

};

// Subject holds references to all observers and is able to broadcast messages
class EventSubject
{
public:

	virtual void Subscribe(EventObserver* observer) { observerList.push_back(observer); }
	virtual void Unsubscribe(EventObserver* observer) 
	{
		for(uint16_t i = 0; i < observerList.size(); i++)
		{
			EventObserver* currObs = observerList[i];
			if(currObs == observer)
			{
				observerList.erase(observerList.begin() + i);
				return;
			}
		}
	}

	// This will only broadcast to observers who are subscribed to the event categories
	virtual void Broadcast(const Event& event)
	{
		EventCategory eventCat = event.GetCategory();
		for(auto& obs : observerList)
		{
			if (obs->IsSubscribedToCategory(eventCat)) obs->OnEvent(event);
		}
	}

protected:

	std::vector<EventObserver*> observerList;


};

#endif