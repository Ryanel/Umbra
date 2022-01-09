#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/vfs/vfs.h>
#include <stdio.h>

#include <algorithm>
#include <string_view>

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

vfs_node* virtual_filesystem::find(std::string_view path, int flags) {
    auto* directory = m_root;

    std::string_view path_view(path.data());

    kernel::log::debug("vfs", "Finding %s\n", path_view.data());
    if (path_view.find('/') == 0) { path_view = path_view.substr(1); }

    while (path_view.find('/') != std::string::npos) {
        size_t delim_pos    = path_view.find('/');
        auto   before_delim = path_view.substr(0, delim_pos);

        // This is a directory, which means its a child of the parent.
        if (delim_pos == (path_view.size() - 1)) {
            path_view = before_delim;
            break;
        }

        // Check the prospective parents children to see if this directory is one of them.
        auto child_dir_it = std::find_if(directory->children.begin(), directory->children.end(),
                                         [before_delim](vfs_node* n) { return before_delim.compare(n->name()) == 0; });

        if (child_dir_it == directory->children.end()) {
            return nullptr;  // We did not find it...
        }

        directory = *child_dir_it;
        path_view = path_view.substr(delim_pos + 1);
    }

    // Search all the children of directory
    for (auto&& i : directory->children) {
        if (path_view.compare(i->name()) == 0) {
            if (flags & VFS_FIND_PARENT) { return i->parent; }
            return i;
        }
    }

    // We didn't find it, return nullptr.
    return nullptr;
}

file_id_t virtual_filesystem::open_file(std::string_view path, int flags) {
    task* current_task = scheduler::get_current_task();

    auto* file = find(path);

    if (flags & FILE_CREATE) {
        if (file == nullptr) {
            auto* parent = find(path, VFS_FIND_PARENT);
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
    auto task_fd         = task_file_descriptor();
    task_fd.m_descriptor = descriptor;
    task_fd.m_local_id   = current_task->next_fd_id;
    current_task->next_fd_id++;
    current_task->m_file_descriptors.push_back(task_fd);
    return task_fd.m_local_id;
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

    assert(buf != nullptr);
    assert(node != nullptr);
    assert(node->delegate != nullptr);

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

void virtual_filesystem::debug(vfs_node* node, int depth) {
    if (depth > 40) { return; }

    int padding = 11;

    for (int i = 0; i < depth + padding; i++) { kernel::log::get().write(' '); }

    const char* ftype = "dir";
    switch (node->type) {
        case vfs::vfs_type::file: ftype = "file"; break;
        case vfs::vfs_type::directory: ftype = "dir"; break;
        case vfs::vfs_type::device: ftype = "device"; break;
        default: break;
    }

    printf("%s (%s, sz: %d)\n", node->name(), ftype, node->size);

    for (auto&& i : node->children) { debug(i, depth + 1); }
}

}  // namespace vfs
}  // namespace kernel
