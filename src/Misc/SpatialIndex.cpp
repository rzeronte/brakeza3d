#include "../../include/Misc/SpatialIndex.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>
#include "../../include/3D/Object3D.h"
#include "../../include/3D/Vector3D.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"

SpatialIndex::SpatialIndex()
{
    bucketStart.assign(BUCKETS + 1, 0);
}

int SpatialIndex::cellOf(float v) const
{
    return (int) std::floor(v / cellSize);
}

uint32_t SpatialIndex::bucket(int cx, int cz)
{
    return ((uint32_t) cx * 73856093u ^ (uint32_t) cz * 19349663u) & (BUCKETS - 1);
}

void SpatialIndex::setMask(Object3D *obj, uint32_t mask)
{
    if (obj == nullptr) return;
    std::lock_guard lock(membersMutex);
    obj->spatialMask = mask;
    if (mask != 0 && obj->spatialSlot < 0) {
        obj->spatialSlot = (int) members.size();
        members.push_back(obj);
    } else if (mask == 0 && obj->spatialSlot >= 0) {
        const int slot = obj->spatialSlot;
        members[slot] = members.back();
        members[slot]->spatialSlot = slot;
        members.pop_back();
        obj->spatialSlot = -1;
    }
    builtFrame = UINT64_MAX;   // altas/bajas visibles ya en la siguiente consulta de este frame
}

void SpatialIndex::unregister(Object3D *obj)
{
    setMask(obj, 0);
}

void SpatialIndex::setCellSize(float size)
{
    if (size <= 0.0f) return;
    cellSize = size;
    builtFrame = UINT64_MAX;
}

int SpatialIndex::size() const
{
    std::lock_guard lock(membersMutex);
    return (int) members.size();
}

void SpatialIndex::ensureBuilt()
{
    const uint64_t frame = Brakeza::get()->getFrameIndex();
    if (frame == builtFrame) return;
    builtFrame = frame;
    rebuild();
}

void SpatialIndex::rebuild()
{
    scratch.clear();
    {
        std::lock_guard lock(membersMutex);
        for (Object3D *o : members) {
            if (o->isRemoved()) continue;
            const Vertex3D &p = o->getPosition();
            scratch.push_back({p.x, p.y, p.z, cellOf(p.x), cellOf(p.z), o->spatialMask, o->getId(), o->isEnabled()});
        }
    }

    // Ordenación por conteo: contar por cubo, prefijos, colocar
    std::fill(bucketStart.begin(), bucketStart.end(), 0);
    for (const Entry &e : scratch) bucketStart[bucket(e.cx, e.cz) + 1]++;
    for (uint32_t i = 0; i < BUCKETS; i++) bucketStart[i + 1] += bucketStart[i];

    entries.resize(scratch.size());
    cursor.assign(bucketStart.begin(), bucketStart.end() - 1);
    for (const Entry &e : scratch) entries[cursor[bucket(e.cx, e.cz)]++] = e;
}

template<typename F>
void SpatialIndex::forEachInRadius(float x, float z, float r, uint32_t mask, bool enabledOnly, F &&fn)
{
    ensureBuilt();
    if (entries.empty() || r < 0.0f) return;

    const float r2 = r * r;
    auto test = [&](const Entry &e) {
        if ((e.mask & mask) == 0) return;
        if (enabledOnly && !e.enabled) return;
        const float dx = e.x - x, dz = e.z - z;
        const float d2 = dx * dx + dz * dz;
        if (d2 <= r2) fn(e, d2);
    };

    // Radio enorme respecto a lo indexado: recorrer todo es más barato que visitar celdas vacías.
    // En float y antes de cellOf(): con r = math.huge (consulta "todos") el cast a int desbordaría.
    const float span = 2.0f * r / cellSize + 2.0f;
    if (!(span * span < (float) entries.size())) {
        for (const Entry &e : entries) test(e);
        return;
    }

    const int cx0 = cellOf(x - r), cx1 = cellOf(x + r);
    const int cz0 = cellOf(z - r), cz1 = cellOf(z + r);

    for (int cx = cx0; cx <= cx1; cx++) {
        for (int cz = cz0; cz <= cz1; cz++) {
            const uint32_t b = bucket(cx, cz);
            for (uint32_t i = bucketStart[b]; i < bucketStart[b + 1]; i++) {
                const Entry &e = entries[i];
                if (e.cx == cx && e.cz == cz) test(e);   // otras celdas que comparten cubo, fuera
            }
        }
    }
}

