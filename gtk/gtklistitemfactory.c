/*
 * Copyright © 2018 Benjamin Otte
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Benjamin Otte <otte@gnome.org>
 */

#include "config.h"

#include "gtklistitemfactoryprivate.h"

#include "gtklistitemprivate.h"

G_DEFINE_TYPE (GtkListItemFactory, gtk_list_item_factory, G_TYPE_OBJECT)

static void
gtk_list_item_factory_default_setup (GtkListItemFactory *self,
                                     GtkListItem        *list_item)
{
  if (self->setup_func)
    self->setup_func (list_item, self->user_data);
}

static void
gtk_list_item_factory_default_teardown (GtkListItemFactory *self,
                                        GtkListItem        *list_item)
{
  GtkWidget *child;

  child = gtk_bin_get_child (GTK_BIN (list_item));
  if (child)
    gtk_container_remove (GTK_CONTAINER (list_item), child);

  gtk_list_item_set_selectable (list_item, TRUE);

}

static void                  
gtk_list_item_factory_default_bind (GtkListItemFactory *self,
                                    GtkListItem        *list_item,
                                    guint               position,
                                    gpointer            item,
                                    gboolean            selected)
{
  gtk_list_item_set_item (list_item, item);
  gtk_list_item_set_position (list_item, position);
  gtk_list_item_set_selected (list_item, selected);

  if (self->bind_func)  
    self->bind_func (list_item, self->user_data);
}

static void
gtk_list_item_factory_default_rebind (GtkListItemFactory *self,
                                      GtkListItem        *list_item,
                                      guint               position,
                                      gpointer            item,
                                      gboolean            selected)
{
  gtk_list_item_set_item (list_item, item);
  gtk_list_item_set_position (list_item, position);
  gtk_list_item_set_selected (list_item, selected);

  if (self->bind_func)  
    self->bind_func (list_item, self->user_data);
}

static void
gtk_list_item_factory_default_update (GtkListItemFactory *self,
                                      GtkListItem        *list_item,
                                      guint               position,
                                      gboolean            selected)
{
  gtk_list_item_set_position (list_item, position);
  gtk_list_item_set_selected (list_item, selected);
}

static void
gtk_list_item_factory_default_unbind (GtkListItemFactory *self,
                                      GtkListItem        *list_item)
{
  gtk_list_item_set_item (list_item, NULL);
  gtk_list_item_set_position (list_item, 0);
  gtk_list_item_set_selected (list_item, FALSE);
}

static void
gtk_list_item_factory_finalize (GObject *object)
{
  GtkListItemFactory *self = GTK_LIST_ITEM_FACTORY (object);

  if (self->user_destroy)
    self->user_destroy (self->user_data);

  G_OBJECT_CLASS (gtk_list_item_factory_parent_class)->finalize (object);
}

static void
gtk_list_item_factory_class_init (GtkListItemFactoryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtk_list_item_factory_finalize;

  klass->setup = gtk_list_item_factory_default_setup;
  klass->teardown = gtk_list_item_factory_default_teardown;
  klass->bind = gtk_list_item_factory_default_bind;
  klass->rebind = gtk_list_item_factory_default_rebind;
  klass->update = gtk_list_item_factory_default_update;
  klass->unbind = gtk_list_item_factory_default_unbind;
}

static void
gtk_list_item_factory_init (GtkListItemFactory *self)
{
}

GtkListItemFactory *
gtk_list_item_factory_new (GtkListItemSetupFunc setup_func,
                           GtkListItemBindFunc  bind_func,
                           gpointer             user_data,
                           GDestroyNotify       user_destroy)
{
  GtkListItemFactory *self;

  g_return_val_if_fail (setup_func || bind_func, NULL);
  g_return_val_if_fail (user_data != NULL || user_destroy == NULL, NULL);

  self = g_object_new (GTK_TYPE_LIST_ITEM_FACTORY, NULL);

  self->setup_func = setup_func;
  self->bind_func = bind_func;
  self->user_data = user_data;
  self->user_destroy = user_destroy;

  return self;
}

void
gtk_list_item_factory_setup (GtkListItemFactory *self,
                             GtkListItem        *list_item)
{
  g_return_if_fail (GTK_IS_LIST_ITEM_FACTORY (self));

  GTK_LIST_ITEM_FACTORY_GET_CLASS (self)->setup (self, list_item);
}

void
gtk_list_item_factory_teardown (GtkListItemFactory *self,
                                GtkListItem        *list_item)
{
  g_return_if_fail (GTK_IS_LIST_ITEM_FACTORY (self));

  GTK_LIST_ITEM_FACTORY_GET_CLASS (self)->teardown (self, list_item);
}

void
gtk_list_item_factory_bind (GtkListItemFactory *self,
                            GtkListItem        *list_item,
                            guint               position,
                            gpointer            item,
                            gboolean            selected)
{
  g_return_if_fail (GTK_IS_LIST_ITEM_FACTORY (self));
  g_return_if_fail (GTK_IS_LIST_ITEM (list_item));

  g_object_freeze_notify (G_OBJECT (list_item));

  GTK_LIST_ITEM_FACTORY_GET_CLASS (self)->bind (self, list_item, position, item, selected);

  g_object_thaw_notify (G_OBJECT (list_item));
}

void
gtk_list_item_factory_rebind (GtkListItemFactory *self,
                              GtkListItem        *list_item,
                              guint               position,
                              gpointer            item,
                              gboolean            selected)
{
  g_return_if_fail (GTK_IS_LIST_ITEM_FACTORY (self));
  g_return_if_fail (GTK_IS_LIST_ITEM (list_item));

  g_object_freeze_notify (G_OBJECT (list_item));

  GTK_LIST_ITEM_FACTORY_GET_CLASS (self)->rebind (self, list_item, position, item, selected);

  g_object_thaw_notify (G_OBJECT (list_item));
}

void
gtk_list_item_factory_update (GtkListItemFactory *self,
                              GtkListItem        *list_item,
                              guint               position,
                              gboolean            selected)
{
  g_return_if_fail (GTK_IS_LIST_ITEM_FACTORY (self));
  g_return_if_fail (GTK_IS_LIST_ITEM (list_item));

  g_object_freeze_notify (G_OBJECT (list_item));

  GTK_LIST_ITEM_FACTORY_GET_CLASS (self)->update (self, list_item, position, selected);

  g_object_thaw_notify (G_OBJECT (list_item));
}

void
gtk_list_item_factory_unbind (GtkListItemFactory *self,
                              GtkListItem        *list_item)
{
  g_return_if_fail (GTK_IS_LIST_ITEM_FACTORY (self));
  g_return_if_fail (GTK_IS_LIST_ITEM (list_item));

  g_object_freeze_notify (G_OBJECT (list_item));

  GTK_LIST_ITEM_FACTORY_GET_CLASS (self)->unbind (self, list_item);

  g_object_thaw_notify (G_OBJECT (list_item));
}