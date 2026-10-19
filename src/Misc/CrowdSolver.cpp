#include "../../include/Misc/CrowdSolver.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include "../../include/3D/Object3D.h"
#include "../../include/3D/Vector3D.h"

namespace {
constexpr float PI_F = 3.14159265358979f;
constexpr float MASS_BY_PRIORITY[CrowdSolver::CROWD_MAX_PRIORITY + 1] = { 1.0f, 4.0f, 16.0f, 64.0f, 256.0f, 1024.0f };
}

CrowdSolver::CrowdSolver()
{
    bucketStart.assign(BUCKETS + 1, 0);
    cursor.assign(BUCKETS, 0);
}

uint32_t CrowdSolver::bucketOf(int cx, int cz)
{
    return ((uint32_t) cx * 73856093u ^ (uint32_t) cz * 19349663u) & (BUCKETS - 1);
}

void CrowdSolver::add(Object3D *obj, float radius, int priority, uint32_t layer, uint32_t collidesWith)
{
    if (obj == nullptr) return;
    priority = std::clamp(priority, -1, CROWD_MAX_PRIORITY);
    if (obj->crowdSlot >= 0 && obj->crowdSlot < (int) agents.size() && agents[obj->crowdSlot].obj == obj) {
        Agent &a = agents[obj->crowdSlot];
        a.radius = radius; a.priority = priority; a.layer = layer; a.collidesWith = collidesWith;
        return;
    }
    obj->crowdSlot = (int) agents.size();
    agents.push_back({ obj, radius, priority, layer, collidesWith, 0.0f, 0.0f, 0.0f, false });
}

void CrowdSolver::remove(Object3D *obj)
{
    if (obj == nullptr) return;
    const int slot = obj->crowdSlot;
    if (slot < 0 || slot >= (int) agents.size() || agents[slot].obj != obj) { obj->crowdSlot = -1; return; }
    agents[slot] = agents.back();
    agents[slot].obj->crowdSlot = slot;
    agents.pop_back();
    obj->crowdSlot = -1;
}

void CrowdSolver::clear()
{
    for (auto &a : agents) a.obj->crowdSlot = -1;
    agents.clear();
}

void CrowdSolver::setPriority(Object3D *obj, int priority)
{
    if (obj == nullptr || obj->crowdSlot < 0 || obj->crowdSlot >= (int) agents.size()) return;
    agents[obj->crowdSlot].priority = std::clamp(priority, -1, CROWD_MAX_PRIORITY);
}

void CrowdSolver::setRadius(Object3D *obj, float radius)
{
    if (obj == nullptr || obj->crowdSlot < 0 || obj->crowdSlot >= (int) agents.size()) return;
    agents[obj->crowdSlot].radius = radius;
}

bool CrowdSolver::has(const Object3D *obj) const
{
    return obj != nullptr && obj->crowdSlot >= 0 && obj->crowdSlot < (int) agents.size() && agents[obj->crowdSlot].obj == obj;
}

