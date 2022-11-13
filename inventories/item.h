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
protected:
	static void _bind_methods();
	Rect2i atlas_rect;
	String display_name;
	int stack_size;
	static HashMap<StringName, Ref<ItemData>> data;
	static Ref<Texture2D> atlas;
	static LocalVector<StringName> registered;
	void pre_unregister();
public:
	int get_stack_size();
	void set_stack_size(int stack_size);
	Rect2i get_atlas_rect();
	void set_atlas_rect(Rect2i atlas_rect);
	String get_display_name();
	void set_display_name(String display_name);
	GDVIRTUAL2RC(ItemUseResult, _use_item, Ref<Item>, Node *);
	GDVIRTUAL0C(_pre_unregister);
	ItemUseResult use_item(Ref<Item> item, Node *owner);
	Ref<AtlasTexture> get_texture();
	static void register_data(StringName id, Ref<ItemData> data);
	static void unregister_data(StringName id);
	static void unregister_all();
	static Ref<ItemData> get_data(StringName id);
	static void register_hook();
	static void unregister_hook();
	ItemData();
	~ItemData();
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
