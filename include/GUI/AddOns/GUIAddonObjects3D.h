//
// Created by Eduardo on 06/12/2025.
// Header for GUIAddonObjects3D with CustomTreeNode support
//

#ifndef GUIADDONOBJECTS3D_H
#define GUIADDONOBJECTS3D_H

#include <string>
#include <vector>
#include "../../../include/GUI/GUIManager.h"

class GUIAddonObjects3D {
public:
    // ========================================================================
    // FUNCIONES PRINCIPALES
    // ========================================================================

    /**
     * Dibuja la ventana principal de objetos de la escena
     */
    static void DrawWinSceneObjects(GUIManager *gui);

    /**
     * Dibuja los botones de filtro de tipos de objetos (Lights, Meshes, etc.)
     */
    static void DrawObjectTypes(GUIManager *gui);

    /**
     * Dibuja los objetos en modo árbol (agrupados por tipo)
     */
    static void DrawObjectsTree(GUIManager *gui, const std::vector<Object3D *> &objects, const std::string &filter);

    /**
     * Dibuja los objetos en modo lista (todos juntos)
     */
    static void DrawObjectList(GUIManager *gui, std::vector<Object3D *> &objects, const std::string &filter);

    // ========================================================================
    // NUEVA FUNCIÓN - CustomTreeNode
    // ========================================================================

    /**
     * Dibuja un objeto individual usando CustomTreeNode
     * Con iconos clickables, drag&drop, selección y checkbox
     *
     * @param o Objeto a dibujar
     * @param index Índice del objeto en la lista (para el label)
     */
    static void DrawObjectWithCustomNode(Object3D* o, int index);

    // ========================================================================
    // FUNCIONES AUXILIARES
    // ========================================================================

    /**
     * Verifica si un tipo de objeto está visible según los filtros activos
     */
    static bool isObjectTypeVisible(GUIManager *gui, ObjectType typeObject);

    /**
     * Busca un patrón en un string (case-insensitive)
     * Para el sistema de filtrado
     */
    static bool exist(const std::string &pattern1, const std::string &pattern2);

    // ========================================================================
    // FUNCIONES LEGACY (ya no se usan con CustomTreeNode, pero las dejo)
    // ========================================================================

    /**
     * @deprecated Use DrawObjectWithCustomNode instead
     * Antigua función para dibujar items en tabla
     */
    // static void DrawItem(int i, Object3D* o, bool icon);
};

#endif // GUIADDONOBJECTS3D_H