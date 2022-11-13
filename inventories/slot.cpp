#include "slot.h"
#include "scene/main/window.h"
#include "scene/main/canvas_layer.h"

void AbstractSlot::_bind_methods() {
    GDVIRTUAL_BIND(_peek_item);
    GDVIRTUAL_BIND(_take_item);
    GDVIRTUAL_BIND(_swap_item, "other");
    GDVIRTUAL_BIND(_set_item, "other");

    ClassDB::bind_method(D_METHOD("peek_item"), &AbstractSlot::peek_item);
    ClassDB::bind_method(D_METHOD("take_item"), &AbstractSlot::take_item);
    ClassDB::bind_method(D_METHOD("swap_item"), &AbstractSlot::swap_item);
    ClassDB::bind_method(D_METHOD("set_item", "other"), &AbstractSlot::set_item);
    ClassDB::bind_method(D_METHOD("is_disabled"), &AbstractSlot::is_disabled);
    ClassDB::bind_method(D_METHOD("set_disabled", "disabled"), &AbstractSlot::set_disabled);
    ClassDB::bind_method(D_METHOD("is_mouse_hovering"), &AbstractSlot::is_mouse_hovering);
    ClassDB::bind_method(D_METHOD("set_mouse_hovering", "hovering"), &AbstractSlot::set_mouse_hovering);

    ClassDB::bind_method(D_METHOD("get_default_slot_helper"), &AbstractSlot::get_default_slot_helper);
    
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "item", PROPERTY_HINT_RESOURCE_TYPE, "Item"), "set_item", "peek_item");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "disabled"), "set_disabled", "is_disabled");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "mouse_hovering", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_mouse_hovering", "is_mouse_hovering");
}

Ref<Item> AbstractSlot::peek_item() {
    Ref<Item> output;
    if (GDVIRTUAL_CALL(_peek_item, output)) {
        return output;
    } else {
        return Ref<Item>(nullptr);
    }
}

Ref<Item> AbstractSlot::take_item() {
    Ref<Item> output;
    if (GDVIRTUAL_CALL(_take_item, output)) {
        return output;
    } else {
        output = peek_item();
        set_item(memnew(Item));
        return output;
    }
}

Ref<Item> AbstractSlot::swap_item(Ref<Item> other) {
    Ref<Item> output;
    other = other;
    if (GDVIRTUAL_CALL(_swap_item, other, output)) {
        queue_redraw();
        return output;
    } else {
        if (Item::is_empty_or_null(output)) {
            if (set_item(other)) {
                return memnew(Item);
            } else {
                return other;
            }
        } else if (Item::is_empty_or_null(other)) {
            return output;
        } else {
            if (other->get_id() == output->get_id()) {
                int other_count = other->get_count() + output->get_count();
                int output_count = 0;
                other->set_count(other_count);
                if (other->is_full()) {
                    output_count = other_count - other->get_count();
                }
                output->set_count(output_count);
            }
            set_item(other);
            return output;
        }
    }
}

bool AbstractSlot::set_item(Ref<Item> other) {
    bool output = true;
    GDVIRTUAL_REQUIRED_CALL(_set_item, other, output);
    if (mouse_hovering) {
        SlotHelper *helper = get_default_slot_helper();
        if (helper != nullptr) {
            helper->update_tooltip(this);
        }
    }
    queue_redraw();
    return output;
}

Size2 AbstractSlot::get_minimum_size() const {
    return Size2(theme_cache.min_size, theme_cache.min_size);
}

void AbstractSlot::_update_theme_item_cache() {
	Control::_update_theme_item_cache();

	theme_cache.normal_style = get_theme_stylebox(SNAME("normal"));
	theme_cache.hover_style = get_theme_stylebox(SNAME("hover"));
    theme_cache.disabled_style = get_theme_stylebox(SNAME("disabled"));

	theme_cache.font = get_theme_font(SNAME("font"));

	theme_cache.font_size = get_theme_font_size(SNAME("font_size"));
	theme_cache.font_color = get_theme_color(SNAME("font_color"));

    theme_cache.min_size = get_theme_constant(SNAME("min_size"));
}

