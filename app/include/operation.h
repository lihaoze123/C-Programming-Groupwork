#ifndef OPERATION_H
#define OPERATION_H

#include <order.h>
#include <config.h>

void init_operation(Order_array* orders, Config* cfg);
void save();
void print();
void exit_system();
void add();
void delete();
void modify();
void query();
void sort();
void statistics();
void export_csv();
void import_csv();
void help();

#endif // OPERATION_H
