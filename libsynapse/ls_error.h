#ifndef _LS_ERROR_H_
#define _LS_ERROR_H_

int error_queue_len(void);
int next_error(void);
int add_error_to_queue(int error);

#endif
