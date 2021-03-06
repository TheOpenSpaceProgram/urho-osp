#include <Urho3D/IO/Log.h>

#include "Machine.h"

using namespace osp;
using namespace Urho3D;

void Machine::OnSceneSet(Scene* scene)
{
    if (scene)
    {
        if (ActiveArea* area = reinterpret_cast<ActiveArea*>(
                                    scene->GetVar("ActiveArea").GetPtr()))
        {
            // Scene has an ActiveArea
            URHO3D_LOGINFOF("Loaded in active area");

            m_area = area;
            m_state = E_ACTIVE;
            loaded_active();
        }
        else if (scene->HasTag("EDITOR"))
        {
            URHO3D_LOGINFOF("Loaded in editor");
            m_area = nullptr;
            m_state = E_EDITOR;
            loaded_editor();
        }
    }
    else
    {
        // scene is null
        m_state = E_NOTLOADED;
        m_area = nullptr;
    }


    //if (scene->HasComponent<ActiveArea>())
    //{

    //}
}

void Machine::OnNodeSetEnabled(Node* node)
{
    URHO3D_LOGINFOF("NODE ENABLED");
}

WireInput* Machine::get_wire_input(const String &name)
{
    StringHash nameHash(name);

    // Loop through all WireInputs and return the matching name
    // maybe optimize this some day
    for (int i = 0; i < m_wireInputs.Size(); i ++)
    {
        if (m_wireInputs[i]->get_nameHash() == nameHash)
        {
            return m_wireInputs[i];
        }
    }
    return nullptr;
}

WireOutput* Machine::get_wire_output(const String &name)
{
    StringHash nameHash(name);
    // Loop through all WireOutputs and return the matching name
    // maybe optimize this some day
    for (int i = 0; i < m_wireOutputs.Size(); i ++)
    {
        if (m_wireOutputs[i]->get_nameHash() == nameHash)
        {
            return m_wireOutputs[i];
        }
    }
    return nullptr;
}

/*void Machine::resubscribe()
{
    switch (m_state)
    {
    case E_NOTLOADED:
        // Remove some subscriptions
        //if )
        break;
    case E_ACTIVE:
        SubscribeToEvent(this, E_PHYSICSPRESTEP,
                         URHO3D_HANDLER(Machine, update_active));
        break;
    case E_EDITOR:

        break;

    }

}*/
