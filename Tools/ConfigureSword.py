"""Run in Unreal Python after building/restarting. Authors assets, never changes maps.

TEMP: simple prototype art; replace WeaponMesh in DA_Sword with final weapon art.
Existing mesh/materials are preserved. Only DA_Sword's equipment fields are configured.
"""
import unreal

library = unreal.EditorAssetLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
mesh_path = '/Game/Weapons/SM_Sword_Prototype'

if not library.does_asset_exist(mesh_path):
    options = unreal.FbxImportUI()
    options.set_editor_property('import_mesh', True)
    options.set_editor_property('import_as_skeletal', False)
    options.set_editor_property('import_materials', False)
    options.set_editor_property('import_textures', False)
    options.set_editor_property('automated_import_should_detect_type', False)
    options.set_editor_property('mesh_type_to_import', unreal.FBXImportType.FBXIT_STATIC_MESH)
    data = options.get_editor_property('static_mesh_import_data')
    data.set_editor_property('combine_meshes', True)
    data.set_editor_property('auto_generate_collision', False)
    data.set_editor_property('convert_scene', False)
    data.set_editor_property('normal_import_method', unreal.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS)
    task = unreal.AssetImportTask()
    task.set_editor_property('filename', unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_content_dir() + 'Weapons/Source/SM_Sword_Prototype.obj'))
    task.set_editor_property('destination_path', '/Game/Weapons')
    task.set_editor_property('destination_name', 'SM_Sword_Prototype')
    task.set_editor_property('automated', True)
    task.set_editor_property('save', False)
    task.set_editor_property('replace_existing', False)
    task.set_editor_property('factory', unreal.FbxFactory())
    task.set_editor_property('options', options)
    tools.import_asset_tasks([task])
    mesh = library.load_asset(mesh_path)
    assert isinstance(mesh, unreal.StaticMesh), 'Sword mesh import failed'
    colors = {'Blade': (0.6, 0.68, 0.76), 'Guard': (0.5, 0.29, 0.07), 'Grip': (0.055, 0.025, 0.015)}
    for index, slot in enumerate(mesh.get_editor_property('static_materials')):
        name = str(slot.get_editor_property('material_slot_name'))
        assert name in colors, 'Unexpected material slot: ' + name
        path = '/Game/Weapons/M_Sword_' + name
        material = library.load_asset(path) if library.does_asset_exist(path) else None
        if not material:
            material = tools.create_asset('M_Sword_' + name, '/Game/Weapons', unreal.Material, unreal.MaterialFactoryNew())
            color = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionConstant3Vector)
            color.set_editor_property('constant', unreal.LinearColor(*colors[name], 1.0))
            unreal.MaterialEditingLibrary.connect_material_property(color, '', unreal.MaterialProperty.MP_BASE_COLOR)
            metallic = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionConstant)
            metallic.set_editor_property('r', 0.0 if name == 'Grip' else 0.75)
            unreal.MaterialEditingLibrary.connect_material_property(metallic, '', unreal.MaterialProperty.MP_METALLIC)
            unreal.MaterialEditingLibrary.recompile_material(material)
            assert library.save_loaded_asset(material)
        mesh.set_material(index, material)
    assert library.save_loaded_asset(mesh)

mesh = library.load_asset(mesh_path)
bounds = mesh.get_bounding_box()
assert bounds.is_valid and abs(bounds.max.z - 78.0) < 0.1 and abs(bounds.min.z + 9.0) < 0.1, 'Unexpected sword axes or scale'
assert len(mesh.get_editor_property('static_materials')) == 3, 'Sword material slots missing'
for slot in mesh.get_editor_property('static_materials'):
    assert slot.get_editor_property('material_interface'), 'Sword material reference missing'
unreal.log('SWORD_MESH_OK {} bounds={}'.format(mesh.get_path_name(), bounds))
definition = library.load_asset('/Game/Items/Definitions/DA_Sword')
assert definition, 'DA_Sword is missing'
slot_type = type(definition.get_editor_property('equipment_slot'))
if not hasattr(slot_type, 'WEAPON'):
    unreal.log_warning('SWORD_SETUP_PENDING: Build C++, restart Unreal, then run Tools/ConfigureSword.py again.')
else:
    # Native socket already present on Human_Male_rigged; no runtime fallback or rig edits.
    definition.set_editor_property('weapon_mesh', mesh)
    definition.set_editor_property('attachment_socket', 'HandGrip_R')
    definition.set_editor_property('attachment_transform', unreal.Transform())
    definition.set_editor_property('equipment_slot', slot_type.WEAPON)
    assert library.save_loaded_asset(definition), 'DA_Sword save failed'
    unreal.log('SWORD_SETUP_COMPLETE')
