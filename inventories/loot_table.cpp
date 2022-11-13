#include "loot_table.h"
#include "core/math/random_number_generator.h"

void LootTable::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_root"), &LootTable::get_root);
	ClassDB::bind_method(D_METHOD("set_root", "root"), &LootTable::set_root);
	ClassDB::bind_method(D_METHOD("get_output"), &LootTable::get_output);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "root", PROPERTY_HINT_RESOURCE_TYPE, "LootTableEntry"), "set_root", "get_root");
}

Ref<LootTableEntry> LootTable::get_root() {
	return root;
}

void LootTable::set_root(Ref<LootTableEntry> root) {
	this->root = root;
}

Ref<Item> LootTable::get_output() {
	ERR_FAIL_NULL_V_MSG(root, memnew(Item), "This loot table does not have a root!");

	return root->get_output();
}

LootTable::LootTable() {
	root = Ref<LootTableEntry>(nullptr);
}

LootTable::~LootTable() {

}

void LootTableEntry::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_output"), &LootTableEntry::get_output);
	ClassDB::bind_method(D_METHOD("get_weight"), &LootTableEntry::get_weight);
	ClassDB::bind_method(D_METHOD("set_weight", "weight"), &LootTableEntry::set_weight);
	
	ADD_PROPERTY(PropertyInfo(Variant::INT, "weight"), "set_weight", "get_weight");
}

Ref<Item> LootTableEntry::get_output() {
	return Ref<Item>(memnew(Item));
}

int LootTableEntry::get_weight() {
	return weight;
}

void LootTableEntry::set_weight(int weight) {
	this->weight = weight;
}

LootTableEntry::LootTableEntry() {
	weight = 100;
	rng = Ref<RandomNumberGenerator>(memnew(RandomNumberGenerator));
	rng->randomize();
}

LootTableEntry::~LootTableEntry() {

}

void LootTableEntryArray::_bind_methods() {
}

void LootTableEntryArray::_get_property_list(List<PropertyInfo> *r_props) const {
	for (int i = 0; i < entries.size(); i++) {
		r_props->push_back(PropertyInfo(Variant::OBJECT, "Entry " + String::num_int64(i), PROPERTY_HINT_RESOURCE_TYPE, "LootTableEntry"));
	}
	r_props->push_back(PropertyInfo(Variant::OBJECT, "New Entry", PROPERTY_HINT_RESOURCE_TYPE, "LootTableEntry"));
}

bool LootTableEntryArray::_get(const StringName &p_property, Variant &r_value) const {
	String property = String(p_property);
	if (!property.contains("Entry")) {
		return false;
	}
	String id = property.trim_prefix("Entry ");
	if (id.is_valid_int()) {
		int idx = id.to_int();
		if (entries.size() <= idx) {
			r_value = Variant();
			return true;
		} else {
			r_value = entries[idx];
			return true;
		}
	}
	r_value = Variant();
	return true;
}

bool LootTableEntryArray::_set(const StringName &p_property, const Variant &p_value) {
	String property = String(p_property);
	if (!property.contains("Entry")) {
		return false;
	}
	String id = property.trim_prefix("Entry ");
	bool is_nil = p_value.get_type() == Variant::NIL;
	if (p_value.get_type() == Variant::OBJECT) {
		is_nil = (Object*)p_value == nullptr;
	}
	if (id.is_valid_int()) {
		int idx = id.to_int();
		if (idx < entries.size()) {
			if (is_nil) {
				entries.remove_at(idx);
				notify_property_list_changed();
			} else {
				entries[idx] = p_value;
			}
		} else {
			if (!is_nil) {
				entries.push_back(p_value);
				notify_property_list_changed();
			}
		}
		return true;
	}
	if (!is_nil) {
		entries.push_back(p_value);
		notify_property_list_changed();
	}
	return true;
}


