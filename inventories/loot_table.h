#ifndef LOOT_TABLE_H
#define LOOT_TABLE_H

#include "core/math/random_number_generator.h"
#include "core/object/ref_counted.h"
#include "core/variant/typed_array.h"
#include "core/string/ustring.h"
#include "core/string/string_name.h"
#include "core/io/resource.h"

#include "item.h"

class LootTableEntry : public Resource {
	GDCLASS(LootTableEntry, Resource);
	OBJ_SAVE_TYPE(LootTableEntry);
protected:
	static void _bind_methods();
	int weight;
	Ref<RandomNumberGenerator> rng;
public:
	void set_weight(int weight);
	int get_weight();
	virtual Ref<Item> get_output();
	LootTableEntry();
	~LootTableEntry();
};

class LootTableEntryConstant : public LootTableEntry {
	GDCLASS(LootTableEntryConstant, LootTableEntry);
protected:
	static void _bind_methods();
	Ref<Item> output;
public:
	void set_output(Ref<Item> output);
	Ref<Item> editor_get_output();
    Ref<Item> get_output() override;
	LootTableEntryConstant();
	~LootTableEntryConstant();
};

class LootTableEntryRandomize : public LootTableEntry {
	GDCLASS(LootTableEntryRandomize, LootTableEntry);
protected:
	static void _bind_methods();
	int min;
	int max;
	Ref<LootTableEntry> entry;
public:
    Ref<LootTableEntry> get_entry();
    void set_entry(Ref<LootTableEntry> entry);
	int get_min();
	void set_min(int min);
	int get_max();
	void set_max(int max);
    Ref<Item> get_output() override;
	LootTableEntryRandomize();
	~LootTableEntryRandomize();
};

class LootTableEntryArray : public LootTableEntry {
	GDCLASS(LootTableEntryArray, LootTableEntry);
protected:
	static void _bind_methods();
	TypedArray<LootTableEntry> entries;
public:
    void _get_property_list(List<PropertyInfo> *r_props) const;
    bool _get(const StringName &p_property, Variant &r_value) const;
    bool _set(const StringName &p_property, const Variant &p_value);
    Ref<Item> get_output() override;
	LootTableEntryArray();
	~LootTableEntryArray();
};

class LootTable : public Resource {
	GDCLASS(LootTable, Resource);
	RES_BASE_EXTENSION("loot");

protected:
	static void _bind_methods();
	Ref<LootTableEntry> root;
	Ref<RandomNumberGenerator> rng;

public:
	void set_root(Ref<LootTableEntry> input);
	Ref<LootTableEntry> get_root();
	Ref<Item> get_output();

	LootTable();
	~LootTable();
};

#endif // LOOT_TABLE_H