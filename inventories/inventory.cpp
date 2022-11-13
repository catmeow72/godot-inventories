#include "inventory.h"
#include "core/object/object.h"
#include "core/variant/typed_array.h"
#include "item.h"

void Inventory::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_size"), &Inventory::get_size);
    ClassDB::bind_method(D_METHOD("set_size", "size"), &Inventory::set_size);
    ClassDB::bind_method(D_METHOD("get_items"), &Inventory::get_items);
    ClassDB::bind_method(D_METHOD("set_items", "items"), &Inventory::set_items);
    ClassDB::bind_method(D_METHOD("set_slot", "slot_id" "item"), &Inventory::set_slot);
    ClassDB::bind_method(D_METHOD("has_item", "id"), &Inventory::has_item);
    ClassDB::bind_method(D_METHOD("take_slot", "slot_id"), &Inventory::take_slot);
    ClassDB::bind_method(D_METHOD("peek_slot", "slot_id"), &Inventory::peek_slot);
    ClassDB::bind_method(D_METHOD("take_item", "id", "count"), &Inventory::take_item);
    ClassDB::bind_method(D_METHOD("add_item", "item"), &Inventory::add_item);
    ClassDB::bind_method(D_METHOD("swap_item", "slot_id", "item"), &Inventory::swap_item);
    ClassDB::bind_method(D_METHOD("get_item_count", "id"), &Inventory::get_item_count);
    ClassDB::bind_method(D_METHOD("use_slot", "slot_id", "owner"), &Inventory::use_slot);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "size"), "set_size", "get_size");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "items", PROPERTY_HINT_ARRAY_TYPE, "Item"), "set_items", "get_items");
    ADD_SIGNAL(MethodInfo("item_changed", PropertyInfo(Variant::INT, "slot_id"), PropertyInfo(Variant::OBJECT, "new_item", PROPERTY_HINT_RESOURCE_TYPE, "Item")));
}

void Inventory::add_to_cache(StringName id, int diff) {
    if (cache.has(id)) {
        cache[id] = cache[id] + diff;
    } else {
        cache[id] = diff;
    }
    if (cache[id] < 0) {
        ERR_PRINT("Inventory item count cache is invalid!");
        invalidate_cache();
    }
}

void Inventory::invalidate_cache() {
    cache.clear();
    for (uint i = 0; i < items.size(); i++) {
        Ref<Item> item = items[i];
        if (!Item::is_empty_or_null(item)) {
            add_to_cache(item->get_id(), item->get_count());
        }
    }
}

void Inventory::update_slot(int slot_id) {
    if (slot_id < (int)items.size()) {
        invalidate_cache();
        emit_signal("item_changed", slot_id, peek_slot(slot_id)->clone());
    }
}

void Inventory::set_slot(int slot_id, Ref<Item> item) {
    if (slot_id < (int)items.size()) {
        Ref<Item> backup = peek_slot(slot_id);
        if (backup.is_valid()) {
            backup = backup->clone();
        }
        items[slot_id] = item;
        if (!Item::is_empty_or_null(item)) {
            add_to_cache(item->get_id(), item->get_count());
        } 
        if (!Item::is_empty_or_null(backup)) {
            add_to_cache(backup->get_id(), -backup->get_count());
        }
        emit_signal("item_changed", slot_id, item);
    }
}

ItemUseResult Inventory::use_slot(int slot_id, Node *owner) {
    if (slot_id < (int)items.size()) {
        Ref<Item> item = peek_slot(slot_id);
        if (!Item::is_empty_or_null(item)) {
            Ref<Item> backup = item->clone();
            ItemUseResult result = item->use(owner);
            if (item->get_id() != backup->get_id()) {
                add_to_cache(backup->get_id(), -backup->get_count());
                add_to_cache(item->get_id(), item->get_count());
            } else {
                add_to_cache(item->get_id(), item->get_count() - backup->get_count());
            }
            emit_signal("item_changed", slot_id, item);
            return result;
        }
    }
    return ITEM_USE_RESULT_FAIL;
}

int Inventory::get_size() const {
    return size;
}

void Inventory::set_size(int size) {
    for (uint i = size; i < items.size();) {
        set_slot(i, Ref<Item>(nullptr));
    }
    items.resize(size);
    this->size = size;
}

TypedArray<Item> Inventory::get_items() const {
    TypedArray<Item> output;
    for (uint i = 0; i < items.size(); i++) {
        output.append(peek_slot(i));
    }
    return output;
}

