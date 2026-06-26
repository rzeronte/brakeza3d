#!/usr/bin/env python3
"""
fbx_diff.py -- Compara dos FBX binarios mostrando todas las strings distintas.

Uso:
  python fbx_diff.py A.fbx B.fbx
"""

import sys, struct, os, difflib

if hasattr(sys.stdout, 'reconfigure'):
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')


def find_string_props(data: bytes):
    """Extrae todas las propiedades tipo 'S' del FBX binario."""
    results = []
    i = 0
    while i < len(data) - 5:
        if data[i] == 0x53:  # 'S'
            length = struct.unpack_from('<I', data, i + 1)[0]
            if 1 <= length <= 4096:
                end = i + 5 + length
                if end <= len(data):
                    content = data[i + 5:end]
                    if all(0x20 <= b <= 0x7E or b in (0x09, 0x0A, 0x0D) for b in content):
                        try:
                            s = content.decode('utf-8')
                            results.append((i, length, s))
                        except Exception:
                            pass
        i += 1
    return results


def read_fbx(path):
    if not os.path.isfile(path):
        print(f"[ERROR] No encontrado: {path}"); sys.exit(1)
    with open(path, 'rb') as f:
        data = f.read()
    if data[:20] != b'Kaydara FBX Binary  ':
        print(f"[ERROR] No es FBX binario: {path}"); sys.exit(1)
    ver = struct.unpack_from('<I', data, 23)[0]
    return data, ver


def main():
    if len(sys.argv) != 3:
        print(__doc__); sys.exit(0)

    pathA, pathB = sys.argv[1], sys.argv[2]
    dataA, verA = read_fbx(pathA)
    dataB, verB = read_fbx(pathB)

    nameA = os.path.basename(pathA)
    nameB = os.path.basename(pathB)

    print(f"\n  {nameA}: {len(dataA):,} bytes  FBX {verA}")
    print(f"  {nameB}: {len(dataB):,} bytes  FBX {verB}")
    print(f"  Tamano: {'IGUAL' if len(dataA) == len(dataB) else 'DIFERENTE'}")
    print()

    stringsA = [s for (_, _, s) in find_string_props(dataA)]
    stringsB = [s for (_, _, s) in find_string_props(dataB)]

    setA = set(stringsA)
    setB = set(stringsB)

    only_in_A = sorted(setA - setB)
    only_in_B = sorted(setB - setA)
    common    = sorted(setA & setB)

    if only_in_A:
        print(f"  Solo en {nameA} ({len(only_in_A)}):")
        for s in only_in_A:
            print(f"    - {repr(s)}")
        print()

    if only_in_B:
        print(f"  Solo en {nameB} ({len(only_in_B)}):")
        for s in only_in_B:
            print(f"    + {repr(s)}")
        print()

    if not only_in_A and not only_in_B:
        print("  Strings identicas en ambos ficheros.")
        print()

    # Diff de orden/secuencia (mismas strings pero en distinto orden = distinta estructura)
    if stringsA != stringsB and only_in_A == [] and only_in_B == []:
        print("  Las strings son las mismas pero en DISTINTO ORDEN (estructura de nodos diferente).")
        print()

    # Diff unificado de secuencia completa
    diff = list(difflib.unified_diff(stringsA, stringsB, fromfile=nameA, tofile=nameB, lineterm=''))
    if diff:
        print(f"  Diff de secuencia completa de strings ({len(diff)} lineas):")
        for line in diff[:120]:
            print(f"    {line}")
        if len(diff) > 120:
            print(f"    ... ({len(diff) - 120} lineas mas)")
    else:
        print("  Secuencia de strings identica.")
    print()

    # Comparacion byte a byte
    if dataA == dataB:
        print("  FICHEROS IDENTICOS byte a byte.")
    else:
        diffs = [(i, dataA[i], dataB[i]) for i in range(min(len(dataA), len(dataB))) if dataA[i] != dataB[i]]
        print(f"  Bytes distintos: {len(diffs)}" + (f" (+ {abs(len(dataA)-len(dataB))} de diferencia en longitud)" if len(dataA) != len(dataB) else ""))
        if diffs:
            print(f"  Primeros 20 bytes diferentes:")
            for i, a, b in diffs[:20]:
                print(f"    offset 0x{i:06X}: {nameA}=0x{a:02X}({a:3d})  {nameB}=0x{b:02X}({b:3d})")
    print()


if __name__ == '__main__':
    main()