Ref<Item> LootTableEntryArray::get_output() {
	ERR_FAIL_COND_V_MSG(entries.is_empty(), memnew(Item), "This array loot table entry doesn't have any entries!");
	int max = 0;
	for (int i = 0; i < entries.size(); i++) {
		max += Object::cast_to<LootTableEntry>(entries[i])->get_weight();
	}
	int rand_num = rng->randi_range(0, max);
	max = 0;
	for (int i = 0; i < entries.size(); i++) {
		Ref<LootTableEntry> output = Object::cast_to<LootTableEntry>(entries[i]);
		max += output->get_weight();
		if (rand_num < max) {
			return output->get_output();
		}
	}
	return Ref<Item>(Object::cast_to<LootTableEntry>(entries[entries.size() - 1])->get_output());
}

LootTableEntryArray::LootTableEntryArray() {
	entries = TypedArray<LootTableEntry>();
	weight = 100;
}

LootTableEntryArray::~LootTableEntryArray() {

}

void LootTableEntryConstant::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_output", "output"), &LootTableEntryConstant::set_output);
	ClassDB::bind_method(D_METHOD("editor_get_output"), &LootTableEntryConstant::editor_get_output);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "output", PROPERTY_HINT_RESOURCE_TYPE, "Item"), "set_output", "editor_get_output");
}

void LootTableEntryConstant::set_output(Ref<Item> output) {
	this->output = output;
}

Ref<Item> LootTableEntryConstant::get_output() {
	if (output.is_valid()) {
		return output->clone();
	} else {
		return output;
	}
}

Ref<Item> LootTableEntryConstant::editor_get_output() {
	return output;
}

LootTableEntryConstant::LootTableEntryConstant() {
	output = Ref<Item>(nullptr);
}

LootTableEntryConstant::~LootTableEntryConstant() {

}

void LootTableEntryRandomize::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_min"), &LootTableEntryRandomize::get_min);
	ClassDB::bind_method(D_METHOD("set_min", "min"), &LootTableEntryRandomize::set_min);
	ClassDB::bind_method(D_METHOD("get_max"), &LootTableEntryRandomize::get_max);
	ClassDB::bind_method(D_METHOD("set_max", "max"), &LootTableEntryRandomize::set_max);
	ClassDB::bind_method(D_METHOD("get_entry"), &LootTableEntryRandomize::get_entry);
	ClassDB::bind_method(D_METHOD("set_entry", "entry"), &LootTableEntryRandomize::set_entry);
	
	ADD_PROPERTY(PropertyInfo(Variant::INT, "min_count"), "set_min", "get_min");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_count"), "set_max", "get_max");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "entry", PROPERTY_HINT_RESOURCE_TYPE, "LootTableEntry"), "set_entry", "get_entry");
}

int LootTableEntryRandomize::get_min() {
	return min;
}

void LootTableEntryRandomize::set_min(int min) {
	this->min = MAX(1, min);
}

int LootTableEntryRandomize::get_max() {
	return max;
}

void LootTableEntryRandomize::set_max(int max) {
	this->max = MAX(min, max);
}

Ref<LootTableEntry> LootTableEntryRandomize::get_entry() {
	return entry;
}

void LootTableEntryRandomize::set_entry(Ref<LootTableEntry> entry) {
	this->entry = entry;
}

Ref<Item> LootTableEntryRandomize::get_output() {
	ERR_FAIL_NULL_V_MSG(entry, memnew(Item), "This loot table entry randomizer doesn't have a loot table entry to randomize!");

	Ref<Item> output = entry->get_output();
	if (!output->is_empty()) {
		output->set_count(rng->randi_range(MAX(1, min), MIN(output->get_data()->get_stack_size(), max)));
	}
	return output;
}

LootTableEntryRandomize::LootTableEntryRandomize() {
	entry = Ref<LootTableEntry>(nullptr);
	min = 1;
	max = 100;
}

LootTableEntryRandomize::~LootTableEntryRandomize() {

}