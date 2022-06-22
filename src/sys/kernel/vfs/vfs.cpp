#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/vfs/filesystem.h>
#include <kernel/vfs/node.h>
#include <kernel/vfs/vfs.h>
#include <kernel/vfs/delegate.h>
#include <stdio.h>

#include <algorithm>
#include <string_view>

using namespace kernel;
using namespace kernel::tasks;

namespace kernel {
namespace vfs {

virtual_filesystem g_vfs;

void virtual_filesystem::init() {}

bool virtual_filesystem::mount(std::string_view path, filesystem* fs) {
    for (auto&& i : m_mountpoints) {
        if (path.compare(i.m_path.data()) == 0) { return false; }
    }

    mountpoint mp;
    mp.m_fs   = fs;
    mp.m_path = std::string(path);
    m_mountpoints.push_back(mp);
    return true;
}

node* virtual_filesystem::find(std::string_view path) {
    filesystem* current_fs    = nullptr;
    node*       current_node  = nullptr;
    node_type   expected_type = node_type::file;
    size_t      path_ptr      = 0;

    // Strip trailing slashes
    if (path.rfind('/') == path.size() - 1 && path.size() != 1) {
        path          = path.substr(0, path.size() - 1);
        expected_type = node_type::directory;
    }

    kernel::log::trace("vfs", "Finding node: %s\n", std::string(path).c_str());

    while (path_ptr != path.size()) {
    next_itr:
        // First, obtain the names of what we're searching for
        size_t           next_delim = path.find('/', path_ptr);
        std::string_view current_name, search_path;
        const bool       root_dir = next_delim == 0;
        const bool       dir      = root_dir || next_delim != path.npos;

        if (dir) {
            // Directories
            next_delim   = root_dir ? 1 : next_delim;  // Auto-advance the delimiter for the root directory only
            current_name = path.substr(path_ptr, (next_delim - path_ptr));
            search_path  = path.substr(0, next_delim);
            path_ptr     = root_dir ? 1 : (next_delim + 1);
        } else {
            // File
            current_name = path.substr(path_ptr, path.size());
            search_path  = path;
            path_ptr     = path.size();
        }

        kernel::log::trace("vfs", "Looking for: %s (path is %s)\n", std::string(current_name).data(),
                           std::string(search_path).data());

        // Is there a mountpoint on this path? If so, swap to that.
        auto* mp = get_mountpoint(search_path);
        if (mp != nullptr) {
            current_fs   = mp->m_fs;
            current_node = mp->m_fs->get_root();
            kernel::log::trace("vfs", "Swapping to filesystem %s\n", current_fs->name());

            if (path.size() == path_ptr) { return current_node; }
            continue;
        }

        // No mountpoint, that's fine. Check the current node to see if there are any files.
        if (current_node != nullptr) {
            auto children = current_fs->get_children(current_node);

            for (auto&& i : children) {
                if (current_name.compare((const char*)&i->m_name) != 0) { continue; }
                current_node = i;
                if (next_delim != path.npos) { goto next_itr; }

                // We've reached the target
                if (expected_type == node_type::directory) {
                    return current_node->m_type == node_type::directory ? current_node : nullptr;
                }
                return current_node;
            }
        }

        kernel::log::warn("vfs", "Unable to find: %s (path is %s)\n", std::string(current_name).data(),
                          std::string(search_path).data());
        break;
    }

    return nullptr;
}

virtual_filesystem::mountpoint* virtual_filesystem::get_mountpoint(std::string_view path) {
    for (auto&& i : m_mountpoints) {
        if (path.compare(i.m_path.data()) == 0) { return &i; }
    }
    return nullptr;
}

handle* virtual_filesystem::open_file(std::string_view path, int flags) {
    node* n            = find(path);
    task* current_task = scheduler::get_current_task();
    if (n) {
        // Create the kernel file descriptor handle
        auto * khnd = this->m_open_files.create(make_ref(new file_descriptor(n, current_task->m_next_unix_fileid++)), 0, 0xFFFFFFFF, 0);
        // Now, duplicate it and transfer to the task.
        return this->m_open_files.duplicate_and_transfer(khnd, &current_task->m_local_handles, current_task->m_task_id);
    } else {
        if ((flags & VFS_OPEN_FLAG_CREATE != 0)) { assert(false && "flags & VFS_OPEN_FLAG_CREATE: Implement this"); }
    }

    return nullptr;
}
size_t virtual_filesystem::read(handle *hnd, uint8_t* buf, size_t count) {
    if (hnd == nullptr || !hnd->is<file_descriptor>()) { return 0; }
    auto fd = hnd->as<file_descriptor>();

    kernel::log::debug("vfs", "Reading %d bytes of %s into buf 0x%016p\n", count, fd->m_node->name(), buf);

    if (fd->m_node->m_delegate == nullptr) {
        kernel::log::error("vfs", "Error reading file %s, delegate is null\n", fd->m_node->name());
        return 0;
    }

    return fd->m_node->m_delegate->read(fd->m_node, buf, 0, count);
}
size_t virtual_filesystem::write(handle *hnd, uint8_t* buf, size_t count) { return 0; }

}  // namespace vfs
}  // namespace kernel