#include "item.h"
#include "core/io/resource_loader.h"
#include "core/object/class_db.h"
#include "core/object/method_bind.h"
#include "core/object/object.h"

void ItemData::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_stack_size"), &ItemData::get_stack_size);
    ClassDB::bind_method(D_METHOD("set_stack_size", "stack_size"), &ItemData::set_stack_size);
    ClassDB::bind_method(D_METHOD("get_texture"), &ItemData::get_texture);
    ClassDB::bind_method(D_METHOD("set_texture", "texture"), &ItemData::set_texture);
    ClassDB::bind_method(D_METHOD("get_display_name"), &ItemData::get_display_name);
    ClassDB::bind_method(D_METHOD("set_display_name", "display_name"), &ItemData::set_display_name);
    ClassDB::bind_method(D_METHOD("use_item", "item", "owner"), &ItemData::use_item);
    GDVIRTUAL_BIND(_use_item, "item", "owner");
    GDVIRTUAL_BIND(_pre_unregister);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "stack_size"), "set_stack_size", "get_stack_size");
    ADD_PROPERTY(PropertyInfo(Variant::RECT2I, "texture"), "set_texture", "get_texture");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "display_name"), "set_display_name", "get_display_name");
    BIND_ENUM_CONSTANT(ITEM_USE_RESULT_CONSUME);
    BIND_ENUM_CONSTANT(ITEM_USE_RESULT_NONE);
    BIND_ENUM_CONSTANT(ITEM_USE_RESULT_FAIL);
}


int ItemData::get_stack_size() {
    return stack_size;
}

void ItemData::set_stack_size(int stack_size) {
    this->stack_size = stack_size;
}

Ref<Texture2D> ItemData::get_texture() {
    return texture;
}

void ItemData::set_texture(Ref<Texture2D> texture) {
    this->texture = texture;
}

String ItemData::get_display_name() {
    return display_name;
}

void ItemData::set_display_name(String display_name) {
    this->display_name = display_name;
}

void ItemData::pre_unregister() {
    GDVIRTUAL_CALL(_pre_unregister);
}

ItemUseResult ItemData::use_item(Ref<Item> item, Node *owner) {
    ItemUseResult ret = ITEM_USE_RESULT_CONSUME;
    if (GDVIRTUAL_CALL(_use_item, item, owner, ret)) {
        return ret;
    } else {
        return ITEM_USE_RESULT_CONSUME;
    }
}

ItemData::ItemData() {
    stack_size = 100;
    display_name = "";
    texture = Ref<Texture2D>(nullptr);
}

ItemData::~ItemData() {
    
}

ItemRegistry* ItemRegistry::singleton = nullptr;

void ItemRegistry::_bind_methods() {
    ClassDB::bind_method(D_METHOD("register", "id", "data"), &ItemRegistry::register_data);
    ClassDB::bind_method(D_METHOD("unregister", "id"), &ItemRegistry::unregister_data);
    ClassDB::bind_method(D_METHOD("unregister_all"), &ItemRegistry::unregister_all);
    ClassDB::bind_method(D_METHOD("get_data", "id"), &ItemRegistry::get_data);
    ClassDB::bind_method(D_METHOD("get_all_data"), &ItemRegistry::get_all_data);
    ClassDB::bind_method(D_METHOD("set_all_data", "data"), &ItemRegistry::set_all_data);

    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "all_data"), "set_all_data", "get_all_data");
}

Ref<ItemData> ItemRegistry::get_data(StringName id) {
    if (data.has(id)) {
        return data[id];
    }
    else if (id == "empty") {
        return memnew(ItemData);
    }
    else {
        ERR_FAIL_V_MSG(memnew(ItemData), vformat("Item ID '%s' not found.", id));
    }
}

void ItemRegistry::register_data(StringName id, Ref<ItemData> new_data) {
    ERR_FAIL_NULL_MSG(new_data, vformat("Attempt to register null data to '%s'!", id));
    if (data.has(id)) {
        unregister_data(id);
    }
    data[id] = Ref<ItemData>(new_data);
    registered.push_back(id);
}

void ItemRegistry::unregister_data(StringName id) {
    if (data.has(id)) {
        data[id]->pre_unregister();
        registered.erase(id);
        data.erase(id);
    }
}
void ItemRegistry::unregister_all() {
    while (!registered.is_empty()) {
        unregister_data(registered[0]);
    }
}

Dictionary ItemRegistry::get_all_data() {
    Dictionary output;
    for (int i = 0; i < (int)registered.size(); i++) {
        StringName name = registered[i];
        output[name] = data[name];
    }
    return output;
}

