#include "../../include/Components/ComponentsManager.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Render/Profiler.h"

ComponentsManager *ComponentsManager::instance = nullptr;

ComponentsManager *ComponentsManager::get()
{
    if (instance == nullptr) {
        instance = new ComponentsManager();
    }

    return instance;
}

void ComponentsManager::RegisterComponent(Component *component, const std::string& label)
{
    Logging::Message("[ComponentsManager] Register Profile measure for: %s", label.c_str());

    component->setId((int) components.size());
    component->setLabel(label);

    components.push_back(component);
    Profiler::get()->ResetMeasure(Profiler::get()->getComponentMeasures(), component->getLabel() + ProfilerConstants::SUFFIX_PRE);
    Profiler::get()->ResetMeasure(Profiler::get()->getComponentMeasures(),component->getLabel() + ProfilerConstants::SUFFIX_UPDATE);
    Profiler::get()->ResetMeasure(Profiler::get()->getComponentMeasures(), component->getLabel() + ProfilerConstants::SUFFIX_POST);
}

ComponentsManager::~ComponentsManager()
{
    for (auto component : components) {
        delete component;
    }
}