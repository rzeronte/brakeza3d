# New Entity Type

Add a new entity type to Brakeza3D following the full checklist.

The new entity name is: **$ARGUMENTS**

## Checklist

1. **Read** `include/3D/Object3D.h` and a similar entity (e.g. `include/3D/Image3D.h`) to understand the base pattern.

2. **Create entity class**:
   - `include/3D/$ARGUMENTS.h` — subclass `Object3D`
   - `src/3D/$ARGUMENTS.cpp` — implement constructor, `onUpdate()`, `Draw()`

3. **Create serializer**:
   - `include/Serializers/$ARGUMENTS Serializer.h` — subclass `JSONSerializer<$ARGUMENTS>`
   - `src/Serializers/$ARGUMENTS Serializer.cpp` — implement `to_json()` and `from_json()`

4. **Register serializer** in `src/Render/JSONSerializerRegistry.cpp`

5. **Create ThreadJob** for async loading:
   - `include/Threads/ThreadJobLoad$ARGUMENTS.h`

6. **Add GUI inspector** in `src/GUI/Objects/` — follow the pattern of existing inspector files

7. **Add to scene hierarchy panel** — `src/GUI/AddOns/GUIAddonObjects3D.cpp` (type filter + icon)

8. **Add icon** to `include/GUI/IconsGUI.h` if needed

At each step, read the equivalent file for an existing similar entity before writing the new one.
