#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/vfs/vfs.h>
#include <stdio.h>

using namespace kernel;
using namespace kernel::tasks;

namespace kernel {
namespace vfs {

virtual_filesystem g_vfs;

/// The null delegate implements no file operations.
class null_delegate : public vfs_delegate {
   public:
    null_delegate() {}
    int         read(vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }
    int         write(vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }
    char const* delegate_name() { return "null delegate"; }
};

void virtual_filesystem::init() {
    auto* vfs_root = new vfs_node(nullptr, new null_delegate(), vfs_type::directory, 0);
    vfs_root->set_name("/");
    m_root = vfs_root;

    auto* dev = new vfs_node(m_root, new null_delegate(), vfs_type::directory, 0);
    dev->set_name("dev");
}

vfs_node* virtual_filesystem::find(kernel::string path, int flags) {
    auto* directory = m_root;

    kernel::log::debug("vfs", "Finding %s\n", path.data());
    if (path.find('/') == 0) { path = path.substr(1); }

    while (path.find('/') != kernel::string::npos) {
        size_t delim_pos    = path.find('/');
        auto   before_delim = path.substr(0, delim_pos);

        // This is a directory, which means its a child of the parent.
        if (delim_pos == (path.size() - 1)) {
            path = before_delim;
            break;
        }

        // Check the prospective parents children to see if this directory is one of them.
        for (auto* c = directory->children.front(); c != nullptr; c = c->m_next) {
            if (strcmp(c->val->name(), before_delim.data()) == 0) {
                directory = c->val;
                break;
            }
        }
        path = path.substr(delim_pos + 1);
    }

    // Search all the children of directory
    for (auto* c = directory->children.front(); c != nullptr; c = c->m_next) {
        if (strcmp(c->val->name(), path.data()) == 0) {
            if (flags & VFS_FIND_PARENT) { return c->val->parent; }
            return c->val;
        }
    }

    // We didn't find it, return nullptr.
    return nullptr;
}

file_id_t virtual_filesystem::open_file(kernel::string path, int flags) {
    task* current_task = scheduler::get_current_task();

    auto* file = find(path);

    if (flags & FILE_CREATE) {
        if (file == nullptr) {
            auto* parent = find(path, VFS_FIND_FILE);
            if (parent == nullptr) { return -1; }

            if (flags & FILE_TEMPORARY) { file = new vfs_node(parent, nullptr, vfs_type::file, 0); }
        }
    }

    if (file == nullptr) { return -1; }

    file_descriptor* descriptor = new file_descriptor();
    descriptor->flags           = flags;
    descriptor->m_id            = next_descriptor_id++;
    descriptor->m_node          = file;
    open_files.push_back(descriptor);

    // Now, in the current task, we'll open this file.
    auto* task_fd         = new task_file_descriptor();
    task_fd->m_descriptor = descriptor;
    task_fd->m_local_id   = current_task->next_fd_id;
    current_task->next_fd_id++;
    current_task->m_file_descriptors.push_back(task_fd);
    return task_fd->m_local_id;
}

file_descriptor* virtual_filesystem::taskfd_to_fd(file_id_t id) {
    task* current_task = scheduler::get_current_task();

    for (auto fd : current_task->m_file_descriptors) {
        if (fd.m_local_id == id) { return fd.m_descriptor; }
    }
    return nullptr;
}

size_t virtual_filesystem::read(file_id_t fdid, uint8_t* buf, size_t count) {
    auto* fd = taskfd_to_fd(fdid);
    if (fd == nullptr || fd->m_node == nullptr) { return -1; }

    auto* node = fd->m_node;

    if (count > node->size) { count = node->size; }
    if (count == 0) { return 0; }
    if (buf == nullptr) { return -1; }

    if (node->delegate == nullptr) { return -1; }

    return node->delegate->read(node, 0, count, buf);
}

size_t virtual_filesystem::write(file_id_t fdid, uint8_t* buf, size_t count) {
    auto* fd = taskfd_to_fd(fdid);
    if (fd == nullptr || fd->m_node == nullptr) { return -1; }

    auto* node = fd->m_node;

    if (count == 0) { return 0; }
    // We don't care about limits in devices
    if (node->type != vfs_type::device) {
        if (count > node->size) { count = node->size; }
    }

    if (buf == nullptr) { return -1; }

    if (node->delegate == nullptr) { return -1; }
    return node->delegate->write(node, 0, count, buf);
}

file_stats virtual_filesystem::fstat(file_id_t fdid) {
    auto* fd = taskfd_to_fd(fdid);
    if (fd == nullptr || fd->m_node == nullptr) { return file_stats(); }

    file_stats stats;
    stats.size = fd->m_node->size;

    return stats;
}

}  // namespace vfs
}  // namespace kernel
