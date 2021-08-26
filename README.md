# c-vector
Dynamic size arrays made easy in C.
Who needs fancy generics like C++ for `std::vector` when you have the good old preprocessor macros.

## But, why?
Beginers usually prefer C++ over C because they see C++ easier and C++ has a lot of things built in. That is not true. Being fancier is not being easier. Things like classes, namespaces, inheritance, polymorphism, constructors, virtual, friend, public and private methods, `this`, `new`, `delete` and many others are just more complications that make the codebase complicated and messed up. Why do you think compiling C++ is slower?
More experienced programmers who can see the truth, spend a long time implementing utilities that they need for their program.
This project provides a general purpose implementation of the vector data structure which can be used almost everywhere.

## `struct`s and functions
All the functions take the pointer of the `cvector_t` struct. It's a pointer to prevent memory copying.
### cvector\_t: struct
The struct containing the metadata.
 - `int item_size`: The size of the array's item. Used for indexing. **DO NOT** modify.
 - `int cap`: The capacity of the array. **DO NOT** modify.
 - `int len`: The number of items the array has. **DO NOT** modify.
 - `int size`: The allocated size. Aligment defined by the [ALIGNMENT](#ALIGNMENT) macro. **DO NOT** modify.
 - `void *data`: The pointer to the allocated memory. **DO NOT** modify.
 - `void (print_item *)(void *)`: The function that prints the item. Receives the pointer of the item. **DO NOT** print a newline at the end.

### cvector\_init
Initializes the vector.
  - Arguments:
    - `cvector_t *vec`: -
    - `int item_size`: The size of each item.
    - `int cap`: If you know how big the array might get, to prevent continous reallocations. If `0`, no memory is allocated.
  - Return(`int`): `0` if allocation succeded, otherwise `1`.

### cvector\_get
Gets an item at the specified index.
  - Arguments:
    - `cvector_t *vec`: -
    - `int idx`: The index of the that will be returned
  - Return(`void *`): The pointer to the item, `NULL` if the index is out of bounds.

### cvector\_expand
`realloc`ates more memory for more items.
  - Arguments:
    - `cvector_t *vec`: -
  - Return(`int`): `0` if the reallocation succeded, othewise `1`.

### cvector\_shrink
Shrinks the allocated memory.
  - Arguments:
    - `cvector_t *vec`: -
  - Return(`int`): `0` if the reallocation succeded, othewise `1`.

### cvector\_expand\_if\_needed
Expands the vector if `len >= cap`.
  - Arguments:
    - `cvector_t *vec`: -
  - Return(`int`): The same as [cvector\_expand](#cvector_expand)

### cvector\_shrink\_if\_needed
Calls [cvector\_shrink](#cvector_shrink) if `len < cap / 2`
  - Arguments:
    - `cvector_t *vec`: -
  - Return(`int`): The same as [cvector\_shrink](#cvector_shrink)

### cvector\_cleanup
  - Arguments:
    - `cvector_t *vec`: -
`free`s the allocated memory and `0`s all the struct's fields.

## Macros
The macros [CVECTOR\_DECLARE\_WITH\_NAME](#CVECTOR_DECLARE_WITH_NAME) and [CVECTOR\_WITH\_NAME](#CVECTOR_WITH_NAME) could have been a single macro, but the first one is made to be used in a header file and the second to be used in a source file. The reason is that if you have multiple source files and want them to have the same definition, you will end up having link time conflicts. That's why it's better to use the first one on a header file and the second on a source file. If you are going to use it only in one place, use both of them one after the other.
### ALIGNMENT
The alignment of the size that will be `malloc`'d. Defaults to `sizeof(size_t)`.
### CVECTOR\_DECLARE\_WITH\_NAME
  - Arguments:
    - type: The type of the items.
    - name: An identifier that will be used in the start of the emited functions' names..
  - Output:
    - `typedef cvector_t` the identifier you provide as argument `name`.
    - `init`ializer functions according to the type and name.
    - `push` and `pop` functions' declarations according to the type and name.
    - `get` and `set` functions' declarations according to the type and name.
    - `print` function declaration.
    - inline `cleanup` function that calls [cvector\_cleanup](#cvector_cleanup).

**NOTE**: The `name` argument will be pasted in front of the functions listed above. i.e. If you pas the identifier `Foo` as the `name` argument, the function names will be like below:
  `Foo_init`,
  `Foo_push`, etc...

### CVECTOR\_WITH\_NAME
  - Arguments: Same as [CVECTOR\_DECLARE\_WITH\_NAME](#CVECTOR_DECLARE_WITH_NAME).
  - Output:
    - The bodies of non inline functions.

## Usage
Basic usage:
```c
#include <stdio.h>
#include <c-vector/cvector.h>

CVECTOR_DECLARE_WITH_NAME(int, IntVector);
CVECTOR_WITH_NAME(int, IntVector);

void print_int(int *ptr) { printf("%d", *ptr); }

int main(void) {
  IntVector vec;
  vec.print_item = ((void *)(void *))print_int;
  // Initializes it as empty
  IntVector_init(&vec);
  // Initializes it with allocated capacity of 6 items
  IntVector_init_with_capacity(&vec, 6);
  // Initializes it with length 4 and each item has the value 853
  IntVector_init_with_fill(&vec, 4, 853);

  // Push items at the end; return 1 if can't reallocate memory when trying to extend, otherwise 0
  IntVector_push(&vec, 83);
  // Pop and return an item; the item is 0'd if can't pop anymore.
  IntVector_pop(&vec);

  // Set the value of an item at a specific index; return 1 if out of bounds, otherwise 0
  IntVector_set(&vec, 2, 24);
  // Get the pointer to an item; NULL if out of bounds
  IntVector_get(&vec, 2);

  // Print the vector; if field print_item is not set, it will not try to print the items
  IntVector_print(&vec);
  /*
   * Will print as below:
   * [IntVector][cvector_t(int)] {
   *   .len: 4
   *   .cap: 8
   *   .data: 0x<some pointer>
   *   .items: [853, 853, 24, 853]
   * }
   */

  return 0;
}
```
