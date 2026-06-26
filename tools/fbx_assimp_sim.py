#!/usr/bin/env python3
"""
fbx_assimp_sim.py -- Simula como Assimp ve los materiales de un FBX binario.

Muestra:
  - Lista de materiales en orden (como los ve Assimp)
  - Indice de material asignado a cada mesh
  - Conexiones textura->material
  - Diagnostico de posible index out-of-bounds

Uso:
  python fbx_assimp_sim.py archivo.fbx [otro.fbx ...]
"""

import sys, struct, re, os

if hasattr(sys.stdout, 'reconfigure'):
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')

IMG_EXT_RE = re.compile(rb'\.(png|jpg|jpeg|tga|bmp|tiff?)$', re.I)


def read_uint32(data, pos):
    return struct.unpack_from('<I', data, pos)[0]

def read_uint64(data, pos):
    return struct.unpack_from('<Q', data, pos)[0]

def read_int32(data, pos):
    return struct.unpack_from('<i', data, pos)[0]


class FBXNode:
    def __init__(self, name, props, children, offset):
        self.name = name
        self.props = props       # list of (type_char, value)
        self.children = children # list of FBXNode
        self.offset = offset


def parse_property(data, pos):
    """Parsea una propiedad FBX en 'pos'. Devuelve (type, value, next_pos)."""
    if pos >= len(data):
        return None
    t = chr(data[pos])
    pos += 1

    if t == 'Y':  # int16
        v = struct.unpack_from('<h', data, pos)[0]; return t, v, pos+2
    if t == 'C':  # bool
        v = data[pos] != 0; return t, v, pos+1
    if t == 'I':  # int32
        v = struct.unpack_from('<i', data, pos)[0]; return t, v, pos+4
    if t == 'F':  # float32
        v = struct.unpack_from('<f', data, pos)[0]; return t, v, pos+4
    if t == 'D':  # float64
        v = struct.unpack_from('<d', data, pos)[0]; return t, v, pos+8
    if t == 'L':  # int64
        v = struct.unpack_from('<q', data, pos)[0]; return t, v, pos+8
    if t == 'S':  # string
        length = read_uint32(data, pos); pos += 4
        v = data[pos:pos+length].decode('utf-8', 'replace'); return t, v, pos+length
    if t == 'R':  # raw
        length = read_uint32(data, pos); pos += 4
        return t, data[pos:pos+length], pos+length
    # Arrays: f, d, i, l, b
    if t in ('f', 'd', 'i', 'l', 'b'):
        arr_len  = read_uint32(data, pos)
        encoding = read_uint32(data, pos+4)
        comp_len = read_uint32(data, pos+8)
        return t, arr_len, pos+12+comp_len
    return None


def parse_nodes(data, start, end, depth=0):
    """Parsea nodos FBX binario recursivamente."""
    nodes = []
    pos = start
    while pos < end:
        # FBX 7.x: EndOffset(4) + NumProps(4) + PropListLen(4) + NameLen(1) + Name
        if pos + 13 > end:
            break
        end_offset  = read_uint32(data, pos)
        num_props   = read_uint32(data, pos+4)
        # prop_list_len = read_uint32(data, pos+8)  # unused
        name_len    = data[pos+12]
        name        = data[pos+13:pos+13+name_len].decode('utf-8', 'replace')
        pos += 13 + name_len

        if end_offset == 0:
            break  # sentinel null record

        # Propiedades
        props = []
        for _ in range(num_props):
            result = parse_property(data, pos)
            if result is None:
                break
            t, v, pos = result
            props.append((t, v))

        # Hijos (entre pos y end_offset - 13)
        children = []
        if pos < end_offset - 13:
            children = parse_nodes(data, pos, end_offset - 13, depth+1)

        nodes.append(FBXNode(name, props, children, end_offset))
        pos = end_offset

    return nodes


def find_nodes(nodes, name):
    """Busca todos los nodos con ese nombre (no recursivo en primer nivel)."""
    return [n for n in nodes if n.name == name]


def find_nodes_recursive(nodes, name):
    result = []
    for n in nodes:
        if n.name == name:
            result.append(n)
        result.extend(find_nodes_recursive(n.children, name))
    return result


