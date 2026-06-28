#!/usr/bin/env python
"""
fbx_bounds.py -- Extrae las dimensiones (AABB) de los meshes de un FBX binario.

Uso:
    python tools/fbx_bounds.py <archivo.fbx> [archivo2.fbx ...]

Salida por mesh:
    X[min, max]  Y[min, max]  Z[min, max]  verts=N

Para calcular dimensiones en world space con posicion (px,py,pz) y escala s:
    worldMin = pos + localMin * scale
    worldMax = pos + localMax * scale
    worldSize = (localMax - localMin) * scale

Ejemplo (LocalFloor01, pos=512,512, scale=1024):
    localX [-0.04160, 0.04160] → worldX [469.4, 554.6]  (~85u)
    localZ [-0.02960, 0.05360] → worldZ [481.7, 566.9]  (~85u)
"""

import struct
import re
import sys
import zlib


def extract_bounds(data, hit):
    """Extrae AABB del bloque de vertices encontrado en 'hit'."""
    prop_start = hit + len(b'Vertices')
    ptype = data[prop_start]
    if ptype not in (0x64, 0x66):
        return None
    arr_len  = struct.unpack_from('<I', data, prop_start + 1)[0]
    encoding = struct.unpack_from('<I', data, prop_start + 5)[0]
    comp_len = struct.unpack_from('<I', data, prop_start + 9)[0]
    raw      = data[prop_start + 13 : prop_start + 13 + comp_len]
    if encoding == 1:
        raw = zlib.decompress(raw)
    fmt   = 'd' if ptype == 0x64 else 'f'
    verts = struct.unpack_from('<' + fmt * arr_len, raw)
    xs = verts[0::3]
    ys = verts[1::3]
    zs = verts[2::3]
    return {
        'minX': min(xs), 'maxX': max(xs),
        'minY': min(ys), 'maxY': max(ys),
        'minZ': min(zs), 'maxZ': max(zs),
        'n': arr_len // 3,
    }


def fbx_bounds(path):
    """Devuelve lista de dicts con AABB por mesh."""
    with open(path, 'rb') as f:
        data = f.read()
    hits = [m.start() for m in re.finditer(b'Vertices', data)]
    results = []
    for h in hits:
        b = extract_bounds(data, h)
        if b:
            results.append(b)
    return results


def world_bounds(local, pos, scale):
    """Convierte bounds locales a world space dado pos=(px,pz) y scale escalar."""
    px, pz = pos
    return {
        'worldMinX': px + local['minX'] * scale,
        'worldMaxX': px + local['maxX'] * scale,
        'worldMinZ': pz + local['minZ'] * scale,
        'worldMaxZ': pz + local['maxZ'] * scale,
        'worldSizeX': (local['maxX'] - local['minX']) * scale,
        'worldSizeZ': (local['maxZ'] - local['minZ']) * scale,
    }


if __name__ == '__main__':
    paths = sys.argv[1:]
    if not paths:
        print(__doc__)
        sys.exit(0)

    for path in paths:
        print('=== %s ===' % path)
        results = fbx_bounds(path)
        if not results:
            print('  (no se encontraron datos de vertices)')
            continue
        for i, b in enumerate(results):
            print('  mesh%d: X[%.5f, %.5f]  Y[%.5f, %.5f]  Z[%.5f, %.5f]  verts=%d' % (
                i, b['minX'], b['maxX'], b['minY'], b['maxY'], b['minZ'], b['maxZ'], b['n']))
            sx = b['maxX'] - b['minX']
            sz = b['maxZ'] - b['minZ']
            print('         sizeX=%.5f  sizeZ=%.5f' % (sx, sz))
