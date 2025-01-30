/*
    Copyright (C) 2024 Quectel Wireless Solutions Co., Ltd.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, see
    <https://www.gnu.org/licenses/>.
*/

#ifndef __QUECTEL_LIST_H__
#define __QUECTEL_LIST_H__
struct qlistnode
{
    struct qlistnode *next;
    struct qlistnode *prev;
};

#define qnode_to_item(node, container, member) \
    (container *) (((char*) (node)) - offsetof(container, member))

#define qlist_for_each(node, list) \
    for (node = (list)->next; node != (list); node = node->next)

#define qlist_empty(list) ((list) == (list)->next)
#define qlist_head(list) ((list)->next)
#define qlist_tail(list) ((list)->prev)

static void qlist_init(struct qlistnode *node)
{
    node->next = node;
    node->prev = node;
}

static void qlist_add_tail(struct qlistnode *head, struct qlistnode *item)
{
    item->next = head;
    item->prev = head->prev;
    head->prev->next = item;
    head->prev = item;
}

static void qlist_remove(struct qlistnode *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
}
#endif