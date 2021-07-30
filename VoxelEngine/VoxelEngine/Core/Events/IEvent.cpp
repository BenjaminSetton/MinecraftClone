#include "../../Misc/pch.h"

#include "IEvent.h"

//////////////////////////////////////
///		EVENT OBSERVER
//////////////////////////////////////

EventObserver::EventObserver(const EventCategory categories) : 
	m_subscribedCategories(static_cast<uint16_t>(categories))
{
	GlobalSubject->Subscribe(this);
}

const uint16_t EventObserver::GetSubscribedCategories() const { return m_subscribedCategories; }

const bool EventObserver::IsSubscribedToCategory(EventCategory& cat) { return m_subscribedCategories & static_cast<uint16_t>(cat); }


//////////////////////////////////////
///		EVENT SUBJECT
//////////////////////////////////////

EventSubject::EventSubject()
{
	VX_LOG_INFO("EventSubject constructor called.");
	GlobalSubject = this;
}

EventSubject::~EventSubject()
{
	VX_LOG_INFO("EventSubject destructor called.");
	if (GlobalSubject) GlobalSubject = nullptr;
}

void EventSubject::Subscribe(EventObserver* observer) { observerList.push_back(observer); }

void EventSubject::Unsubscribe(EventObserver* observer)
{
	for (uint16_t i = 0; i < observerList.size(); i++)
	{
		EventObserver* currObs = observerList[i];
		if (currObs == observer)
		{
			observerList.erase(observerList.begin() + i);
			return;
		}
	}
}

void EventSubject::Broadcast(const Event& event)
{
	EventCategory eventCat = event.GetCategory();
	for (auto& obs : observerList)
	{
		if (obs->IsSubscribedToCategory(eventCat)) obs->OnEvent(event);
	}

}

