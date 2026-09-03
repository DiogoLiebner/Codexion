/*
scheduler.c
   ├── int entry_compare(t_queue_entry *entry1, t_queue_entry *entry2, int scd_type)
   ├── static void bubble_up(t_queue_entry *queue, int size, int scd_type)
   ├── static void bubble_down(t_queue_entry *queue, int size, int scd_type)
   ├── void heap_insert(t_dongle *dongle, t_queue_entry entry, int scd_type)
   ├── t_queue_entry heap_pop(t_dongle *dongle, int scd_type)
   └── t_queue_entry heap_peek(t_dongle *dongle)
*/
#include "codexion.h"

int entry_compare(t_queue_entry *entry1, t_queue_entry *entry2, int scd_type)
{
	if (scd_type == 0)
	{
		if (entry1->request_time > entry2->request_time)
			return (1);
		else if (entry1->request_time == entry2->request_time)
			return (entry1->coder_id > entry2->coder_id);
		else
			return (0); 
	}
	else if (scd_type == 1)
	{
		if (entry1->deadline > entry2->deadline)
			return (1);
		else if (entry1->deadline == entry2->deadline)
			return (entry1->coder_id > entry2->coder_id);
		else
			return (0);
	}
	return (0);
}

static void bubble_up(t_queue_entry *queue, int size, int scd_type)
{
	int i;
	int parent;
	t_queue_entry temp;

	i = size - 1;
	parent = 0;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (entry_compare(&queue[parent], &queue[i], scd_type))
		{
			temp = queue[i];
			queue[i] = queue[parent];
			queue[parent] = temp;
			i = parent;
		}
		else
			break;
	}
}

static void bubble_down(t_queue_entry *queue, int size, int scd_type)
{
	int i;
	int right_child;
	int left_child;
	int smallest;
	t_queue_entry temp;

	i = 0;
	left_child = 1;
	right_child = 2;
	smallest = left_child;
	while (left_child < size)
	{
		smallest = left_child;
		if (right_child < size && entry_compare(&queue[left_child], &queue[right_child], scd_type))
			smallest = right_child;
		if (entry_compare(&queue[smallest], &queue[i], scd_type))
		{
			temp = queue[i];
			queue[i] = queue[smallest];
			queue[smallest] = temp;
			i = smallest;
		}
		else
			break;
		left_child = 2 * i + 1;
		right_child = 2 * i + 2;
	}
}

void heap_insert(t_dongle *dongle, t_queue_entry entry, int scd_type)
{
	dongle->wait_queue[dongle->queue_size] = entry;
	dongle->queue_size = dongle->queue_size + 1;
	bubble_up(dongle->wait_queue, dongle->queue_size, scd_type);
}

t_queue_entry heap_pop(t_dongle *dongle, int scd_type)
{
	t_queue_entry saved;
	saved = dongle->wait_queue[0];
	dongle->wait_queue[0] = dongle->wait_queue[dongle->queue_size];
	dongle->queue_size = dongle->queue_size - 1;
	bubble_down(dongle->wait_queue, dongle->queue_size, scd_type);
	return (saved);
}

t_queue_entry heap_peek(t_dongle *dongle)
{
	return(dongle->wait_queue[0]);
}