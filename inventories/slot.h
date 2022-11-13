#ifndef SLOT_H
#define SLOT_H

#include "core/math/random_number_generator.h"
#include "core/object/ref_counted.h"
#include "core/variant/typed_array.h"
#include "core/string/ustring.h"
#include "core/string/string_name.h"
#include "core/io/resource.h"
#include "scene/gui/control.h"
#include "scene/2d/node_2d.h"
#include "core/object/callable_method_pointer.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/label.h"

#include "item.h"
#include "inventory.h"

class SlotHelper;

class AbstractSlot : public Control {
    GDCLASS(AbstractSlot, Control);
    OBJ_SAVE_TYPE(AbstractSlot);

private:
	struct ThemeCache {
		Ref<StyleBox> normal_style;
		Ref<StyleBox> hover_style;
        Ref<StyleBox> disabled_style;
		Ref<Font> font;

		int font_size = 0;
		Color font_color;
        int min_size = 0;
	} theme_cache;
protected:
    static void _bind_methods();
    bool mouse_hovering;
    bool mouse_pressed;
    bool disabled;

	virtual void _update_theme_item_cache() override;
    Size2 get_minimum_size() const override;
public:
    SlotHelper *get_default_slot_helper();
    void _notification(int notification);
    void set_mouse_hovering(bool mouse_hovering);
    bool is_mouse_hovering() const;
    void gui_input(const Ref<InputEvent> &p_event) override;
    bool is_disabled() const;
    void set_disabled(bool disabled);

    GDVIRTUAL0RC(Ref<Item>, _peek_item);
    GDVIRTUAL0RC(Ref<Item>, _take_item);
    GDVIRTUAL1RC(Ref<Item>, _swap_item, Ref<Item>);
    GDVIRTUAL1RC(bool, _set_item, Ref<Item>);

    // Must be overriden to work.
    virtual Ref<Item> peek_item();
    // Uses peek_item if not overridden.
    virtual Ref<Item> take_item();
    // Uses take_item and set_item if not overridden.
    virtual Ref<Item> swap_item(Ref<Item> other);
    // Must be overriden to work.
    virtual bool set_item(Ref<Item> other);

    AbstractSlot();
    ~AbstractSlot();
};

class Slot : public AbstractSlot {
    GDCLASS(Slot, AbstractSlot);
protected:
    static void _bind_methods();
    
    Ref<Item> item;

public:
    Ref<Item> peek_item() override;
    bool set_item(Ref<Item> item) override;

    Slot();
    ~Slot();
};

class SlotHelper : public Slot {
    GDCLASS(SlotHelper, Slot);
    friend class AbstractSlot;
protected:
    static void _bind_methods();
    
    static SlotHelper *default_helper;
    static Vector<SlotHelper*> helpers;
    AbstractSlot *tooltip_slot;
    PanelContainer *tooltip_container;
    Label *tooltip_label;
public:
    void _notification(int notification);

    void show_tooltip(AbstractSlot *slot);
    void update_tooltip(AbstractSlot *slot);
    void hide_tooltip(AbstractSlot *slot);

    void swap_with_slot(AbstractSlot *slot);

    SlotHelper();
    ~SlotHelper();
};

class InventorySlot : public AbstractSlot {
    GDCLASS(InventorySlot, AbstractSlot);

protected:
    static void _bind_methods();
    Ref<Inventory> inventory;
    int slot_id;

    // Item change event handler
    Callable on_item_change_callable;
    void on_item_change(int slot_id, Ref<Item> new_item);

public:
    Ref<Item> peek_item() override;
    Ref<Item> take_item() override;
    bool set_item(Ref<Item> other) override;
    Ref<Item> swap_item(Ref<Item> other) override;
    void set_inventory(Ref<Inventory> inventory);
    Ref<Inventory> get_inventory();
    void set_slot_id(int slot_id);
    int get_slot_id() const;

    InventorySlot();
    ~InventorySlot();
};

#endif // SLOT_H