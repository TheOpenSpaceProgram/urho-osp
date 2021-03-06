#include <Urho3D/Core/Context.h>
#include <Urho3D/Container/Ptr.h>

#include "ActiveArea.h"
#include "AstronomicalBody.h"
#include "../Terrain/PlanetTerrain.h"

namespace osp
{

Node* AstronomicalBody::load(ActiveArea* area, const Vector3& pos)
{
    Satellite::load(area, pos);

    Node* scene = area->get_active_node();

    m_activeNode = scene->CreateChild(m_name);
    PlanetTerrain* terrain = m_activeNode->CreateComponent<PlanetTerrain>();
    terrain->initialize(this);

    LongVector3 relativePos = Satellite::calculate_relative_position(
                                        area, this, m_precision);


    Vector3 floatPos;
    floatPos.x_ = float(relativePos.x_) / (1 << m_precision);
    floatPos.y_ = float(relativePos.y_) / (1 << m_precision);
    floatPos.z_ = float(relativePos.z_) / (1 << m_precision);

    URHO3D_LOGINFOF("Center at: %f %f %f", floatPos.x_,
                    floatPos.y_, floatPos.z_);

    m_activeNode->SetPosition(floatPos);

    return m_activeNode.Get();
}

} // namespace osp
