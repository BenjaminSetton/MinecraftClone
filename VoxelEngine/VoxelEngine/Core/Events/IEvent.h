#ifndef _IEVENT_H
#define _IEVENT_H

#include <vector>

// Forward declare the Event class
class Event;

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

	virtual void Broadcast(const Event& event) = 0;

protected:

	std::vector<EventObserver*> observerList;
};

// Observer is able to subscribe and unsubscribe from subjects' messages
class EventObserver
{
public:


	virtual void OnEvent(const Event& event) = 0;
};

#endif