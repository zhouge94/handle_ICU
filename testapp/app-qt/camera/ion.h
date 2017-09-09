#ifndef ION_H
#define ION_H

int ion_alloc_fd(int fd, size_t len, size_t align, unsigned int heap_mask,
        unsigned int flags, int *handle_fd);

#endif // ION_H
