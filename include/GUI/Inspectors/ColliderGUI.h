/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by darkh on 05/02/2026.
//

#ifndef BRAKEZA3D_COLLIDERGUI_H
#define BRAKEZA3D_COLLIDERGUI_H

class Object3D;

class ColliderGUI {
    
public:
    static void DrawColliderWorldVariables(Object3D *o);
    static void DrawPopUpColliderSetting(Object3D * object_3d);
};


#endif //BRAKEZA3D_COLLIDERGUI_H