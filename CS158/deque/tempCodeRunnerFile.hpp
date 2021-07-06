while(b->cur_size < (2 * block_size) && b->next != tailer){
                tmp_b = b->next;
                b->cur_size += tmp_b->cur_size;
                tmp_b->next->last = b;
                b->next = tmp_b->next;
                delete tmp_b;
            }