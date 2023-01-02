#include "register_types.h"

#include "core/object/class_db.h"

#include "item.h"
#include "loot_table.h"
#include "inventory.h"
#include "slot.h"
#include "crafting_recipe.h"
ItemRegistry* item_registry;
void initialize_inventories_module(ModuleInitializationLevel p_level) {	
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<Item>();
	ClassDB::register_class<ItemData>();
	ClassDB::register_class<ItemRegistry>();
	item_registry = new ItemRegistry();
	Engine::get_singleton()->add_singleton(Engine::Singleton("ItemRegistry", item_registry, "ItemRegistry"));
	
	ClassDB::register_class<LootTable>();
	ClassDB::register_class<LootTableEntry>(true);
	ClassDB::register_class<LootTableEntryArray>();
	ClassDB::register_class<LootTableEntryConstant>();
	ClassDB::register_class<LootTableEntryRandomize>();
	
	ClassDB::register_class<Inventory>();

	ClassDB::register_class<AbstractSlot>();
	ClassDB::register_class<Slot>();
	ClassDB::register_class<InventorySlot>();

	ClassDB::register_class<SlotHelper>();

	ClassDB::register_class<CraftingRecipe>();
}

void uninitialize_inventories_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	Engine::get_singleton()->remove_singleton("ItemRegistry");
	delete item_registry;
	CraftingRecipe::unregister_hook();
}
