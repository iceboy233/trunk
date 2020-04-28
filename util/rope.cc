#include "util/rope.h"

#include <cstdint>
#include <utility>
#include "absl/random/random.h"

namespace util {

void Rope::insert(size_t index, std::string_view str) {
    rep_ = insert(std::move(rep_), index, str.data(), str.size());
}

void Rope::erase(size_t index, size_t size) {
    rep_ = erase(std::move(rep_), index, size);
}

void Rope::copy(size_t index, size_t size, char *dest) const {
    copy(*rep_, index, size, dest);
}

std::unique_ptr<Rope::Rep> Rope::insert(
    std::unique_ptr<Rep> rep, size_t index, const char *buffer, size_t size) {
    if (!rep) {
        // TODO(iceboy): Use shared bitgens.
        static thread_local absl::BitGen gen;

        rep = std::make_unique<Rep>();
        rep->priority = absl::Uniform<uint32_t>(gen);
    }
    if (index < rep->lsize) {
        rep->lchild = insert(std::move(rep->lchild), index, buffer, size);
        rep->lsize += size;
        return balance_left(std::move(rep));
    }
    index -= rep->lsize;
    if (index > rep->size) {
        rep->rchild = insert(
            std::move(rep->rchild), index - rep->size, buffer, size);
        return balance_right(std::move(rep));
    }
    // [rep->buffer, rep->buffer + index) : index
    // [buffer, buffer + size) : size
    // [rep->buffer + index, rep->buffer + rep->size) : rep->size - index
    if (rep->size + size > buffer_size) {
        size_t new_rep_size = std::max(buffer_size, index + size) - size;
        rep->rchild = insert(
            std::move(rep->rchild), 0, rep->buffer + new_rep_size,
            rep->size - new_rep_size);
        rep->size = new_rep_size;
        if (index + size > buffer_size) {
            size_t new_size = buffer_size - index;
            rep->rchild = insert(
                std::move(rep->rchild), 0, buffer + new_size, size - new_size);
            size = new_size;
        }
    }
    memmove(rep->buffer + index + size, rep->buffer + index, rep->size - index);
    memcpy(rep->buffer + index, buffer, size);
    rep->size += size;
    return balance_right(std::move(rep));
}

std::unique_ptr<Rope::Rep> Rope::erase(
    std::unique_ptr<Rep> rep, size_t index, size_t size) {
    if (index < rep->lsize) {
        size_t erase_size = std::min(size, rep->lsize - index);
        rep->lchild = erase(std::move(rep->lchild), index, erase_size);
        rep->lsize -= erase_size;
        size -= erase_size;
        if (!size) {
            goto out;
        }
        index = 0;
    } else {
        index -= rep->lsize;
    }
    if (index < rep->size) {
        size_t erase_size = std::min(size, rep->size - index);
        memmove(rep->buffer + index, rep->buffer + index + erase_size,
                rep->size - (index + erase_size));
        rep->size -= erase_size;
        size -= erase_size;
        if (!size) {
            goto out;
        }
        index = 0;
    } else {
        index -= rep->size;
    }
    rep->rchild = erase(std::move(rep->rchild), index, size);
out:
    if (!rep->size) {
        rep = merge(std::move(rep->lchild), std::move(rep->rchild));
    }
    return rep;
}

void Rope::copy(const Rep &rep, size_t index, size_t size, char *dest) {
    if (index < rep.lsize) {
        size_t get_size = std::min(size, rep.lsize - index);
        copy(*rep.lchild, index, get_size, dest);
        dest += get_size;
        size -= get_size;
        if (!size) {
            return;
        }
        index = 0;
    } else {
        index -= rep.lsize;
    }
    if (index < rep.size) {
        size_t get_size = std::min(size, rep.size - index);
        memcpy(dest, rep.buffer + index, get_size);
        dest += get_size;
        size -= get_size;
        if (!size) {
            return;
        }
        index = 0;
    } else {
        index -= rep.size;
    }
    copy(*rep.rchild, index, size, dest);
}

std::unique_ptr<Rope::Rep> Rope::merge(
    std::unique_ptr<Rep> lrep, std::unique_ptr<Rep> rrep) {
    if (!lrep) {
        return rrep;
    }
    if (!rrep) {
        return lrep;
    }
    lrep->rchild = merge(std::move(lrep->rchild), std::move(rrep));
    return balance_right(std::move(lrep));
}

std::unique_ptr<Rope::Rep> Rope::balance_left(std::unique_ptr<Rep> rep) {
    if (rep->lchild) {
        rep->lchild = balance_left(std::move(rep->lchild));
        if (rep->lchild->priority > rep->priority) {
            rep = rotate_right(std::move(rep));
        }
    }
    return rep;
}

std::unique_ptr<Rope::Rep> Rope::balance_right(std::unique_ptr<Rep> rep) {
    if (rep->rchild) {
        rep->rchild = balance_right(std::move(rep->rchild));
        if (rep->rchild->priority > rep->priority) {
            rep = rotate_left(std::move(rep));
        }
    }
    return rep;
}

std::unique_ptr<Rope::Rep> Rope::rotate_left(std::unique_ptr<Rep> rep) {
    std::unique_ptr<Rep> peer = std::move(rep->rchild);
    rep->rchild = std::move(peer->lchild);
    peer->lsize += rep->lsize + rep->size;
    peer->lchild = std::move(rep);
    return peer;
}

std::unique_ptr<Rope::Rep> Rope::rotate_right(std::unique_ptr<Rep> rep) {
    std::unique_ptr<Rep> peer = std::move(rep->lchild);
    rep->lchild = std::move(peer->rchild);
    rep->lsize -= peer->lsize + peer->size;
    peer->rchild = std::move(rep);
    return peer;
}

}  // namespace util