void CrowdSolver::step(float dt)
{
    const auto t0 = std::chrono::steady_clock::now();
    stats = Stats{};
    if (!cfg.enabled || agents.empty() || dt <= 0.0f) return;

    // ── Agentes activos ────────────────────────────────────────────────────────────────────────
    work.clear();
    float maxRadius = 0.1f;
    for (Agent &a : agents) {
        if (a.obj->isRemoved() || !a.obj->isEnabled()) { a.hasLast = false; continue; }
        const Vertex3D &p = a.obj->getPosition();
        int prio = a.priority;
        if (prio < 0) {
            // Prioridad automática: lo que el script ha movido al agente desde donde lo dejó el solver
            float motion = 0.0f;
            if (a.hasLast) motion = std::sqrt((p.x - a.lastX) * (p.x - a.lastX) + (p.z - a.lastZ) * (p.z - a.lastZ)) / dt;
            a.stillT = (a.hasLast && motion < cfg.autoStillSpeed) ? a.stillT + dt : 0.0f;
            prio = a.stillT >= cfg.autoStillDelay ? cfg.autoStillPriority : cfg.autoMovingPriority;
            prio = std::clamp(prio, 0, CROWD_MAX_PRIORITY);
        }
        work.push_back({ a.obj, p.x, p.z, p.x, p.z, a.radius, MASS_BY_PRIORITY[prio], a.layer, a.collidesWith });
        maxRadius = std::max(maxRadius, a.radius);
    }
    const int n = (int) work.size();
    stats.agents = n;
    if (n < 2) return;

    // ── Hash espacial (ordenación por conteo). Celda = diámetro del mayor agente: un solape solo puede ocurrir con las 8
    //    celdas vecinas. Se reconstruye en cada pasada porque los agentes se mueven al corregirse. ─────────────
    const float cell = std::max(2.0f * maxRadius + cfg.margin, 0.5f);
    const float inv = 1.0f / cell;
    cellBucket.resize(n);
    order.resize(n);

    auto build = [&]() {
        std::fill(bucketStart.begin(), bucketStart.end(), 0);
        for (int i = 0; i < n; ++i) {
            const uint32_t b = bucketOf((int) std::floor(work[i].x * inv), (int) std::floor(work[i].z * inv));
            cellBucket[i] = b;
            ++bucketStart[b + 1];
        }
        for (uint32_t b = 0; b < BUCKETS; ++b) bucketStart[b + 1] += bucketStart[b];
        std::copy(bucketStart.begin(), bucketStart.begin() + BUCKETS, cursor.begin());
        for (int i = 0; i < n; ++i) order[cursor[cellBucket[i]]++] = (uint32_t) i;
    };

    const int iterations = std::max(1, cfg.iterations);
    for (int it = 0; it < iterations; ++it) {
        build();
        int corrected = 0;
        for (int i = 0; i < n; ++i) {
            Work &wi = work[i];
            const int cx = (int) std::floor(wi.x * inv), cz = (int) std::floor(wi.z * inv);
            // cubos distintos de las 9 celdas (dos celdas pueden compartir cubo: no visitar un par dos veces)
            uint32_t seen[9];
            int nSeen = 0;
            for (int dz = -1; dz <= 1; ++dz) {
                for (int dx = -1; dx <= 1; ++dx) {
                    const uint32_t b = bucketOf(cx + dx, cz + dz);
                    bool dup = false;
                    for (int s = 0; s < nSeen; ++s) if (seen[s] == b) { dup = true; break; }
                    if (dup) continue;
                    seen[nSeen++] = b;
                    for (uint32_t k = bucketStart[b]; k < bucketStart[b + 1]; ++k) {
                        const int j = (int) order[k];
                        if (j <= i) continue;       // cada par una vez
                        Work &wj = work[j];
                        if (!((wi.collidesWith & wj.layer) && (wj.collidesWith & wi.layer))) continue;
                        const float ddx = wi.x - wj.x, ddz = wi.z - wj.z;
                        const float minD = wi.radius + wj.radius + cfg.margin;
                        const float d2 = ddx * ddx + ddz * ddz;
                        if (d2 >= minD * minD) continue;
                        float nx, nz, d = std::sqrt(d2);
                        if (d < 1e-4f) {            // encima: dirección estable por pareja
                            const float ang = (float) (i * 7 + j * 13) * 2.399f;
                            nx = std::cos(ang); nz = std::sin(ang); d = 0.0f;
                        } else {
                            nx = ddx / d; nz = ddz / d;
                        }
                        const float overlap = (minD - d) * cfg.stiffness;
                        const float shareI = wj.mass / (wi.mass + wj.mass);
                        const float shareJ = wi.mass / (wi.mass + wj.mass);
                        wi.x += nx * overlap * shareI; wi.z += nz * overlap * shareI;
                        wj.x -= nx * overlap * shareJ; wj.z -= nz * overlap * shareJ;
                        ++corrected;
                    }
                }
            }
        }
        stats.pairs += corrected;
        if (corrected == 0) break;                  // ya no queda ningún solape
    }

    // ── Escritura: límite de corrección, comprobación de terreno y posición final ────────────────────
    const float cap = cfg.maxPushSpeed > 0.0f ? cfg.maxPushSpeed * dt : 1e9f;
    for (Work &w : work) {
        Agent &ag = agents[w.obj->crowdSlot];
        ag.lastX = w.ox; ag.lastZ = w.oz; ag.hasLast = true;     // por defecto, donde lo dejó el script
        float dx = w.x - w.ox, dz = w.z - w.oz;
        float len = std::sqrt(dx * dx + dz * dz);
        if (len < 1e-4f) continue;
        if (len > cap) { const float k = cap / len; dx *= k; dz *= k; len = cap; }
        float nx = w.ox + dx, nz = w.oz + dz;
        // Si el agente ya está en un sitio prohibido (nace ahí, o un script lo dejó allí) la corrección no puede empeorar nada:
        // se acepta, o se quedaría apilado para siempre. Solo se anula/desliza cuando sale de un sitio permitido a uno prohibido.
        if (walkable && !walkable(nx, nz) && walkable(w.ox, w.oz)) {
            // desliza por un solo eje antes de rendirse
            if (std::fabs(dx) > 1e-4f && walkable(nx, w.oz)) { nz = w.oz; len = std::fabs(dx); }
            else if (std::fabs(dz) > 1e-4f && walkable(w.ox, nz)) { nx = w.ox; len = std::fabs(dz); }
            else { ++stats.reverted; continue; }
        }
        const Vertex3D &p = w.obj->getPosition();
        w.obj->setPosition(Vertex3D(nx, p.y, nz));
        ag.lastX = nx; ag.lastZ = nz;
        ++stats.moved;
        stats.maxPush = std::max(stats.maxPush, len);
    }
    stats.stepMs = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - t0).count();
}

