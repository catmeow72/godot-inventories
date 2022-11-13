#include "crafting_recipe.h"

void CraftingRecipe::_bind_methods() {
    ClassDB::bind_static_method("CraftingRecipe", D_METHOD("all_craftable", "inventory"), &CraftingRecipe::all_craftable);
    ClassDB::bind_static_method("CraftingRecipe", D_METHOD("register", "recipe"), &CraftingRecipe::register_recipe);

    ClassDB::bind_method(D_METHOD("get_inputs"), &CraftingRecipe::get_inputs);

    ClassDB::bind_method(D_METHOD("get_output"), &CraftingRecipe::get_output);
    ClassDB::bind_method(D_METHOD("set_output", "output"), &CraftingRecipe::set_output);

    ClassDB::bind_method(D_METHOD("craftable", "inventory"), &CraftingRecipe::craftable);
    ClassDB::bind_method(D_METHOD("craft", "inventory"), &CraftingRecipe::craft);
    ClassDB::bind_method(D_METHOD("take_inputs", "inventory"), &CraftingRecipe::take_inputs);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "output", PROPERTY_HINT_RESOURCE_TYPE, "Item"), "set_output", "get_output");
}

bool CraftingRecipe::craftable(Ref<Inventory> inventory) const {
	ERR_FAIL_NULL_V_MSG(inventory, false, "Attempt to use null inventory");
	for (int i = 0; i < inputs.size(); i++) {
		Ref<Item> input = inputs[i];
		if (Item::is_empty_or_null(input)) {
			continue;
		}
		if (inventory->get_item_count(input->get_id()) < input->get_count()) {
			return false;
		}
	}
	return true;
}

bool CraftingRecipe::take_inputs(Ref<Inventory> inventory) const {
	if (craftable(inventory)) {
		for (int i = 0; i < inputs.size(); i++) {
			Ref<Item> input = inputs[i];
			if (Item::is_empty_or_null(input)) {
				continue;
			}
			inventory->take_item(input->get_id(), input->get_count());
		}
		return true;
	} else {
		return false;
	}
}

Ref<Item> CraftingRecipe::craft(Ref<Inventory> inventory) const {
	if (take_inputs(inventory)) {
		Ref<Item> output = get_output();
		if (output.is_null()) {
			return memnew(Item);
		} else {
			return output->clone();
		}
	} else {
		return memnew(Item);
	}
}

void CraftingRecipe::_get_property_list(List<PropertyInfo> *r_props) const {
	for (int i = 0; i < inputs.size(); i++) {
		r_props->push_back(PropertyInfo(Variant::OBJECT, "Input " + String::num_int64(i), PROPERTY_HINT_RESOURCE_TYPE, "Item"));
	}
	r_props->push_back(PropertyInfo(Variant::OBJECT, "New Input", PROPERTY_HINT_RESOURCE_TYPE, "Item"));
}

Vector<Ref<CraftingRecipe>> CraftingRecipe::recipes = Vector<Ref<CraftingRecipe>>();

bool CraftingRecipe::_get(const StringName &p_property, Variant &r_value) const {
	String property = p_property;
	if (!property.contains("Input")) {
		return false;
	}
	String id = property.trim_prefix("Input ");
	if (id.is_valid_int()) {
		int idx = id.to_int();
		if (inputs.size() <= idx) {
			r_value = Variant();
			return true;
		} else {
			r_value = inputs[idx];
			return true;
		}
	}
	r_value = Variant();
	return true;
}

bool CraftingRecipe::_set(const StringName &p_property, const Variant &p_value) {
	String property = p_property;
	if (!property.contains("Input")) {
		return false;
	}
	String id = property.trim_prefix("Input ");
	bool is_nil = p_value.get_type() == Variant::NIL;
	if (p_value.get_type() == Variant::OBJECT) {
		is_nil = (Object*)p_value == nullptr;
	}
	if (id.is_valid_int()) {
		int idx = id.to_int();
		if (idx < inputs.size()) {
			if (is_nil) {
				inputs.remove_at(idx);
				notify_property_list_changed();
			} else {
				inputs.write[idx] = p_value;
			}
		} else {
			if (!is_nil) {
				inputs.push_back(p_value);
				notify_property_list_changed();
			}
		}
		return true;
	}
	if (!is_nil) {
		inputs.push_back(p_value);
		notify_property_list_changed();
	}
	return true;
}

TypedArray<Item> CraftingRecipe::get_inputs() const {
    TypedArray<Item> output;
    for (int i = 0; i < inputs.size(); i++) {
        Ref<Item> input = inputs[i];
        if (!Item::is_empty_or_null(input)) {
            output.append(inputs[i]);
        }
    }
    return output;
}

Ref<Item> CraftingRecipe::get_output() const {
    return output;
}

void CraftingRecipe::set_output(Ref<Item> output) {
    this->output = output;
}

void CraftingRecipe::register_recipe(Ref<CraftingRecipe> recipe) {
	ERR_FAIL_NULL_MSG(recipe, "Attempt to register a null crafting recipe");
	recipes.push_back(recipe);
}

TypedArray<CraftingRecipe> CraftingRecipe::all_craftable(Ref<Inventory> inventory) {
	TypedArray<CraftingRecipe> output;
	ERR_FAIL_NULL_V_MSG(inventory, output, "Attempt to determine craftable recipes within a null inventory!");
	for (int i = 0; i < recipes.size(); i++) {
		Ref<CraftingRecipe> recipe = recipes[i];
		if (recipe.is_valid() && recipe->craftable(inventory)) {
			output.append(recipe);
		}
	}
	return output;
}

TypedArray<CraftingRecipe> CraftingRecipe::all_registered() {
	TypedArray<CraftingRecipe> output;
	for (int i = 0; i < recipes.size(); i++) {
		Ref<CraftingRecipe> recipe = recipes[i];
		if (recipe.is_valid()) {
			output.append(recipe);
		}
	}
	return output;
}

CraftingRecipe::CraftingRecipe() {
    output = Ref<Item>(nullptr);
}

CraftingRecipe::~CraftingRecipe() {
    inputs.clear();
	output = Ref<Item>(nullptr);
}

void CraftingRecipe::unregister_hook() {
	recipes.clear();
}