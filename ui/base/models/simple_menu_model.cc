// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/models/simple_menu_model.h"

#include "base/message_loop.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/base/l10n/l10n_util.h"

namespace ui {

const int kSeparatorId = -1;

// The instance is alive.
static const uint32 kMagicIdAlive = 0xCa11ab1e;

  // The instance has been deleted.
static const uint32 kMagicIdDead = 0xDECEA5ED;

struct SimpleMenuModel::Item {

// TODO(sky): Remove this when done investigating 95851.
#if defined(COMPILER_MSVC)
#pragma optimize("", off)
MSVC_PUSH_DISABLE_WARNING(4748)
#endif

  ~Item() {
    CHECK_EQ(magic_id, kMagicIdAlive);
    magic_id = kMagicIdDead;
  }

#if defined(COMPILER_MSVC)
MSVC_POP_WARNING()
#pragma optimize("", on)
#endif

  int command_id;
  string16 label;
  SkBitmap icon;
  ItemType type;
  int group_id;
  MenuModel* submenu;
  ButtonMenuItemModel* button_model;
  uint32 magic_id;
};

////////////////////////////////////////////////////////////////////////////////
// SimpleMenuModel::Delegate, public:

bool SimpleMenuModel::Delegate::IsCommandIdVisible(int command_id) const {
  return true;
}

bool SimpleMenuModel::Delegate::IsItemForCommandIdDynamic(
    int command_id) const {
  return false;
}

string16 SimpleMenuModel::Delegate::GetLabelForCommandId(int command_id) const {
  return string16();
}

bool SimpleMenuModel::Delegate::GetIconForCommandId(
    int command_id, SkBitmap* bitmap) const {
  return false;
}

void SimpleMenuModel::Delegate::CommandIdHighlighted(int command_id) {
}

void SimpleMenuModel::Delegate::ExecuteCommand(
    int command_id, int event_flags) {
  ExecuteCommand(command_id);
}

void SimpleMenuModel::Delegate::MenuWillShow(SimpleMenuModel* /*source*/) {
}

void SimpleMenuModel::Delegate::MenuClosed(SimpleMenuModel* /*source*/) {
}

////////////////////////////////////////////////////////////////////////////////
// SimpleMenuModel, public:

SimpleMenuModel::SimpleMenuModel(Delegate* delegate)
    : delegate_(delegate),
      menu_model_delegate_(NULL),
      ALLOW_THIS_IN_INITIALIZER_LIST(method_factory_(this)) {
}

SimpleMenuModel::~SimpleMenuModel() {
  for (size_t i = 0; i < items_.size(); ++i)
    CHECK_EQ(kMagicIdAlive, items_[i].magic_id) << i;
}

void SimpleMenuModel::AddItem(int command_id, const string16& label) {
  Item item = { command_id, label, SkBitmap(), TYPE_COMMAND, -1, NULL, NULL,
                kMagicIdAlive};
  AppendItem(item);
}

void SimpleMenuModel::AddItemWithStringId(int command_id, int string_id) {
  AddItem(command_id, l10n_util::GetStringUTF16(string_id));
}

void SimpleMenuModel::AddSeparator() {
  Item item = { kSeparatorId, string16(), SkBitmap(), TYPE_SEPARATOR, -1,
                NULL, NULL, kMagicIdAlive };
  AppendItem(item);
}

void SimpleMenuModel::AddCheckItem(int command_id, const string16& label) {
  Item item = { command_id, label, SkBitmap(), TYPE_CHECK, -1, NULL,
                NULL, kMagicIdAlive };
  AppendItem(item);
}

void SimpleMenuModel::AddCheckItemWithStringId(int command_id, int string_id) {
  AddCheckItem(command_id, l10n_util::GetStringUTF16(string_id));
}

void SimpleMenuModel::AddRadioItem(int command_id, const string16& label,
                                   int group_id) {
  Item item = { command_id, label, SkBitmap(), TYPE_RADIO, group_id, NULL,
                NULL, kMagicIdAlive };
  AppendItem(item);
}

void SimpleMenuModel::AddRadioItemWithStringId(int command_id, int string_id,
                                               int group_id) {
  AddRadioItem(command_id, l10n_util::GetStringUTF16(string_id), group_id);
}

void SimpleMenuModel::AddButtonItem(int command_id,
                                    ButtonMenuItemModel* model) {
  Item item = { command_id, string16(), SkBitmap(), TYPE_BUTTON_ITEM, -1, NULL,
                model, kMagicIdAlive };
  AppendItem(item);
}

void SimpleMenuModel::AddSubMenu(int command_id, const string16& label,
                                 MenuModel* model) {
  Item item = { command_id, label, SkBitmap(), TYPE_SUBMENU, -1, model, NULL,
                kMagicIdAlive };
  AppendItem(item);
}

void SimpleMenuModel::AddSubMenuWithStringId(int command_id,
                                             int string_id, MenuModel* model) {
  AddSubMenu(command_id, l10n_util::GetStringUTF16(string_id), model);
}

void SimpleMenuModel::InsertItemAt(
    int index, int command_id, const string16& label) {
  Item item = { command_id, label, SkBitmap(), TYPE_COMMAND, -1, NULL, NULL,
                kMagicIdAlive };
  InsertItemAtIndex(item, index);
}

void SimpleMenuModel::InsertItemWithStringIdAt(
    int index, int command_id, int string_id) {
  InsertItemAt(index, command_id, l10n_util::GetStringUTF16(string_id));
}

void SimpleMenuModel::InsertSeparatorAt(int index) {
  Item item = { kSeparatorId, string16(), SkBitmap(), TYPE_SEPARATOR, -1,
                NULL, NULL, kMagicIdAlive };
  InsertItemAtIndex(item, index);
}

void SimpleMenuModel::InsertCheckItemAt(
    int index, int command_id, const string16& label) {
  Item item = { command_id, label, SkBitmap(), TYPE_CHECK, -1, NULL, NULL,
                kMagicIdAlive };
  InsertItemAtIndex(item, index);
}

void SimpleMenuModel::InsertCheckItemWithStringIdAt(
    int index, int command_id, int string_id) {
  InsertCheckItemAt(
      FlipIndex(index), command_id, l10n_util::GetStringUTF16(string_id));
}

void SimpleMenuModel::InsertRadioItemAt(
    int index, int command_id, const string16& label, int group_id) {
  Item item = { command_id, label, SkBitmap(), TYPE_RADIO, group_id, NULL,
                NULL, kMagicIdAlive };
  InsertItemAtIndex(item, index);
}

void SimpleMenuModel::InsertRadioItemWithStringIdAt(
    int index, int command_id, int string_id, int group_id) {
  InsertRadioItemAt(
      index, command_id, l10n_util::GetStringUTF16(string_id), group_id);
}

void SimpleMenuModel::InsertSubMenuAt(
    int index, int command_id, const string16& label, MenuModel* model) {
  Item item = { command_id, label, SkBitmap(), TYPE_SUBMENU, -1, model, NULL,
                kMagicIdAlive };
  InsertItemAtIndex(item, index);
}

void SimpleMenuModel::InsertSubMenuWithStringIdAt(
    int index, int command_id, int string_id, MenuModel* model) {
  InsertSubMenuAt(index, command_id, l10n_util::GetStringUTF16(string_id),
                  model);
}

void SimpleMenuModel::SetIcon(int index, const SkBitmap& icon) {
  items_[index].icon = icon;
}

void SimpleMenuModel::Clear() {
  items_.clear();
}

int SimpleMenuModel::GetIndexOfCommandId(int command_id) {
  std::vector<Item>::iterator itr;
  for (itr = items_.begin(); itr != items_.end(); itr++) {
    if ((*itr).command_id == command_id) {
      return FlipIndex(static_cast<int>(std::distance(items_.begin(), itr)));
    }
  }
  return -1;
}

////////////////////////////////////////////////////////////////////////////////
// SimpleMenuModel, MenuModel implementation:

bool SimpleMenuModel::HasIcons() const {
  for (std::vector<Item>::const_iterator iter = items_.begin();
       iter != items_.end(); ++iter) {
    if (!iter->icon.isNull())
      return true;
  }

  return false;
}

int SimpleMenuModel::GetItemCount() const {
  return static_cast<int>(items_.size());
}

MenuModel::ItemType SimpleMenuModel::GetTypeAt(int index) const {
  return items_.at(FlipIndex(index)).type;
}

int SimpleMenuModel::GetCommandIdAt(int index) const {
  return items_.at(FlipIndex(index)).command_id;
}

string16 SimpleMenuModel::GetLabelAt(int index) const {
  if (IsItemDynamicAt(index))
    return delegate_->GetLabelForCommandId(GetCommandIdAt(index));
  return items_.at(FlipIndex(index)).label;
}

bool SimpleMenuModel::IsItemDynamicAt(int index) const {
  if (delegate_)
    return delegate_->IsItemForCommandIdDynamic(GetCommandIdAt(index));
  return false;
}

bool SimpleMenuModel::GetAcceleratorAt(int index,
                                       ui::Accelerator* accelerator) const {
  if (delegate_) {
    return delegate_->GetAcceleratorForCommandId(GetCommandIdAt(index),
                                                 accelerator);
  }
  return false;
}

bool SimpleMenuModel::IsItemCheckedAt(int index) const {
  if (!delegate_)
    return false;
  int item_index = FlipIndex(index);
  MenuModel::ItemType item_type = items_[item_index].type;
  return (item_type == TYPE_CHECK || item_type == TYPE_RADIO) ?
      delegate_->IsCommandIdChecked(GetCommandIdAt(index)) : false;
}

int SimpleMenuModel::GetGroupIdAt(int index) const {
  return items_.at(FlipIndex(index)).group_id;
}

bool SimpleMenuModel::GetIconAt(int index, SkBitmap* icon) {
  if (IsItemDynamicAt(index))
    return delegate_->GetIconForCommandId(GetCommandIdAt(index), icon);

  if (items_[index].icon.isNull())
    return false;

  *icon = items_[index].icon;
  return true;
}

ButtonMenuItemModel* SimpleMenuModel::GetButtonMenuItemAt(int index) const {
  return items_.at(FlipIndex(index)).button_model;
}

bool SimpleMenuModel::IsEnabledAt(int index) const {
  int command_id = GetCommandIdAt(index);
  if (!delegate_ || command_id == kSeparatorId ||
      items_.at(FlipIndex(index)).button_model)
    return true;
  return delegate_->IsCommandIdEnabled(command_id);
}

bool SimpleMenuModel::IsVisibleAt(int index) const {
  int command_id = GetCommandIdAt(index);
  if (!delegate_ || command_id == kSeparatorId ||
      items_.at(FlipIndex(index)).button_model)
    return true;
  return delegate_->IsCommandIdVisible(command_id);
}

void SimpleMenuModel::HighlightChangedTo(int index) {
  if (delegate_)
    delegate_->CommandIdHighlighted(GetCommandIdAt(index));
}

void SimpleMenuModel::ActivatedAt(int index) {
  if (delegate_)
    delegate_->ExecuteCommand(GetCommandIdAt(index));
}

void SimpleMenuModel::ActivatedAt(int index, int event_flags) {
  if (delegate_)
    delegate_->ExecuteCommand(GetCommandIdAt(index), event_flags);
}

MenuModel* SimpleMenuModel::GetSubmenuModelAt(int index) const {
  return items_.at(FlipIndex(index)).submenu;
}

void SimpleMenuModel::MenuWillShow() {
  if (delegate_)
    delegate_->MenuWillShow(this);
}

void SimpleMenuModel::MenuClosed() {
  // Due to how menus work on the different platforms, ActivatedAt will be
  // called after this.  It's more convenient for the delegate to be called
  // afterwards though, so post a task.
  MessageLoop::current()->PostTask(
      FROM_HERE,
      method_factory_.NewRunnableMethod(&SimpleMenuModel::OnMenuClosed));
}

void SimpleMenuModel::SetMenuModelDelegate(
      ui::MenuModelDelegate* menu_model_delegate) {
  menu_model_delegate_ = menu_model_delegate;
}

void SimpleMenuModel::OnMenuClosed() {
  if (delegate_)
    delegate_->MenuClosed(this);
}

int SimpleMenuModel::FlipIndex(int index) const {
  return index;
}

////////////////////////////////////////////////////////////////////////////////
// SimpleMenuModel, Private:

void SimpleMenuModel::AppendItem(const Item& item) {
  ValidateItem(item);
  items_.push_back(item);
}

void SimpleMenuModel::InsertItemAtIndex(const Item& item, int index) {
  ValidateItem(item);
  items_.insert(items_.begin() + FlipIndex(index), item);
}

void SimpleMenuModel::ValidateItem(const Item& item) {
#ifndef NDEBUG
  if (item.type == TYPE_SEPARATOR) {
    DCHECK_EQ(item.command_id, kSeparatorId);
  } else {
    DCHECK_GE(item.command_id, 0);
  }
#endif  // NDEBUG
}

}  // namespace ui
