"""Run in Unreal Editor Python. Create missing item/texture assets; never overwrite existing ones."""
import unreal

tools = unreal.AssetToolsHelpers.get_asset_tools()
library = unreal.EditorAssetLibrary
definition_class = unreal.load_class(None, '/Script/Character_Lab.ARPGItemDefinition')
assert definition_class, 'Build the existing inventory classes before running this script.'

# Read the exact mesh already used by the character, without modifying or saving a map.
unreal.EditorLoadingAndSavingUtils.new_blank_map(False)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
character_class = library.load_blueprint_class('/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter')
character = actor_subsystem.spawn_actor_from_class(character_class, unreal.Vector())
mesh_component = character.get_editor_property('ChestArmorMesh')
assert mesh_component, 'Character chest component missing'
mesh = mesh_component.get_editor_property('skeletal_mesh_asset')
assert mesh, 'Character chest mesh missing'
unreal.log('ITEM_ASSETS_MESH ' + mesh.get_path_name())

definitions = [
    ('Chest', 'Chest Armor', 2, 3, True),
    ('Gem', 'Ädelsten', 1, 1, False),
    ('Scroll', 'Skriftrulle', 1, 2, False),
    ('Ore', 'Malmstycke', 2, 2, False),
    ('Sword', 'Svärd', 1, 3, False),
]
for key, display_name, width, height, equipped in definitions:
    texture_path = '/Game/UI/ItemIcons/T_' + key
    texture = library.load_asset(texture_path) if library.does_asset_exist(texture_path) else None
    if not texture:
        task = unreal.AssetImportTask()
        task.set_editor_property('filename', unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_content_dir() + 'UI/Icons/' + key + '.png'))
        task.set_editor_property('destination_path', '/Game/UI/ItemIcons')
        task.set_editor_property('destination_name', 'T_' + key)
        task.set_editor_property('automated', True)
        task.set_editor_property('save', False)
        task.set_editor_property('replace_existing', False)
        tools.import_asset_tasks([task])
        texture = library.load_asset(texture_path)
        assert texture, 'Texture import failed: ' + key
        texture.set_editor_property('lod_group', unreal.TextureGroup.TEXTUREGROUP_UI)
        texture.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_EDITOR_ICON)
        assert library.save_loaded_asset(texture), 'Texture save failed'
    asset_path = '/Game/Items/Definitions/DA_' + key
    if not library.does_asset_exist(asset_path):
        factory = unreal.DataAssetFactory()
        factory.set_editor_property('data_asset_class', definition_class)
        asset = tools.create_asset('DA_' + key, '/Game/Items/Definitions', definition_class, factory)
        assert asset, 'Data Asset creation failed: ' + key
        asset.set_editor_property('display_name', display_name)
        asset.set_editor_property('size', unreal.IntPoint(width, height))
        brush = asset.get_editor_property('icon')
        brush.set_editor_property('resource_object', texture)
        brush.set_editor_property('draw_as', unreal.SlateBrushDrawType.IMAGE)
        asset.set_editor_property('icon', brush)
        if equipped:
            # Read enum type from the reflected property, avoiding Python prefix assumptions.
            slot_type = type(asset.get_editor_property('equipment_slot'))
            asset.set_editor_property('equipment_slot', slot_type.CHEST)
            asset.set_editor_property('equipped_mesh', mesh)
        assert library.save_loaded_asset(asset), 'Data Asset save failed: ' + key
    asset = library.load_asset(asset_path)
    assert asset.get_editor_property('icon').get_editor_property('resource_object'), 'Icon missing'
    size = asset.get_editor_property('size')
    assert size.x > 0 and size.y > 0
    unreal.log('ITEM_ASSET_OK ' + asset.get_path_name() + ' ' + str(size.x) + 'x' + str(size.y))
actor_subsystem.destroy_actor(character)
unreal.log('ITEM_ASSETS_COMPLETE')