bool SpatialIndex::nearest(float x, float z, float r, uint32_t mask, unsigned int excludeId, bool enabledOnly, Hit &out)
{
    bool found = false;
    forEachInRadius(x, z, r, mask, enabledOnly, [&](const Entry &e, float d2) {
        if (e.id == excludeId) return;
        if (!found || d2 < out.d2 || (d2 == out.d2 && e.id < out.id)) {
            out = {e.id, e.x, e.y, e.z, d2};
            found = true;
        }
    });
    return found;
}

const std::vector<SpatialIndex::Hit> &SpatialIndex::query(float x, float z, float r, uint32_t mask, int maxResults, bool sorted, bool enabledOnly)
{
    hits.clear();
    forEachInRadius(x, z, r, mask, enabledOnly, [&](const Entry &e, float d2) {
        hits.push_back({e.id, e.x, e.y, e.z, d2});
    });

    // Desempate por id: mismo orden en cada ejecución
    auto closer = [](const Hit &a, const Hit &b) { return a.d2 < b.d2 || (a.d2 == b.d2 && a.id < b.id); };
    const bool limited = maxResults > 0 && (size_t) maxResults < hits.size();
    if (sorted) {
        if (limited) {
            std::partial_sort(hits.begin(), hits.begin() + maxResults, hits.end(), closer);
        } else {
            std::sort(hits.begin(), hits.end(), closer);
        }
    }
    if (limited) hits.resize(maxResults);
    return hits;
}

int SpatialIndex::count(float x, float z, float r, uint32_t mask, bool enabledOnly)
{
    int n = 0;
    forEachInRadius(x, z, r, mask, enabledOnly, [&](const Entry &, float) { n++; });
    return n;
}

void SpatialIndex::drawDebug(float r, float g, float b, float a)
{
    ensureBuilt();
    if (entries.empty()) return;

    std::vector<Vector3D> lines;
    std::unordered_set<int64_t> drawnCells;
    constexpr float LIFT = 0.2f;
    constexpr float CROSS = 0.5f;
    auto seg = [&lines](Vertex3D from, Vertex3D to) { lines.emplace_back(from, to); };   // Vector3D pide lvalues
    for (const Entry &e : entries) {
        const float y = e.y + LIFT;
        const int64_t key = ((int64_t) e.cx << 32) ^ (uint32_t) e.cz;
        if (drawnCells.insert(key).second) {
            const float x0 = (float) e.cx * cellSize, x1 = x0 + cellSize;
            const float z0 = (float) e.cz * cellSize, z1 = z0 + cellSize;
            seg(Vertex3D(x0, y, z0), Vertex3D(x1, y, z0));
            seg(Vertex3D(x1, y, z0), Vertex3D(x1, y, z1));
            seg(Vertex3D(x1, y, z1), Vertex3D(x0, y, z1));
            seg(Vertex3D(x0, y, z1), Vertex3D(x0, y, z0));
        }
        seg(Vertex3D(e.x - CROSS, y, e.z), Vertex3D(e.x + CROSS, y, e.z));
        seg(Vertex3D(e.x, y, e.z - CROSS), Vertex3D(e.x, y, e.z + CROSS));
    }
    Components::get()->Render()->getShaders()->shaderOGLLine3D->renderLines(
        lines, Components::get()->Window()->getForegroundFramebuffer(), Color(r, g, b, a));
}
