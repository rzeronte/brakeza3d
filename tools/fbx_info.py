#!/usr/bin/env python3
"""
fbx_info.py -- Inspecciona un FBX binario en formato tabla.

Uso:
  python fbx_info.py archivo.fbx [otro.fbx ...]
"""

import sys, re, os, struct, zlib

if hasattr(sys.stdout, 'reconfigure'):
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')

IMG_EXT_RE = re.compile(r'\.(png|jpg|jpeg|tga|bmp|tiff?)$', re.I)
DUP_RE     = re.compile(r'\.\d{3}(?:\.\w+)?$')

def read_fbx_array_length(data, pos):
    """
    Lee una propiedad array FBX binario en 'pos'.
    Formato: type(1) + array_len(4) + encoding(4) + comp_len(4) + data
    Devuelve (array_length, element_type, next_pos) o None si no es array.
    type: 'd'=float64, 'f'=float32, 'i'=int32, 'l'=int64
    """
    if pos >= len(data): return None
    t = data[pos]
    if t not in (0x64, 0x66, 0x69, 0x6C):  # d, f, i, l
        return None
    if pos + 13 > len(data): return None
    arr_len  = struct.unpack_from('<I', data, pos + 1)[0]
    encoding = struct.unpack_from('<I', data, pos + 5)[0]
    comp_len = struct.unpack_from('<I', data, pos + 9)[0]
    next_pos = pos + 13 + comp_len
    return arr_len, chr(t), next_pos


def find_geometry_stats(data):
    """
    Busca nodos Geometry en el FBX binario y extrae stats por mesh.
    Devuelve lista de dicts: {name, vertices, triangles, edges, polys}
    """
    meshes = []

    # Patrón: NameLen(1) + "Geometry" + ... luego propiedades de la geometría
    # Buscamos el nodo por su nombre en el stream de nodos FBX
    # Cada nodo: EndOff(4)+NumProps(4)+PropListLen(4)+NameLen(1)+Name(N)+Props
    HEADER = 27  # offset donde empiezan los nodos tras la cabecera FBX

    pos = HEADER
    data_len = len(data)

    # Búsqueda directa de sub-nodos por nombre en el stream binario.
    # Usamos regex sobre los bytes de nombre de nodo (prefijados por 1 byte de longitud).
    for m in re.finditer(rb'\x08Geometry', data):
        geo_start = m.start()

        # Buscar "Vertices", "PolygonVertexIndex", "Edges" dentro del bloque
        # que empieza en este offset (hasta el siguiente nodo Geometry o 200KB)
        block = data[geo_start : geo_start + 200_000]

        vertices  = 0
        triangles = 0
        edges     = 0

        # Vertices: nodo de nombre \x08Vertices seguido de array tipo 'd'
        for vm in re.finditer(rb'\x08Vertices', block):
            prop_pos = vm.end()
            result = read_fbx_array_length(block, prop_pos)
            if result and result[1] == 'd':
                vertices = result[0] // 3  # cada vertice = x,y,z
                break

        # PolygonVertexIndex: nodo \x13PolygonVertexIndex seguido de array 'i'
        for pm in re.finditer(rb'\x12PolygonVertexIndex', block):
            prop_pos = pm.end()
            result = read_fbx_array_length(block, prop_pos)
            if result and result[1] == 'i':
                # Indices negativos marcan fin de polígono
                # Si triangulado: cada 3er índice es negativo → triangles = len/3
                triangles = result[0] // 3
                break

        # Edges: nodo \x05Edges seguido de array 'i'
        for em in re.finditer(rb'\x05Edges', block):
            prop_pos = em.end()
            result = read_fbx_array_length(block, prop_pos)
            if result and result[1] == 'i':
                edges = result[0]
                break

        if vertices > 0 or triangles > 0:
            meshes.append({
                'vertices':  vertices,
                'triangles': triangles,
                'edges':     edges,
            })

    return meshes


def extract_strings(data):
    return [m.decode('ascii', 'replace') for m in re.findall(rb'[ -~]{3,}', data)]

def is_abs_path(s):
    return bool(re.match(r'^[A-Za-z]:[/\\]', s)) or s.startswith('/')

def strip_garbage(s):
    m = re.match(r'^([A-Za-z]:[/\\][^\x00"<>|?*]+?\.(?:png|jpg|jpeg|tga|bmp|tiff?))', s, re.I)
    if m: return m.group(1).replace('\\', '/')
    m2 = re.match(r'^([^/\\:*?"<>|\x00]+\.(?:png|jpg|jpeg|tga|bmp|tiff?))', s, re.I)
    if m2: return m2.group(1).replace('\\', '/')
    return s

def table(headers, rows, col_sep=" | "):
    widths = [len(h) for h in headers]
    for row in rows:
        for i, cell in enumerate(row):
            widths[i] = max(widths[i], len(str(cell)))
    fmt = col_sep.join(f"{{:<{w}}}" for w in widths)
    div = "-+-".join("-" * w for w in widths)
    print(fmt.format(*headers))
    print(div)
    for row in rows:
        print(fmt.format(*[str(c) for c in row]))