void Inventory::set_items(TypedArray<Item> items) {
    this->items.clear();
    this->items.reserve(size);
    for (int i = 0; i < size && i < (int)items.size(); i++) {
        this->items.push_back((Ref<Item>)items[i]);
    }
    invalidate_cache();
    for (int i = 0; i < size; i++) {
        emit_signal("item_changed", i, (this->items[i]));
    }
}

bool Inventory::has_item(StringName id) const {
    return get_item_count(id) > 0;
}

Ref<Item> Inventory::take_slot(int slot_id) {
    if ((int)items.size() < slot_id) {
        return memnew(Item);
    } else {
        Ref<Item> item = items[slot_id];
        if (Item::is_empty_or_null(item)) {
            return memnew(Item);
        } else {
            set_slot(slot_id, Ref<Item>(nullptr));
            return item;
        }
    }
}

Ref<Item> Inventory::peek_slot(int slot_id) const {
    if ((int)items.size() < slot_id) {
        return memnew(Item);
    } else {
        Ref<Item> item = items[slot_id];
        if (item.is_null()) {
            return memnew(Item);
        } else {
            return item;
        }
    }
}

Ref<Item> Inventory::take_item(StringName id, int count) {
    Ref<Item> output = Ref<Item>(memnew(Item(id, 0)));
    for (uint i = 0; i < items.size(); i++) {
        Ref<Item> item = peek_slot(i);
        if (!Item::is_empty_or_null(item) && item->get_id() == id) {
            int output_new_count = output->get_count() + item->get_count();
            if (output_new_count > count) {
                int item_new_count = output_new_count - count;
                item->set_count(item_new_count);
                output->set_count(count);
                set_slot(i, item);
                break;
            }
            set_slot(i, Ref<Item>(nullptr));
            output->set_count(output_new_count);
        }
    }
    return output;
}

int Inventory::add_slot(int slot_id, Ref<Item> item) {
    if ((int)items.size() <= slot_id) {
        return false;
    }
    if (Item::is_empty_or_null(item)) {
        return 0;
    }
    int stack_size = item->get_data()->get_stack_size();
    Ref<Item> my_item = peek_slot(slot_id);
    if (Item::is_empty_or_null(my_item)) {
        set_slot(slot_id, item);
        return 0;
    } else {
        my_item = my_item->clone();
    }
    if (my_item->is_equal_type(item)) {
        int my_count = my_item->get_count();
        int their_count = item->get_count();
        int their_new_count = 0;
        int my_new_count = my_count + their_count;
        if (my_new_count > stack_size) {
            their_new_count = my_new_count - stack_size;
            my_new_count = stack_size;
        }
        item->set_count(their_new_count);
        my_item->set_count(my_new_count);
        set_slot(slot_id, my_item);
    }
    return item->get_count();
}

int Inventory::add_item(Ref<Item> item) {
    if (Item::is_empty_or_null(item)) {
        return 0;
    }
    int stack_size = item->get_data()->get_stack_size();
    for (uint i = 0; i < items.size(); i++) {
        Ref<Item> my_item = peek_slot(i);
        if (Item::is_empty_or_null(my_item)) {
            set_slot(i, item);
            break;
        } else {
            my_item = my_item->clone();
        }
        if (my_item->is_equal_type(item)) {
            int my_count = my_item->get_count();
            int their_count = item->get_count();
            int their_new_count = 0;
            int my_new_count = my_count + their_count;
            if (my_new_count > stack_size) {
                their_new_count = my_new_count - stack_size;
                my_new_count = stack_size;
            }
            item->set_count(their_new_count);
            my_item->set_count(my_new_count);
            set_slot(i, my_item);
        }
        if (item->is_empty()) {
            break;
        }
    }
    return item->get_count();
}

Ref<Item> Inventory::swap_item(int slot_id, Ref<Item> item) {
    if ((int)items.size() <= slot_id) {
        return item;
    }
    Ref<Item> output = peek_slot(slot_id);
    set_slot(slot_id, item);
    return output;
}

int Inventory::get_item_count(StringName id) const {
    if (cache.has(id)) {
        return cache[id];
    } else {
        return 0;
    }
}

Inventory::Inventory() {
    size = 0;
    set_items(TypedArray<Item>());
}

Inventory::~Inventory() {
    items.clear();
}
