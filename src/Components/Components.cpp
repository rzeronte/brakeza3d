#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Render/Profiler.h"

Components *Components::instance = nullptr;

Components *Components::get()
{
    if (instance == nullptr) {
        instance = new Components();
    }

    return instance;
}

void Components::RegisterComponent(Component *component, const std::string& label)
{
    Logging::Message("[Components] Register Profiler measure for: %s", label.c_str());

    component->setId((int) components.size());
    component->setLabel(label);

    components.push_back(component);
    Profiler::InitMeasure(Profiler::get()->getComponentMeasures(), component->getLabel() + ProfilerConstants::SUFFIX_PRE);
    Profiler::InitMeasure(Profiler::get()->getComponentMeasures(), component->getLabel() + ProfilerConstants::SUFFIX_UPDATE);
    Profiler::InitMeasure(Profiler::get()->getComponentMeasures(), component->getLabel() + ProfilerConstants::SUFFIX_POST);
}

Components::~Components()
{
    for (auto component : components) {
        delete component;
    }
}