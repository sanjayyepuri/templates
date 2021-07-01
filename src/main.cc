#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "liburing.h"

#include <errno.h>
#include <linux/io_uring.h>
#include <stddef.h>
#include <sys/syscall.h>

#define QD 4

using namespace std;


int main() {
    io_uring ring; 
    int i, fd, ret, pending, done; 
    io_uring_sqe *sqe;
    io_uring_cqe *cqe; 
    iovec *iovecs; 
    struct stat sb;
    ssize_t fsize; 
    off_t offset;
    void *buf;

    ret = io_uring_queue_init(QD, &ring, 0);
    if (ret < 0) {
        printf("queue_init: %s\n", strerror(-ret));
        return 1;
    }

    fd = open("test.txt", O_RDONLY| O_DIRECT);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (fstat(fd, &sb) < 0) {
        perror("fstat");
        return 1; 
    }

    fsize = 0;
    iovecs = (iovec *)calloc(QD, sizeof(struct iovec));
    for (i = 0; i < QD; ++i) {
        if (posix_memalign(&buf, 4096, 4096)) 
            return 1;
        iovecs[i].iov_base = buf; 
        iovecs[i].iov_len = 4096;
        fsize += 4096;
    }

    offset = 0;
    i = 0;
    do {
        sqe = io_uring_get_sqe(&ring);
        if(!sqe) break;

        io_uring_prep_readv(sqe, fd, &iovecs[i], 1, offset);
        offset += iovecs[i].iov_len;

        i++;

        if (offset > sb.st_size) break;
    } while (1); 


    ret = io_uring_submit(&ring);
    if (ret < 0) {
        printf("io_uring_submit: %s\n", strerror(-ret));
        return 1;
    } else if (ret != i) {
        printf("io_uring_submit submitted less %d\n", ret);
    }

    done = 0;
    pending = ret;
    fsize = 0;
    for (i = 0; i < pending; ++i) {
        ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            printf("io_uring_wait_cqe: %s\n", strerror(-ret));
            return -1;
        }

        done++;
        ret = 0;
        if (cqe->res != 4096 && cqe->res + fsize != sb.st_size) {
            printf("ret=%d, wanted 4096\n", cqe->res);
            return 1;
        }

        fsize += cqe->res;
        io_uring_cqe_seen(&ring, cqe);
        if (ret) break;
    }

    printf("Submitted=%d, completed=%d, bytes=%lu\n", pending, done,
        (unsigned long) fsize);
    
    close(fd);
    io_uring_queue_exit(&ring);

    return 0;
}