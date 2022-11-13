#ifndef INVENTORY_H
#define INVENTORY_H

#include "core/object/ref_counted.h"
#include "core/templates/hash_map.h"
#include "core/variant/typed_array.h"
#include "core/templates/local_vector.h"
#include "item.h"

class Inventory : public RefCounted {
    GDCLASS(Inventory, RefCounted);
protected:
    static void _bind_methods();
    int size;
    TightLocalVector<Ref<Item>> items;
    HashMap<StringName, int> cache;
    void add_to_cache(StringName id, int diff);
    void invalidate_cache();

public:
    int get_size() const;
    void set_size(int size);
    TypedArray<Item> get_items() const;
    void set_items(TypedArray<Item> items);
    void set_slot(int slot_id, Ref<Item> item);
    bool has_item(StringName id) const;
    Ref<Item> take_slot(int slot_id);
    Ref<Item> peek_slot(int slot_id) const;
    Ref<Item> take_item(StringName id, int count);
    int add_item(Ref<Item> item);
    int add_slot(int slot_id, Ref<Item> item);
    Ref<Item> swap_item(int slot_id, Ref<Item> item);
    int get_item_count(StringName id) const;
    void update_slot(int slot_id);
    ItemUseResult use_slot(int slot_id, Node *owner);

    Inventory();
    ~Inventory();
};

#endif