void AbstractSlot::_notification(int notification) {
    switch (notification) {
        case NOTIFICATION_DRAW: {
            Rect2 ctrl_rect = Rect2(Point2(0, 0), get_size());
            Ref<Item> item = peek_item();
            int count = 0;
            String name = "";
            Ref<Texture2D> texture = Ref<Texture2D>(nullptr);
            if (!Item::is_empty_or_null(item)) {
                count = item->get_count();
                Ref<ItemData> item_data = item->get_data();
                if (item_data.is_valid()) {
                    name = item_data->get_display_name();
                    texture = item_data->get_texture();
                }
            }
            draw_style_box(is_disabled() ? theme_cache.disabled_style : (is_mouse_hovering() ? theme_cache.hover_style : theme_cache.normal_style), ctrl_rect);
            if (texture.is_valid()) {
                Point2 texture_pos = Point2(0, 0);
                Size2 texture_size = Size2(0, 0);
                texture_size = texture->get_size() * 2.0;
                texture_pos = ctrl_rect.get_center() - (texture_size / 2.0);
                Rect2 texture_rect = Rect2(texture_pos, texture_size);
                draw_texture_rect(texture, texture_rect);
            }
            if (count > 1) {
                int font_size = theme_cache.font_size;
                Ref<Font> font = theme_cache.font;
                String count_str = itos(count);
                Size2 str_size = font->get_string_size(count_str, HORIZONTAL_ALIGNMENT_RIGHT, -1, font_size);
                draw_string(theme_cache.font, ctrl_rect.get_end() - Point2(str_size.x, 0), count_str, HORIZONTAL_ALIGNMENT_RIGHT, -1, font_size, theme_cache.font_color);
            }
        } break;
        case NOTIFICATION_THEME_CHANGED: {
            _update_theme_item_cache();
            update_minimum_size();
            queue_redraw();
        } break;
        case NOTIFICATION_MOUSE_ENTER: {
            mouse_hovering = true;
            SlotHelper *helper = get_default_slot_helper();
            if (helper != nullptr) {
                helper->show_tooltip(this);
            }
            queue_redraw();
        } break;
        case NOTIFICATION_MOUSE_EXIT: {
            mouse_hovering = false;
            SlotHelper *helper = get_default_slot_helper();
            if (helper != nullptr) {
                helper->hide_tooltip(this);
            }
            queue_redraw();
        } break;
    }
}

SlotHelper *AbstractSlot::get_default_slot_helper() {
    return SlotHelper::default_helper;
}

void AbstractSlot::gui_input(const Ref<InputEvent> &p_event) {
    if (is_disabled()) {
        mouse_hovering = false;
        return;
    }
    Ref<InputEventMouseMotion> mm_event = p_event;
    if (mm_event.is_valid()) {
        bool mouse_was_hovering = mouse_hovering;
        mouse_hovering = has_point(mm_event->get_position());
        if (mouse_was_hovering != mouse_hovering) {
            SlotHelper *helper = get_default_slot_helper();
            if (helper != nullptr) {
                if (mouse_hovering) {
                    helper->show_tooltip(this);
                } else {
                    helper->hide_tooltip(this);
                }
            }
            queue_redraw();
        }
    }
    Ref<InputEventMouseButton> mb_event = p_event;
    if (mb_event.is_valid() && mb_event->get_button_index() == MouseButton::LEFT) {
        bool mouse_was_pressed = mouse_pressed;
        mouse_pressed = mb_event->is_pressed();
        if (mouse_hovering && mouse_pressed != mouse_was_pressed && mouse_pressed) {
            SlotHelper *default_helper = get_default_slot_helper();
            if (default_helper != nullptr) {
                default_helper->swap_with_slot(this);
            }
        }
    }
}

bool AbstractSlot::is_mouse_hovering() const {
    return !is_disabled() && mouse_hovering;
}

