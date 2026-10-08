#ifndef TYPES_H
#define TYPES_H

typedef enum{
    e_failure,
    e_success
}Status;

typedef enum
{
    e_view,
    e_edit,
    e_help,
    e_unsupported
} OperationType;

#endif