void ItemRegistry::set_all_data(Dictionary data) {
    unregister_all();
    Array keys = data.keys();
    Array values = data.values();
    for (int i = 0; i < keys.size(); i++) {
        Variant key_variant = keys[i];
        Variant value_variant = values[i];
        ItemData* value = nullptr; // Needed in the if statement to allow checking the object-derived type and at the same time setting the value when valid.
        if (Variant::can_convert(key_variant.get_type(), Variant::STRING_NAME) && value_variant.get_type() == Variant::OBJECT && (value = Object::cast_to<ItemData>(value_variant)) != nullptr) {
            StringName key = key_variant;
            register_data(key, value);
        }
    }
}

ItemUseResult ItemRegistry::use_item(Ref<Item> item, Node* owner) {
    if (item.is_valid()) {
        Ref<ItemData> data = item->get_data();
        if (data.is_valid()) {
            return data->use_item(item, owner);
        }
    }
    return ITEM_USE_RESULT_FAIL;
}

ItemRegistry::ItemRegistry() {
    data = HashMap<StringName, Ref<ItemData>>();
    registered = LocalVector<StringName>();
    singleton = this;
}

ItemRegistry::~ItemRegistry() {
    if (singleton == this) {
        singleton = nullptr;
    }
    registered.clear();
    data.clear();
}

void Item::_bind_methods() {
    ClassDB::bind_static_method("Item", D_METHOD("is_empty_or_null", "iten"), &Item::is_empty_or_null);
    ClassDB::bind_method(D_METHOD("get_count"), &Item::get_count);
    ClassDB::bind_method(D_METHOD("set_count", "count"), &Item::set_count);
    ClassDB::bind_method(D_METHOD("get_id"), &Item::get_id);
    ClassDB::bind_method(D_METHOD("set_id", "id"), &Item::set_id);
    ClassDB::bind_method(D_METHOD("make_empty"), &Item::make_empty);
    ClassDB::bind_method(D_METHOD("get_data"), &Item::get_data);
    ClassDB::bind_method(D_METHOD("is_empty"), &Item::is_empty);
    ClassDB::bind_method(D_METHOD("is_full"), &Item::is_full);
    ClassDB::bind_method(D_METHOD("is_equal_type", "other"), &Item::is_equal_type);
    ClassDB::bind_method(D_METHOD("clone"), &Item::clone);
    ClassDB::bind_method(D_METHOD("use", "owner"), &Item::use);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "count"), "set_count", "get_count");
    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "id"), "set_id", "get_id");
}

int Item::get_count() const {
    return is_empty() ? 0 : count;
}

void Item::set_count(int count) {
    if (count == 0) {
        make_empty();
        return;
    }
    Ref<ItemData> data = get_data();
    if (data.is_null()) {
        this->count = MAX(count, 0);
    } else {
        int stack_size = data->get_stack_size();
        this->count = CLAMP(stack_size, 0, count);
    }
}

StringName Item::get_id() const {
    return id;
}

void Item::set_id(StringName id) {
    this->id = id;
}

void Item::make_empty() {
    this->id = "empty";
    this->count = 0;
}

ItemUseResult Item::use(Node *owner) {
    if (is_empty()) {
        return ITEM_USE_RESULT_FAIL;
    }
    ItemUseResult use_result = ItemRegistry::get_singleton()->use_item(this, owner);
    if (use_result == ITEM_USE_RESULT_CONSUME) {
        set_count(get_count() - 1);
    }
    return use_result;
}

Ref<ItemData> Item::get_data() const {
    return ItemRegistry::get_singleton()->get_data(id);
}

bool Item::is_empty() const {
    return id == "empty" || count <= 0;
}

bool Item::is_empty_or_null(const Ref<Item> item) {
    return item.is_null() || item->is_empty();
}

bool Item::is_full() const {
    return !is_empty() && count >= get_data()->get_stack_size();
}

Ref<Item> Item::clone() const {
    return memnew(Item(get_id(), get_count()));
}

bool Item::is_equal_type(const Ref<Item> other) const {
    // Two different empty values are considered unequal to avoid a bug involving using their invalid counts.
    if (Item::is_empty_or_null(other) || is_empty()) {
        return false;
    } else {
        return other->get_id() == get_id();
    }
}

Item::Item(StringName id, int count) {
    this->id = id;
    this->count = count;
}

Item::Item() : Item::Item("empty", 0) {
    // Nothing more needs to be done here.
}

Item::~Item() {

}