void AbstractSlot::set_mouse_hovering(bool hovering) {
    this->mouse_hovering = hovering;
    queue_redraw();
}

bool AbstractSlot::is_disabled() const {
    return disabled;
}

void AbstractSlot::set_disabled(bool disabled) {
    this->disabled = disabled;
    queue_redraw();
}

AbstractSlot::AbstractSlot() {
    mouse_hovering = false;
    mouse_pressed = false;
    disabled = false;
    set_texture_filter(TEXTURE_FILTER_NEAREST);
}

AbstractSlot::~AbstractSlot() {
    if (mouse_hovering) {
        SlotHelper *helper = get_default_slot_helper();
        if (helper != nullptr) {
            helper->hide_tooltip(this);
        }
    }
}

void Slot::_bind_methods() {

}


Ref<Item> Slot::peek_item() {
    return item;
}

bool Slot::set_item(Ref<Item> other) {
    this->item = other;
    queue_redraw();
    return true;
}

Slot::Slot() {
    item = Ref<Item>(nullptr);
}

Slot::~Slot() {

}

void SlotHelper::_bind_methods() {
    ClassDB::bind_method(D_METHOD("swap_with_slot", "slot"), &SlotHelper::swap_with_slot);
    ClassDB::bind_method(D_METHOD("show_tooltip", "slot"), &SlotHelper::show_tooltip);
    ClassDB::bind_method(D_METHOD("update_tooltip", "slot"), &SlotHelper::update_tooltip);
    ClassDB::bind_method(D_METHOD("hide_tooltip", "slot"), &SlotHelper::hide_tooltip);
}

void SlotHelper::_notification(int notification) {
    switch (notification) {
        case NOTIFICATION_INTERNAL_PROCESS: {
            set_global_position(get_global_mouse_position() - (get_size() / 2.0));
        } break;
    }
}
void SlotHelper::show_tooltip(AbstractSlot *slot) {
    if (slot == nullptr) {
        return;
    }
    tooltip_slot = slot;
    update_tooltip(slot);
    if (tooltip_slot != nullptr) {
        tooltip_container->show();
    }
}

void SlotHelper::update_tooltip(AbstractSlot *slot) {
    if (slot == nullptr) {
        return;
    }
    if (slot == tooltip_slot) {
        Ref<Item> item = slot->peek_item();
        if (!Item::is_empty_or_null(item)) {
            Ref<ItemData> data = item->get_data();
            String item_name = "Invalid item!";
            if (data.is_valid()) {
                item_name = data->get_display_name();
            }
            tooltip_label->set_text(item_name);
        } else {
            hide_tooltip(slot);
        }
    }
}

void SlotHelper::hide_tooltip(AbstractSlot *slot) {
    if (slot == tooltip_slot) {
        tooltip_slot = nullptr;
        tooltip_container->hide();
    }
}

void SlotHelper::swap_with_slot(AbstractSlot *slot) {
    if (slot == nullptr) {
        return;
    }
    set_item(slot->swap_item(take_item()));
}

SlotHelper *SlotHelper::default_helper = nullptr;
Vector<SlotHelper*> SlotHelper::helpers = Vector<SlotHelper*>();

SlotHelper::SlotHelper() {
    if (default_helper == nullptr) {
        default_helper = this;
    }
    helpers.append(this);
    tooltip_container = memnew(PanelContainer);
    tooltip_container->set_mouse_filter(MOUSE_FILTER_IGNORE);
    tooltip_label = memnew(Label);
    tooltip_label->set_mouse_filter(MOUSE_FILTER_IGNORE);
    tooltip_container->add_child(tooltip_label);
    tooltip_container->hide();
    add_child(tooltip_container);
    add_theme_style_override("normal", Ref<StyleBox>(memnew(StyleBoxEmpty)));
    add_theme_style_override("hover", Ref<StyleBox>(memnew(StyleBoxEmpty)));
    set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
    set_process_internal(true);
    set_as_top_level(true);
}

