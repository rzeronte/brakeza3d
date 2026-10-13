-- ============================================================================
-- Tutorial 16: Updating a Shader from a Script
-- Type: Global Script
-- ============================================================================
-- Change the values (uniforms) of a post-processing shader while the scene runs.
--
-- The scene has a code shader called "Custom" with three float variables (r_factor, g_factor, a_factor).
-- Every frame this script looks it up by its label and writes new values:
--   Components:Render():getSceneShaderByLabel("Custom")
--   shader:setDataTypeValue("r_factor", value)
-- The shader (assets/shaders/Examples/CodePostprocessing/CustomTint.fs) mixes them into a colour tint.
-- ============================================================================

local warned = false

function onStart()
    warned = false
end

function onUpdate()
    -- Scene shaders load in the background: until "Custom" exists we simply wait
    local customShader = Components:Render():getSceneShaderByLabel("Custom")

    if customShader ~= nil then
        local t = Brakeza:getExecutionTime()
        customShader:setDataTypeValue("r_factor", math.sin(t))
        customShader:setDataTypeValue("g_factor", math.cos(t))
        customShader:setDataTypeValue("a_factor", math.cos(t * 0.5))
    elseif not warned then
        warned = true
        print("[16_CustomShaderUpdate] waiting for a scene shader labelled 'Custom'")
    end
end

function onEnd()
end
