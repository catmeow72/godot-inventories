#ifndef CRAFTING_RECIPE_H
#define CRAFTING_RECIPE_H

#include "core/templates/vector.h"
#include "core/variant/typed_array.h"
#include "core/string/ustring.h"
#include "core/string/string_name.h"
#include "core/io/resource.h"
#include "core/variant/binder_common.h"
#include "item.h"
#include "inventory.h"

class CraftingRecipe : public Resource {
    GDCLASS(CraftingRecipe, Resource);

protected:
    static void _bind_methods();
    Vector<Ref<Item>> inputs;
    Ref<Item> output;
    static Vector<Ref<CraftingRecipe>> recipes;

public:
    TypedArray<Item> get_inputs() const;
    Ref<Item> get_output() const;
    void set_output(Ref<Item>);

    void _get_property_list(List<PropertyInfo> *r_props) const;
    bool _get(const StringName &p_property, Variant &r_value) const;
    bool _set(const StringName &p_property, const Variant &p_value);

    bool craftable(Ref<Inventory> inventory) const;
    Ref<Item> craft(Ref<Inventory> inventory) const;
    bool take_inputs(Ref<Inventory> inventory) const;

    static TypedArray<CraftingRecipe> all_craftable(Ref<Inventory> inventory);
    static TypedArray<CraftingRecipe> all_registered();
    static void register_recipe(Ref<CraftingRecipe> recipe);

    static void unregister_hook();

    CraftingRecipe();
    ~CraftingRecipe();
};

#endif
