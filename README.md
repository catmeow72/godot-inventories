# WARNING
This module is ***ALPHA*** quality! It relies on the latest Godot 4.0 beta and is completely untested on the stable version of Godot!

**Here be dragons!**

# Godot Inventories module
A Godot 4.0 module that adds everything needed for items to the engine.

Allows custom behavior for items and slots using virtual methods.

## Features
 - Items
 - Slots
 - Script-driven item behavior
 - Crafting recipes
 - Loot tables

## Limitations so far.
Items always use a single texture atlas in PNG format in a predefined location - This will be changed!
Item scripts must be instantiated before registration

## How to use this module in a project
First of all, this is a Godot module that **must** be built as part of the engine.

 - See [the Compiling section of the Godot docs](https://docs.godotengine.org/en/latest/development/compiling/index.html), particularly [this section on custom modules](https://docs.godotengine.org/en/latest/development/compiling/introduction_to_the_buildsystem.html#custom-modules).

Once you are running a build of the engine with this module, you can follow these steps 
 1. Create the texture atlas for the items - It must be at `res://texture-atlas.png`!
 2. Write item scripts extending `ItemData`. They can be in any language, as long as they can be instantiated before registering!
 3. Register the items before any items are loaded. 
   - Use the `ItemData.register(StringName id, ItemData data)` static function to register the items.
   - Make sure to instantiate the scripts with the .new() function!
 4. Add an inventory instance somewhere in the project that can be used by inventory slots.
   - Make sure to set the `size` property or the inventory will not work!
 5. Add a container to hold the inventory slots and create those slots, hopefully by a script.
   - The `slot_id` and `inventory` properties need to be set.
 6. Once you start adding slots, add a SlotHelper node
   - This should be in a CanvasLayer node with a higher priority than the slots themselves so that the slot tooltip (when hovering over a slot) and any items being moved show above the slots at all times.
 7. Create a node that will use the item(s) associated with it.
 8. If you want crafting recipes, create crafting recipe resources and register them with the static `CraftingRecipe.register(CraftingRecipe recipe)` function.
 9. Loot tables can be used like normal resources. Just call the `get_output()` function on the LootTable resource to get the output of the resource.

## Items
 - Null items should be considered empty.
   - `Item.is_empty_or_null(Item item)` takes the possibility of a null item into account when checking, so this function should be preferred over `Item.is_empty()`.
   - Empty items should be handled as if they are null.
 - Item IDs are represented by the `Item.id` property.
 - Item counts are represented by the `Item.count` property.
 - Do not register items with the ID of `empty`. This ID is reserved for empty items.
 - To clone an item, use the `Item.clone()` method.

## Crafting recipes
 - Craftable recipes can be queried using the `CraftingRecipes.all_craftable(Inventory inventory)` and `CraftingRecipes.all_registered()` static functions.
 - If a crafting recipe has been crafted by cloning the output directly without using the `CraftingRecipe.craft(Inventory inventory)` function, use the `CraftingRecipe.take_inputs(Inventory inventory)` function to take the inputs of the crafting recipe.
