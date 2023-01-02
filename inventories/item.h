#ifndef ITEM_H
#define ITEM_H

#include "core/object/ref_counted.h"
#include "core/templates/hash_map.h"
#include "core/templates/local_vector.h"
#include "core/math/rect2i.h"
#include "core/string/ustring.h"
#include "core/string/string_name.h"
#include "core/io/resource.h"
#include "core/variant/binder_common.h"
#include "scene/resources/texture.h"
#include "scene/main/node.h"
#include "scene/resources/texture.h"
#include "core/config/project_settings.h"

enum ItemUseResult {
	ITEM_USE_RESULT_CONSUME,
	ITEM_USE_RESULT_NONE,
	ITEM_USE_RESULT_FAIL,
};

VARIANT_ENUM_CAST(ItemUseResult);

class Item;

class ItemData : public Resource {
	GDCLASS(ItemData, Resource);
	friend class ItemRegistry;
protected:
	static void _bind_methods();
	Ref<Texture2D> texture;
	String display_name;
	int stack_size;
	void pre_unregister();
	ItemUseResult use_item(Ref<Item> item, Node* owner);
public:
	int get_stack_size();
	void set_stack_size(int stack_size);
	Ref<Texture2D> get_texture();
	void set_texture(Ref<Texture2D> image);
	String get_display_name();
	void set_display_name(String display_name);
	GDVIRTUAL2RC(ItemUseResult, _use_item, Ref<Item>, Node *);
	GDVIRTUAL0C(_pre_unregister);
	ItemData();
	~ItemData();
};

class ItemRegistry : public Node {
	GDCLASS(ItemRegistry, Node);
protected:
	static void _bind_methods();

	HashMap<StringName, Ref<ItemData>> data;
	LocalVector<StringName> registered;
	static ItemRegistry* singleton;
public:
	_ALWAYS_INLINE_ static ItemRegistry* get_singleton() { return singleton; }
	Ref<ItemData> get_data(StringName id);
	Dictionary get_all_data();
	void set_all_data(Dictionary data);
	ItemUseResult use_item(Ref<Item> item, Node* owner);
	void register_data(StringName id, Ref<ItemData> data);
	void unregister_data(StringName id);
	void unregister_all();
	ItemRegistry();
	~ItemRegistry();

};

class Item : public Resource {
	GDCLASS(Item, Resource);
protected:
	static void _bind_methods();
	int count;
	StringName id;
public:
	int get_count() const;
	void set_count(int count);
	StringName get_id() const;
	void set_id(StringName id);

	Ref<ItemData> get_data() const;
	bool is_empty() const;
	static bool is_empty_or_null(const Ref<Item> item);
	void make_empty();
	bool is_full() const;
	bool is_equal_type(const Ref<Item> other) const;
	Ref<Item> clone() const;
	ItemUseResult use(Node *owner);
	
	Item();
	Item(StringName id, int count);
	~Item();
};

#endif // ITEM_H