def analyze(path):
    if not os.path.isfile(path):
        print(f"[ERROR] No encontrado: {path}"); return

    with open(path, 'rb') as f:
        data = f.read()

    if data[:20] != b'Kaydara FBX Binary  ':
        print(f"[WARN] No es FBX binario: {path}"); return

    fbx_ver  = struct.unpack_from('<I', data, 23)[0]
    strings  = extract_strings(data)
    fbx_dir  = os.path.dirname(os.path.abspath(path))

    node_names, mat_names, abs_tex, rel_tex = [], [], [], []

    for i, s in enumerate(strings):
        if i > 0 and strings[i-1] in ('ModelL', 'ModelS', 'Model'):
            if 2 < len(s) < 80 and not any(c in s for c in '\\/:*?"<>|='):
                node_names.append(s)
        if i > 0 and strings[i-1] in ('MaterialL', 'MaterialS', 'Material'):
            if 2 < len(s) < 80 and not any(c in s for c in '\\/:*?"<>|='):
                mat_names.append(s)
        if IMG_EXT_RE.search(s) and is_abs_path(s):
            abs_tex.append(strip_garbage(s))
        if IMG_EXT_RE.search(s) and not is_abs_path(s) and len(s) < 200:
            cleaned = strip_garbage(s)
            if cleaned and '\\' not in cleaned[:2]:
                rel_tex.append(cleaned)

    def dedup(lst): return list(dict.fromkeys(lst))
    node_names = dedup(node_names)
    mat_names  = dedup(mat_names)
    abs_tex    = dedup(abs_tex)
    rel_tex    = dedup(rel_tex)

    geo_stats = find_geometry_stats(data)
    total_v = sum(g['vertices']  for g in geo_stats)
    total_t = sum(g['triangles'] for g in geo_stats)
    total_e = sum(g['edges']     for g in geo_stats)

    print()
    print(f"  Archivo : {os.path.basename(path)}   ({len(data):,} bytes   FBX {fbx_ver})")
    print(f"  Meshes  : {len(geo_stats)}   Vertices: {total_v:,}   Triangulos: {total_t:,}   Edges: {total_e:,}")
    print()

    # --- Geometria por mesh ---
    if len(geo_stats) > 1:
        geo_rows = [(i+1, g['vertices'], g['triangles'], g['edges'])
                    for i, g in enumerate(geo_stats)]
        table(["Mesh", "Vertices", "Triangulos", "Edges"], geo_rows)
        print()

    # --- Nodos + Materiales lado a lado ---
    max_rows = max(len(node_names), len(mat_names), 1)
    rows = []
    for i in range(max_rows):
        node = node_names[i] if i < len(node_names) else ""
        mat  = mat_names[i]  if i < len(mat_names)  else ""
        nflag = " !!" if DUP_RE.search(node) else ""
        mflag = " !!" if DUP_RE.search(mat)  else ""
        rows.append((node + nflag, mat + mflag))
    table(["Nodo / Modelo", "Material"], rows)

    # --- Texturas (agrupadas por basename para evitar duplicados rel/abs) ---
    print()

    # Construir mapa basename -> mejor ruta disponible en disco
    # Prioridad: abs en disco > rel en disco > rel sin disco > abs sin disco
    by_base = {}  # basename -> (kind, path, exists)

    def check_rel(p):
        full = os.path.join(fbx_dir, p.replace('/', os.sep))
        return os.path.isfile(full), full

    for p in rel_tex:
        exists, full = check_rel(p)
        base = os.path.basename(p.replace('\\', '/'))
        prev = by_base.get(base)
        # Reemplazar si: no había entrada, o la nueva existe y la anterior no
        if prev is None or (exists and not prev[2]):
            by_base[base] = ("rel", p, exists)

    for p in abs_tex:
        exists = os.path.isfile(p.replace('/', os.sep))
        base = os.path.basename(p.replace('\\', '/'))
        prev = by_base.get(base)
        if prev is None or (exists and not prev[2]):
            by_base[base] = ("abs", p, exists)

    tex_rows = []
    for base, (kind, p, exists) in by_base.items():
        status = "OK" if exists else "XX"
        dup    = "!!" if DUP_RE.search(os.path.splitext(base)[0]) else ""
        tex_rows.append((kind, status, dup, p))

    if tex_rows:
        table(["Tipo", "Disco", "Dup", "Ruta"], tex_rows)
    else:
        print("  (sin texturas detectadas)")

    # --- Problemas ---
    problems = []
    dup_mats = [m for m in mat_names if DUP_RE.search(m)]
    if dup_mats:
        problems.append(f"Materiales duplicados Blender: {', '.join(dup_mats)}")
    dup_tex = [base for base, (_, _, _) in by_base.items() if DUP_RE.search(os.path.splitext(base)[0])]
    if dup_tex:
        problems.append(f"Texturas con sufijo .001: {', '.join(dup_tex)}")
    missing = [base for base, (_, _, exists) in by_base.items() if not exists]
    if missing:
        problems.append(f"Texturas no encontradas en disco: {', '.join(missing)}")

    print()
    if problems:
        print(f"  !! {len(problems)} problema(s):")
        for p in problems: print(f"     - {p}")
    else:
        print("  OK Sin problemas")
    print()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__); sys.exit(0)
    for arg in sys.argv[1:]:
        analyze(arg)
