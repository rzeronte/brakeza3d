#!/bin/bash

echo "================================================"
echo "  INSTALADOR BRAKEZA3D DOCS"
echo "================================================"
echo ""

# Verificar si estamos en un proyecto de Docusaurus
if [ ! -f "package.json" ]; then
    echo "❌ Error: No se encontró package.json"
    echo "   Asegúrate de ejecutar este script desde la raíz de tu proyecto Docusaurus"
    exit 1
fi

if ! grep -q "docusaurus" package.json; then
    echo "❌ Error: Este no parece ser un proyecto de Docusaurus"
    exit 1
fi

echo "✓ Proyecto Docusaurus detectado"
echo ""

# Pedir confirmación
read -p "¿Deseas continuar con la instalación? (s/n): " -n 1 -r
echo ""
if [[ ! $REPLY =~ ^[Ss]$ ]]; then
    echo "Instalación cancelada."
    exit 0
fi

echo ""
echo "Instalando archivos..."
echo ""

# Crear backup de docusaurus.config.js si existe
if [ -f "docusaurus.config.js" ]; then
    echo "📋 Creando backup de docusaurus.config.js..."
    cp docusaurus.config.js docusaurus.config.js.backup
    echo "   → Backup guardado como docusaurus.config.js.backup"
fi

# Copiar archivos de documentación
echo ""
echo "📚 Copiando documentación..."
cp *.md docs/ 2>/dev/null
if [ $? -eq 0 ]; then
    echo "   ✓ $(ls -1 *.md 2>/dev/null | wc -l) archivos copiados a docs/"
else
    echo "   ⚠ No se encontraron archivos .md para copiar"
fi

# Copiar configuración
echo ""
echo "⚙️  Copiando configuración..."
if [ -f "docusaurus.config.js" ]; then
    cp docusaurus.config.js .
    echo "   ✓ docusaurus.config.js actualizado"
fi

# Crear directorio de páginas si no existe
mkdir -p src/pages

# Copiar páginas personalizadas
echo ""
echo "📄 Copiando páginas personalizadas..."
if [ -f "gallery.jsx" ]; then
    cp gallery.jsx src/pages/
    echo "   ✓ gallery.jsx copiado"
fi
if [ -f "contact.jsx" ]; then
    cp contact.jsx src/pages/
    echo "   ✓ contact.jsx copiado"
fi

echo ""
echo "================================================"
echo "  ✅ INSTALACIÓN COMPLETADA"
echo "================================================"
echo ""
echo "Próximos pasos:"
echo ""
echo "1. Reinicia el servidor de desarrollo:"
echo "   npm start"
echo ""
echo "2. Visita las nuevas secciones:"
echo "   - Documentation: http://localhost:3000/docs/01-basics"
echo "   - Gallery: http://localhost:3000/gallery"
echo "   - Contact: http://localhost:3000/contact"
echo ""
echo "3. Personaliza tu sitio editando:"
echo "   - docusaurus.config.js (título, enlaces, etc.)"
echo "   - src/pages/gallery.jsx (imágenes de la galería)"
echo "   - src/pages/contact.jsx (información de contacto)"
echo ""
echo "¡Disfruta de tu documentación!"
echo ""