def get_string_prop(node, index=0):
    count = 0
    for t, v in node.props:
        if t == 'S':
            if count == index:
                return v
            count += 1
    return None

def get_int_prop(node, index=0):
    count = 0
    for t, v in node.props:
        if t in ('I', 'L', 'Y'):
            if count == index:
                return v
            count += 1
    return None


def analyze(path):
    print(f"\n{'='*70}")
    print(f"  {os.path.basename(path)}")
    print(f"{'='*70}")

    with open(path, 'rb') as f:
        data = f.read()

    if data[:20] != b'Kaydara FBX Binary  ':
        print("  [ERROR] No es FBX binario"); return

    ver = read_uint32(data, 23)
    print(f"  FBX version: {ver}   Tamano: {len(data):,} bytes")

    # Parsear el arbol completo
    try:
        root_nodes = parse_nodes(data, 27, len(data))
    except Exception as e:
        print(f"  [ERROR] Parseo fallido: {e}"); return

    # --- Buscar Objects ---
    objects_list = find_nodes(root_nodes, 'Objects')
    if not objects_list:
        print("  [WARN] No se encontro nodo 'Objects'"); return
    objects = objects_list[0]

    # --- Materiales ---
    materials = find_nodes(objects.children, 'Material')
    print(f"\n  Materiales ({len(materials)}) — en orden de aparicion (= indice Assimp):")
    mat_id_to_idx = {}
    for idx, mat in enumerate(materials):
        # Prop 0: UID (L), Prop 1: nombre (S), Prop 2: tipo (S)
        uid  = mat.props[0][1] if mat.props else '?'
        name_raw = mat.props[1][1] if len(mat.props) > 1 else '?'
        # Nombre en FBX: "Nombre\x00\x01Material"
        mat_name = name_raw.split('\x00')[0]
        shading = '?'
        for child in mat.children:
            if child.name == 'ShadingModel':
                shading = get_string_prop(child) or '?'
                break
        mat_id_to_idx[uid] = idx
        print(f"    [{idx}] UID={uid}  nombre='{mat_name}'  shading={shading}")

    # --- Geometrias (meshes) ---
    geometries = find_nodes(objects.children, 'Geometry')
    print(f"\n  Geometrias ({len(geometries)}):")
    geo_id_to_name = {}
    for geo in geometries:
        uid  = geo.props[0][1] if geo.props else '?'
        name_raw = geo.props[1][1] if len(geo.props) > 1 else '?'
        geo_name = name_raw.split('\x00')[0]
        geo_id_to_name[uid] = geo_name

        # contar vertices y faces
        vcount = 0; fcount = 0
        for child in geo.children:
            if child.name == 'Vertices':
                for t, v in child.props:
                    if t in ('d','f'): vcount = v // 3; break
            if child.name == 'PolygonVertexIndex':
                for t, v in child.props:
                    if t in ('i','l'): fcount = v // 3; break
        print(f"    UID={uid}  nombre='{geo_name}'  vertices~{vcount}  triangulos~{fcount}")

    # --- Modelos (nodos de escena) ---
    models = find_nodes(objects.children, 'Model')
    print(f"\n  Modelos ({len(models)}):")
    model_id_to_name = {}
    for m in models:
        uid  = m.props[0][1] if m.props else '?'
        name_raw = m.props[1][1] if len(m.props) > 1 else '?'
        mod_name = name_raw.split('\x00')[0]
        model_id_to_name[uid] = mod_name
        print(f"    UID={uid}  nombre='{mod_name}'")

    # --- Texturas ---
    textures = find_nodes(objects.children, 'Texture')
    print(f"\n  Texturas ({len(textures)}):")
    tex_id_to_path = {}
    for tex in textures:
        uid = tex.props[0][1] if tex.props else '?'
        filename = ''; relname = ''
        for child in tex.children:
            if child.name == 'FileName':
                filename = get_string_prop(child) or ''
            if child.name == 'RelativeFilename':
                relname = get_string_prop(child) or ''
        tex_id_to_path[uid] = relname or filename
        print(f"    UID={uid}  rel='{relname}'  abs='{filename}'")

    # --- Conexiones ---
    connections_list = find_nodes(root_nodes, 'Connections')
    conns = []
    if connections_list:
        for c in connections_list[0].children:
            if c.name == 'C':
                kind = c.props[0][1] if c.props else ''
                src  = c.props[1][1] if len(c.props) > 1 else 0
                dst  = c.props[2][1] if len(c.props) > 2 else 0
                prop = c.props[3][1] if len(c.props) > 3 else ''
                conns.append((kind, src, dst, prop))

    print(f"\n  Conexiones ({len(conns)} total) — relevantes material/textura:")
    for kind, src, dst, prop in conns:
        src_is_mat = src in mat_id_to_idx
        src_is_tex = src in tex_id_to_path
        dst_is_mat = dst in mat_id_to_idx
        if src_is_mat or src_is_tex or dst_is_mat:
            src_desc = f"Mat[{mat_id_to_idx.get(src,'?')}] '{[m.props[1][1].split(chr(0))[0] for m in materials if m.props and m.props[0][1]==src][0] if src_is_mat else ''}'" if src_is_mat \
                  else (f"Tex '{tex_id_to_path.get(src,'?')}'" if src_is_tex else f"UID={src}")
            dst_desc = f"Mat[{mat_id_to_idx.get(dst,'?')}]" if dst_is_mat else \
                       (model_id_to_name.get(dst, geo_id_to_name.get(dst, f"UID={dst}")))
            print(f"    {kind}  {src_desc}  ->  {dst_desc}  prop='{prop}'")

    # --- Diagnostic: material index usado por cada mesh ---
    print(f"\n  DIAGNOSTICO — Indice de material por layer de cada Geometry:")
    for geo in geometries:
        uid = geo.props[0][1] if geo.props else '?'
        name_raw = geo.props[1][1] if len(geo.props) > 1 else '?'
        geo_name = name_raw.split('\x00')[0]
        for child in geo.children:
            if child.name == 'LayerElementMaterial':
                mat_idx = None
                for sub in child.children:
                    if sub.name == 'Materials':
                        for t, v in sub.props:
                            if t == 'i': mat_idx = f"array({v} elementos)"; break
                            if t in ('I','L','Y'): mat_idx = v; break
                mapping = ''
                for sub in child.children:
                    if sub.name == 'MappingInformationType':
                        mapping = get_string_prop(sub) or ''
                    if sub.name == 'ReferenceInformationType':
                        pass
                print(f"    Geo '{geo_name}': MaterialIndex={mat_idx}  Mapping={mapping}")

    print(f"\n  VEREDICTO:")
    num_mats = len(materials)
    print(f"    Assimp vera {num_mats} material(es) -> modelTextures tendra {num_mats} entradas")
    print(f"    Indices validos: 0..{num_mats-1}")

    # Buscar si alguna conexion Geometry->Material implica indice > num_mats-1
    # (Assimp asigna materialIndex = posicion del material en la lista global)
    geo_mat_connections = []
    for kind, src, dst, prop in conns:
        if src in mat_id_to_idx and dst in geo_id_to_name:
            geo_mat_connections.append((geo_id_to_name[dst], mat_id_to_idx[src]))
        if dst in mat_id_to_idx and src in geo_id_to_name:
            geo_mat_connections.append((geo_id_to_name[src], mat_id_to_idx[dst]))

    # Conexiones modelo->material
    model_mat_connections = []
    for kind, src, dst, prop in conns:
        if src in mat_id_to_idx and dst in model_id_to_name:
            model_mat_connections.append((model_id_to_name[dst], mat_id_to_idx[src]))

    if model_mat_connections:
        print(f"    Conexiones Modelo->Material:")
        for mname, midx in model_mat_connections:
            ok = midx < num_mats
            print(f"      '{mname}' usa Material[{midx}] -> {'OK' if ok else 'OUT OF BOUNDS!'}")
    else:
        print(f"    No se encontraron conexiones Modelo->Material directas")
    print()


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(__doc__); sys.exit(0)
    for arg in sys.argv[1:]:
        analyze(arg)
