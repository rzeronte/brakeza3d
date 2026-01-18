#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script para extraer archivos modificados y nuevos de una instalación MinGW personalizada
comparándola con una instalación limpia.
"""

import os
import sys
import shutil
import hashlib
from pathlib import Path
from datetime import datetime

def calcular_hash(filepath):
    """Calcula el hash SHA256 de un archivo."""
    sha256_hash = hashlib.sha256()
    try:
        with open(filepath, "rb") as f:
            for byte_block in iter(lambda: f.read(4096), b""):
                sha256_hash.update(byte_block)
        return sha256_hash.hexdigest()
    except Exception as e:
        print(f"⚠️  Error calculando hash de {filepath}: {e}")
        return None

def comparar_directorios(dir_original, dir_modificado, dir_salida):
    """
    Compara dos directorios y extrae archivos nuevos o modificados.

    Args:
        dir_original: Ruta del MinGW limpio
        dir_modificado: Ruta del MinGW personalizado
        dir_salida: Carpeta donde guardar los cambios
    """

    dir_original = Path(dir_original).resolve()
    dir_modificado = Path(dir_modificado).resolve()
    dir_salida = Path(dir_salida).resolve()

    # Verificar que existen los directorios
    if not dir_original.exists():
        print(f"❌ Error: No existe el directorio original: {dir_original}")
        sys.exit(1)

    if not dir_modificado.exists():
        print(f"❌ Error: No existe el directorio modificado: {dir_modificado}")
        sys.exit(1)

    # Crear directorio de salida
    dir_salida.mkdir(parents=True, exist_ok=True)

    print(f"📁 Directorio original: {dir_original}")
    print(f"📁 Directorio modificado: {dir_modificado}")
    print(f"📁 Directorio de salida: {dir_salida}")
    print(f"\n{'='*60}\n")

    archivos_nuevos = []
    archivos_modificados = []
    errores = []

    # Recorrer todos los archivos del directorio modificado
    print("🔍 Analizando archivos...")

    for root, dirs, files in os.walk(dir_modificado):
        root_path = Path(root)

        # Calcular la ruta relativa
        try:
            ruta_relativa = root_path.relative_to(dir_modificado)
        except ValueError:
            continue

        for filename in files:
            archivo_modificado = root_path / filename
            archivo_original = dir_original / ruta_relativa / filename

            # Verificar si el archivo es nuevo
            if not archivo_original.exists():
                archivos_nuevos.append(archivo_modificado)
                print(f"➕ NUEVO: {ruta_relativa / filename}")
            else:
                # Comparar tamaños (ignorar cambios solo de fecha/metadata)
                try:
                    tamano_original = archivo_original.stat().st_size
                    tamano_modificado = archivo_modificado.stat().st_size

                    if tamano_original != tamano_modificado:
                        archivos_modificados.append(archivo_modificado)
                        print(f"✏️  MODIFICADO: {ruta_relativa / filename} (tamaño: {tamano_original} → {tamano_modificado} bytes)")
                    # Si pesa lo mismo, ignorarlo (probablemente solo cambió la fecha)

                except Exception as e:
                    print(f"⚠️  Error comparando {ruta_relativa / filename}: {e}")
                    errores.append(archivo_modificado)

    print(f"\n{'='*60}\n")
    print(f"📊 RESUMEN:")
    print(f"   • Archivos nuevos: {len(archivos_nuevos)}")
    print(f"   • Archivos modificados: {len(archivos_modificados)}")
    print(f"   • Errores: {len(errores)}")
    print(f"\n{'='*60}\n")

    # Copiar archivos al directorio de salida
    if archivos_nuevos or archivos_modificados:
        print("💾 Copiando archivos...")

        todos_archivos = archivos_nuevos + archivos_modificados

        for archivo in todos_archivos:
            try:
                ruta_relativa = archivo.relative_to(dir_modificado)
                archivo_destino = dir_salida / ruta_relativa

                # Crear directorios necesarios
                archivo_destino.parent.mkdir(parents=True, exist_ok=True)

                # Copiar archivo
                shutil.copy2(archivo, archivo_destino)
                print(f"   ✅ {ruta_relativa}")

            except Exception as e:
                print(f"   ❌ Error copiando {archivo}: {e}")
                errores.append(archivo)

        # Crear archivo de registro
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        archivo_log = dir_salida / f"cambios_mingw_{timestamp}.txt"

        with open(archivo_log, 'w', encoding='utf-8') as f:
            f.write(f"Extracción de cambios MinGW\n")
            f.write(f"Fecha: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"{'='*60}\n\n")

            f.write(f"Directorio original: {dir_original}\n")
            f.write(f"Directorio modificado: {dir_modificado}\n")
            f.write(f"Directorio de salida: {dir_salida}\n\n")

            f.write(f"{'='*60}\n\n")
            f.write(f"ARCHIVOS NUEVOS ({len(archivos_nuevos)}):\n")
            f.write(f"{'-'*60}\n")
            for archivo in archivos_nuevos:
                ruta_rel = archivo.relative_to(dir_modificado)
                f.write(f"{ruta_rel}\n")

            f.write(f"\n{'='*60}\n\n")
            f.write(f"ARCHIVOS MODIFICADOS ({len(archivos_modificados)}):\n")
            f.write(f"{'-'*60}\n")
            for archivo in archivos_modificados:
                ruta_rel = archivo.relative_to(dir_modificado)
                f.write(f"{ruta_rel}\n")

            if errores:
                f.write(f"\n{'='*60}\n\n")
                f.write(f"ERRORES ({len(errores)}):\n")
                f.write(f"{'-'*60}\n")
                for archivo in errores:
                    f.write(f"{archivo}\n")

        print(f"\n📝 Log guardado en: {archivo_log}")
        print(f"\n✅ ¡Proceso completado!")
        print(f"   Archivos guardados en: {dir_salida}")
    else:
        print("ℹ️  No se encontraron diferencias.")

def main():
    """Función principal."""
    print("\n" + "="*60)
    print("  EXTRACTOR DE CAMBIOS MinGW")
    print("="*60 + "\n")

    # Solicitar rutas si no se pasan por argumentos
    if len(sys.argv) >= 4:
        dir_original = sys.argv[1]
        dir_modificado = sys.argv[2]
        dir_salida = sys.argv[3]
    else:
        print("Introduce las rutas de los directorios:\n")
        dir_original = input("📁 MinGW original (limpio): ").strip().strip('"')
        dir_modificado = input("📁 MinGW modificado (tuyo): ").strip().strip('"')
        dir_salida = input("📁 Carpeta de salida (para guardar cambios): ").strip().strip('"')
        print()

    comparar_directorios(dir_original, dir_modificado, dir_salida)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n⚠️  Proceso cancelado por el usuario.")
        sys.exit(0)
    except Exception as e:
        print(f"\n❌ Error inesperado: {e}")
        sys.exit(1)