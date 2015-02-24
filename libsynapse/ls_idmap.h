#ifndef _LS_IDMAP_H_
#define _LS_IDMAP_H_

int id_queue_len(void);
unsigned long get_next_id(void);
int add_id_to_waiting_queue(unsigned long id);
int wait_for_id(unsigned long id);
int process_id_message(void *ptr);
void * find_ptr_in_map(unsigned long id, int waitforit);
int find_id_in_map(void *ptr);
void remove_ptr_from_map(void *ptr);

#endif