// ── Geometría de puestos ─────────────────────────────────────────────────────────────────────────────

std::vector<CrowdSolver::Slot> CrowdSolver::ringSlots(float cx, float cz, float radius, int count, float startAngle)
{
    std::vector<Slot> out;
    if (count <= 0) return out;
    out.reserve(count);
    for (int k = 0; k < count; ++k) {
        const float a = startAngle + 2.0f * PI_F * (float) k / (float) count;
        out.push_back({ cx + std::cos(a) * radius, cz + std::sin(a) * radius });
    }
    return out;
}

std::vector<CrowdSolver::Slot> CrowdSolver::packedSlots(float cx, float cz, int count, float spacing, float dirX, float dirZ)
{
    std::vector<Slot> out;
    if (count <= 0 || spacing <= 0.0f) return out;
    float len = std::sqrt(dirX * dirX + dirZ * dirZ);
    if (len < 1e-4f) { dirX = 0.0f; dirZ = 1.0f; len = 1.0f; }
    dirX /= len; dirZ /= len;
    const float sideX = -dirZ, sideZ = dirX;     // fila delantera: a lo largo de "side"

    struct Cand { float lx, lz, d2, ang; };
    std::vector<Cand> cands;
    const int R = (int) std::ceil(std::sqrt((float) count)) + 2;
    for (int r = -R; r <= R; ++r) {
        for (int q = -R; q <= R; ++q) {
            const float lx = spacing * ((float) q + 0.5f * (float) r);
            const float lz = spacing * (0.8660254f * (float) r);
            cands.push_back({ lx, lz, lx * lx + lz * lz, std::atan2(lz, lx) });
        }
    }
    std::sort(cands.begin(), cands.end(), [](const Cand &a, const Cand &b) {
        if (std::fabs(a.d2 - b.d2) > 1e-3f) return a.d2 < b.d2;
        return a.ang < b.ang;
    });
    out.reserve(count);
    for (int k = 0; k < count && k < (int) cands.size(); ++k) {
        out.push_back({ cx + sideX * cands[k].lx + dirX * cands[k].lz, cz + sideZ * cands[k].lx + dirZ * cands[k].lz });
    }
    return out;
}

std::vector<CrowdSolver::Slot> CrowdSolver::arcSlots(float tx, float tz, float fromX, float fromZ, float dist, float spacing, int count)
{
    std::vector<Slot> out;
    if (count <= 0 || spacing <= 0.0f) return out;
    dist = std::max(dist, 0.1f);
    float base = std::atan2(fromZ - tz, fromX - tx);
    if (std::fabs(fromX - tx) < 1e-4f && std::fabs(fromZ - tz) < 1e-4f) base = 0.0f;
    out.reserve(count);
    float r = dist;
    while ((int) out.size() < count) {
        const int perRing = std::max(1, (int) std::floor(2.0f * PI_F * r / spacing));
        const float step = 2.0f * PI_F / (float) perRing;
        // 0, +1, -1, +2, -2, ...
        for (int k = 0; k < perRing && (int) out.size() < count; ++k) {
            const int m = (k + 1) / 2;
            const float off = (k % 2 == 1 ? 1.0f : -1.0f) * (float) m * step;
            const float a = base + (k == 0 ? 0.0f : off);
            out.push_back({ tx + std::cos(a) * r, tz + std::sin(a) * r });
        }
        r += spacing;
    }
    return out;
}

std::vector<int> CrowdSolver::assignSlots(const std::vector<Slot> &from, const std::vector<Slot> &slots)
{
    const int n = (int) from.size(), m = (int) slots.size();
    std::vector<int> result(n, -1);
    if (n == 0 || m == 0) return result;

    struct Pair { float d2; int i, j; };
    std::vector<Pair> pairs;
    pairs.reserve((size_t) n * (size_t) m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            const float dx = from[i].x - slots[j].x, dz = from[i].z - slots[j].z;
            pairs.push_back({ dx * dx + dz * dz, i, j });
        }
    }
    std::sort(pairs.begin(), pairs.end(), [](const Pair &a, const Pair &b) { return a.d2 < b.d2; });
    std::vector<char> usedSlot(m, 0);
    int left = std::min(n, m);
    for (const Pair &p : pairs) {
        if (left == 0) break;
        if (result[p.i] >= 0 || usedSlot[p.j]) continue;
        result[p.i] = p.j;
        usedSlot[p.j] = 1;
        --left;
    }
    return result;
}
