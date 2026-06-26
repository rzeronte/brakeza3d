#!/usr/bin/env python3
"""
fbx_rewrite_paths.py -- Reescribe rutas de texturas en un FBX binario.

Estrategias disponibles:
  --basename   Reemplaza todas las rutas por solo el nombre de fichero
               (compatible con el engine que busca en assets/textures/)
  --fix001     Elimina sufijos .001/.002 de Blender en nombres de textura
  --remap OLD NEW  Reemplaza una ruta concreta por otra

El reemplazo mantiene el mismo numero de bytes (padding con ./ al inicio)
para no romper los offsets del FBX binario.

Uso:
  python fbx_rewrite_paths.py CITY.fbx --basename
  python fbx_rewrite_paths.py CITY.fbx --fix001
  python fbx_rewrite_paths.py CITY.fbx --remap "FACADE37.png.001" "FACADE37.png"
  python fbx_rewrite_paths.py CITY.fbx --basename --fix001 -o CITY_fixed.fbx
"""

import sys, re, os, struct, shutil, argparse

if hasattr(sys.stdout, 'reconfigure'):
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')

IMG_EXT_RE = re.compile(rb'\.(png|jpg|jpeg|tga|bmp|tiff?)$', re.I)
DUP_RE     = re.compile(r'\.\d{3}$')


def pad_to_length(new_path: str, target_len: int) -> bytes:
    """
    Ajusta new_path a exactamente target_len bytes.
    Si es mas corto, antepone './' repetidos para que el basename sea correcto.
    Si es mas largo, lanza error.
    """
    new_bytes = new_path.encode('utf-8')
    if len(new_bytes) == target_len:
        return new_bytes
    if len(new_bytes) > target_len:
        raise ValueError(
            f"Nuevo path '{new_path}' ({len(new_bytes)}b) es mas largo "
            f"que el original ({target_len}b). No se puede hacer in-place."
        )
    # Necesitamos (target_len - len(new_bytes)) bytes de padding delante
    pad_needed = target_len - len(new_bytes)
    # Construir prefijo con './' repetidos + '/' final para que basename funcione
    # Ejemplo: pad_needed=10 → './././././'  (5 x './')
    prefix = (b'./' * ((pad_needed + 1) // 2))[:pad_needed - 1] + b'/'
    assert len(prefix) == pad_needed - 1 + 1  # = pad_needed
    result = prefix + new_bytes
    assert len(result) == target_len, f"Bug: {len(result)} != {target_len}"
    return result


def find_string_props(data: bytes):
    """
    Busca todas las propiedades string en FBX binario.
    Formato: tipo 'S' (0x53) + uint32_le(longitud) + bytes
    Devuelve lista de (offset_del_S, longitud, contenido_str)
    """
    results = []
    i = 0
    while i < len(data) - 5:
        if data[i] == 0x53:  # 'S'
            length = struct.unpack_from('<I', data, i + 1)[0]
            if 1 <= length <= 1024:
                end = i + 5 + length
                if end <= len(data):
                    content = data[i + 5:end]
                    # Solo si parece texto ASCII imprimible
                    if all(0x20 <= b <= 0x7E or b in (0x09, 0x0A, 0x0D) for b in content):
                        try:
                            s = content.decode('utf-8')
                            results.append((i, length, s))
                        except Exception:
                            pass
        i += 1
    return results


def compute_replacements(strings, strategies, custom_remaps):
    """
    Devuelve lista de (offset_S, old_str, new_str) a aplicar.
    """
    replacements = []

    for (offset, length, s) in strings:
        new_s = s

        # --remap: reemplazos manuales exactos
        for old_r, new_r in custom_remaps:
            if old_r in new_s:
                new_s = new_s.replace(old_r, new_r)

        # --fix001: elimina sufijos .001/.002 ANTES de la extension (solo en rutas de imagen)
        if 'fix001' in strategies and IMG_EXT_RE.search(new_s.encode()):
            # FACADE37.png.001 -> FACADE37.png
            new_s = re.sub(r'\.\d{3}(\.\w+)$', r'\1', new_s)
            # FACADE_37.001 (sin extension al final) -> FACADE_37
            new_s = re.sub(r'\.\d{3}$', '', new_s)

        # --basename: queda solo el nombre de fichero
        if 'basename' in strategies and IMG_EXT_RE.search(new_s.encode()):
            new_s = os.path.basename(new_s.replace('\\', '/'))

        if new_s != s:
            replacements.append((offset, s, new_s))

    return replacements


def apply_replacements(data: bytearray, replacements):
    """
    Aplica los reemplazos en el bytearray in-place.
    Cada reemplazo mantiene el mismo numero de bytes usando pad_to_length.
    """
    applied = []
    errors  = []

    for (offset, old_s, new_s) in replacements:
        old_bytes = old_s.encode('utf-8')
        target_len = len(old_bytes)

        # Verificar que el contenido actual sigue siendo el esperado
        current = data[offset + 5 : offset + 5 + target_len]
        if current != old_bytes:
            errors.append(f"Offset 0x{offset:X}: contenido ya no coincide, saltando")
            continue

        try:
            new_bytes = pad_to_length(new_s, target_len)
        except ValueError as e:
            errors.append(str(e))
            continue

        data[offset + 5 : offset + 5 + target_len] = new_bytes
        applied.append((old_s, new_s))

    return applied, errors


def main():
    parser = argparse.ArgumentParser(description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('fbx', help='Fichero FBX de entrada')
    parser.add_argument('-o', '--output', help='Fichero de salida (default: input_fixed.fbx)')
    parser.add_argument('--basename', action='store_true',
                        help='Reducir todas las rutas de textura a solo el nombre de fichero')
    parser.add_argument('--fix001', action='store_true',
                        help='Eliminar sufijos .001/.002 de Blender')
    parser.add_argument('--remap', nargs=2, metavar=('OLD', 'NEW'), action='append',
                        default=[], help='Reemplazar OLD por NEW en rutas (repetible)')
    parser.add_argument('--dry-run', action='store_true',
                        help='Mostrar cambios sin escribir fichero')
    args = parser.parse_args()

    if not os.path.isfile(args.fbx):
        print(f"[ERROR] No encontrado: {args.fbx}"); sys.exit(1)

    with open(args.fbx, 'rb') as f:
        raw = f.read()

    if raw[:20] != b'Kaydara FBX Binary  ':
        print("[ERROR] No es FBX binario"); sys.exit(1)

    strategies = []
    if args.basename: strategies.append('basename')
    if args.fix001:   strategies.append('fix001')
    if not strategies and not args.remap:
        print("[ERROR] Indica al menos una estrategia: --basename, --fix001 o --remap")
        sys.exit(1)

    strings      = find_string_props(raw)
    replacements = compute_replacements(strings, strategies, args.remap)

    if not replacements:
        print("  Sin cambios necesarios.")
        sys.exit(0)

    print(f"\n  Cambios detectados ({len(replacements)}):")
    for (_, old_s, new_s) in replacements:
        print(f"    - '{old_s}'")
        print(f"    + '{new_s}'")

    if args.dry_run:
        print("\n  [dry-run] Fichero no modificado.")
        sys.exit(0)

    data = bytearray(raw)
    applied, errors = apply_replacements(data, replacements)

    for e in errors:
        print(f"  [WARN] {e}")

    out = args.output or os.path.splitext(args.fbx)[0] + '_fixed.fbx'
    with open(out, 'wb') as f:
        f.write(data)

    print(f"\n  Aplicados {len(applied)} reemplazo(s) -> {out}")


if __name__ == '__main__':
    main()
