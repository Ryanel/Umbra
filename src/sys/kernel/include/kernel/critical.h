#pragma once

void k_critical_enter();
void k_critical_exit();

struct critical_section {
    critical_section() { k_critical_enter(); }
    ~critical_section() { k_critical_exit(); }
};
