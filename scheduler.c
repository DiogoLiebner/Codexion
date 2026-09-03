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