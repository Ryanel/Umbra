#pragma once

#include <stddef.h>
#include <string.h>

namespace kernel {

class string {
   public:
    string(char* str) {
        m_size   = strlen(str);
        m_buffer = new char[m_size + 1];
        strncpy(m_buffer, str, m_size + 1);
    }

    // Copy constructor
    string(const string& s) {
        m_size   = s.m_size;
        m_buffer = new char[m_size + 1];  // + 1 for the keeping the null character
        strncpy(m_buffer, s.m_buffer, m_size + 1);
    }

    // Move constructor
    string(string&& s) {
        clean();
        m_size     = s.m_size;
        m_buffer   = s.m_buffer;
        s.m_buffer = nullptr;
    }

    // Move assignment
    string& operator=(string&& s) {
        clean();
        m_size     = s.m_size;
        m_buffer   = s.m_buffer;
        s.m_buffer = nullptr;
        return *this;
    }

    // Copy assignment
    string& operator=(const string& s) {
        clean();
        m_size   = s.m_size;
        m_buffer = new char[s.m_size + 1];
        strncpy(m_buffer, s.m_buffer, s.size() + 1);
        return *this;
    }

    char& operator[](unsigned i) { return m_buffer[i]; }

    ~string() { clean(); }

    // Utility functions
    string substr(size_t start, size_t end = npos) const {
        if (end == npos) { end = size(); }

        string sub(m_buffer + start);
        sub[end - start] = 0;
        return sub;
    }
    size_t find(char c) const {
        for (size_t i = 0; i < size(); i++) {
            if (m_buffer[i] == c) { return i; }
        }
        return npos;
    }

    // Data functions
    size_t      size() const { return strlen(m_buffer); }
    char const* data() { return m_buffer; }

    // Constants
    static const size_t npos = -1;

   private:
    char*  m_buffer;
    size_t m_size;

    void clean() {
        if (m_buffer = nullptr) { delete[] m_buffer; }
        m_size = 0;
    }
};

}  // namespace kernel