SlotHelper::~SlotHelper() {
    helpers.erase(this);
    if (default_helper == this) {
        if (helpers.size() > 0) {
            default_helper = helpers[0];
        } else {
            default_helper = nullptr;
        }
    }
}

void InventorySlot::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_inventory", "inventory"), &InventorySlot::set_inventory);
    ClassDB::bind_method(D_METHOD("get_inventory"), &InventorySlot::get_inventory);
    ClassDB::bind_method(D_METHOD("set_slot_id", "slot_id"), &InventorySlot::set_slot_id);
    ClassDB::bind_method(D_METHOD("get_slot_id"), &InventorySlot::get_slot_id);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "inventory", PROPERTY_HINT_TYPE_STRING, "Inventory", PROPERTY_USAGE_NONE), "set_inventory", "get_inventory");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "slot_id"), "set_slot_id", "get_slot_id");
}

void InventorySlot::set_inventory(Ref<Inventory> inventory) {
    if (this->inventory.is_valid() && this->inventory->is_connected("item_changed", on_item_change_callable)) {
        this->inventory->disconnect("item_changed", on_item_change_callable);
    }
    inventory->connect("item_changed", on_item_change_callable);
    this->inventory = inventory;
    if (mouse_hovering) {
        SlotHelper *helper = get_default_slot_helper();
        if (helper != nullptr) {
            helper->update_tooltip(this);
        }
    }
    queue_redraw();
}

Ref<Inventory> InventorySlot::get_inventory() {
    return inventory;
}

void InventorySlot::set_slot_id(int slot_id) {
    this->slot_id = slot_id;
    if (mouse_hovering) {
        SlotHelper *helper = get_default_slot_helper();
        if (helper != nullptr) {
            helper->update_tooltip(this);
        }
    }
    queue_redraw();
}

int InventorySlot::get_slot_id() const {
    return slot_id;
}

Ref<Item> InventorySlot::peek_item() {
    if (inventory.is_null() || inventory->get_size() < get_slot_id()) {
        return Ref<Item>(nullptr);
    } else {
        return inventory->peek_slot(slot_id);
    }
}

Ref<Item> InventorySlot::take_item() {
    if (inventory.is_null() || inventory->get_size() < get_slot_id()) {
        return Ref<Item>(nullptr);
    } else {
        return inventory->take_slot(slot_id);
    }
}

bool InventorySlot::set_item(Ref<Item> other) {
    inventory->set_slot(slot_id, other);
    if (mouse_hovering) {
        SlotHelper *helper = get_default_slot_helper();
        if (helper != nullptr) {
            helper->update_tooltip(this);
        }
    }
    queue_redraw();
    return true;
}

Ref<Item> InventorySlot::swap_item(Ref<Item> other) {
    if (Item::is_empty_or_null(other)) {
        Ref<Item> output = inventory->take_slot(slot_id);
        queue_redraw();
        return output;
    } else if (other->get_id() == inventory->peek_slot(slot_id)->get_id()) {
        int remainder = inventory->add_slot(slot_id, other->clone());
        other->set_count(remainder);
        queue_redraw();
        return other;
    } else {
        Ref<Item> output = inventory->swap_item(slot_id, other);
        queue_redraw();
        return output;
    }
}

void InventorySlot::on_item_change(int slot_id, Ref<Item> new_item) {
    if (this->slot_id == slot_id) {
        if (mouse_hovering) {
            SlotHelper *helper = get_default_slot_helper();
            if (helper != nullptr) {
                helper->update_tooltip(this);
            }
        }
        queue_redraw();
    }
}

InventorySlot::InventorySlot() {
    on_item_change_callable = create_custom_callable_function_pointer(this,
#ifdef DEBUG_METHODS_ENABLED
        "on_item_change",
#endif
        &InventorySlot::on_item_change);
    inventory = Ref<Inventory>(nullptr);
    slot_id = 0;
}

InventorySlot::~InventorySlot() {

}
