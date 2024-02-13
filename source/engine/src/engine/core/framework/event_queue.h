#pragma once

namespace vct {

class EventListener;
class Scene;

enum EventType {
    EVENT_WINDOW_RESIZE,
    EVENT_SCENE_CHANGED,
};

class Event {
public:
    Event(EventType type) : m_type(type) {}

    virtual ~Event() = default;

protected:
    const EventType m_type;
};

class SceneChangeEvent : public Event {
public:
    SceneChangeEvent(Scene* scene) : Event(EVENT_SCENE_CHANGED), m_scene(scene) {}

    const Scene* get_scene() const { return m_scene; }

protected:
    Scene* m_scene;
};

class ResizeEvent : public Event {
public:
    ResizeEvent(int width, int height) : Event(EVENT_WINDOW_RESIZE), m_width(width), m_height(height) {}

    int get_width() const { return m_width; }
    int get_height() const { return m_height; }

protected:
    int m_width;
    int m_height;
};

class EventListener {
public:
    virtual ~EventListener() {}
    virtual void event_received(std::shared_ptr<Event> event) = 0;
};

class EventQueue {
public:
    void queue_event(std::shared_ptr<Event> event) {
        m_events.push(event);
    }

    void dispatch_event(std::shared_ptr<Event> event) {
        for (auto& listener : m_listeners) {
            listener->event_received(event);
        }
    }

    void flush_events() {
        while (!m_events.empty()) {
            auto event = m_events.front();
            dispatch_event(event);
        }
    }

    void register_listener(EventListener* listener) {
        m_listeners.push_back(listener);
    }

    void unregister_listener(EventListener* listener) {
        unused(listener);
        CRASH_NOW_MSG("TODO");
    }

private:
    std::queue<std::shared_ptr<Event>> m_events;
    std::vector<EventListener*> m_listeners;
};

}  // namespace vct
