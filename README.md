# c-vector
----------
Dynamic size arrays made easy in C.
Who needs fancy generics like C++ for `std::vector` when you have the good old preprocessor macros.

## But, why?
Beginers usually prefer C++ over C because they see C++ easier and C++ has a lot of things built in. That is not true. Being fancier is not being easier. Things like classes, namespaces, inheritance, polymorphism, constructors, virtual, friend, public and private methods, `this`, `new`, `delete` and many others are just more things to remember that make the codebase complicated and messed up. Why do you think compiling C++ is slower?
More experienced programmers who can see the truth, spend a long time implementing utilities that they need for their program.
This project provides a general purpose implementation of the vector data structure which can be used almost everywhere.

## Configure with `#define`
This library relies heavily on macros to replicate what templates and generics in C++ do. Although you can modify the output of the macros using some other macros as configurations. Due to not being able to nest `#ifdef` inside `#define`, other macros will be defined according to the `#define`d configuration macros.
## Silent mode
Activated if you `#define CVECTOR_SILENT`.
Specific functions will by default return a status integer to indicate success of the function. `0` for ok and `1` if it didn't work. This is defined by the macros `CVECTOR_STATUS` and `CVECTOR_RETURN(expr)`, which by default will translate to `int` and `{ return expr; }`. This is called verbose mode. If `CVECTOR_SILENT` is defined, they will be translated to `void` and `{ expr; return; }`. This is called silent mode. It evaluates `expr` because sometimes it will return the status from another function call, which will also be silent, but the call needs to be done.

## Struct mode
This mode is the default. In this mode, a struct is used to store the data. This mode can be more flexible and the struct allows a more general purpose usage. It is also good if you want to print the items of the vector.
All the functions take the pointer of the `cvector_t` struct. It's a pointer to prevent memory copying.

### Functions and structs

#### cvector\_t: struct
A struct representing the vector and holding it's metadata.
 - `int item_size`: The size of the array's item. Used for indexing. **DO NOT** modify.
 - `int cap`: The capacity of the array. **DO NOT** modify.
 - `int len`: The number of items the array has. **DO NOT** modify.
 - `int size`: The allocated size. **DO NOT** modify. Aligment defined by the [ALIGNMENT](#ALIGNMENT) macro.
 - `void *data`: The pointer to the allocated memory. **DO NOT** modify.
 - `void (print_item *)(void *)`: The function that prints the item. Receives the pointer of the item. **DO NOT** print a newline at the end.

#### cvector\_init
Initializes the vector.
  - Arguments:
    - `cvector_t *vec`: -
    - `int item_size`: The size of each item.
    - `int cap`: If you know how big the array might get, to prevent continous reallocations. If `0`, no memory is allocated.
  - Return
    - Verbose mode(`int`): `0` if allocation succeded, otherwise `1`.
    - Silent mode(`void`)

#### cvector\_at
Gets the item at the specified index with bounds check.
  - Arguments:
    - `cvector_t *vec`: -
    - `int idx`: The index of the item.
  - Return(`void *`): The pointer to the item, `NULL` if the index is out of bounds.

#### cvector\_get
Gets the item at the specified index without bounds check.
**WARNING**: Use this only when you are sure the index isn't out of bounds but don't want the overhead of the check.
  - Arguments:
    - `cvector_t *vec`: -
    - `int idx`: The index of the item.
  - Returns(`void *`): The calculated pointer to the item.

#### cvector\_expand
`realloc`ates more memory for more items.
  - Arguments:
    - `cvector_t *vec`: -
  - Return(`int`): `0` if the reallocation succeded, othewise `1`.

#### cvector\_shrink
Shrinks the allocated memory by half.
  - Arguments:
    - `cvector_t *vec`: -
  - Return(`int`): `0` if the reallocation succeded, othewise `1`.

#### cvector\_expand\_if\_needed
Expands the vector if `len >= cap`.
  - Arguments:
    - `cvector_t *vec`: -
  - Return: The same as [cvector\_expand](#cvector_expand)

#### cvector\_shrink\_if\_needed
Calls [cvector\_shrink](#cvector_shrink) if `len < cap / 2`
  - Arguments:
    - `cvector_t *vec`: -
  - Return: The same as [cvector\_shrink](#cvector_shrink)

#### cvector\_cleanup
  - Arguments:
    - `cvector_t *vec`: -
`free`s the allocated memory and `0`s all the struct's fields.

### Macros
The macros [CVECTOR\_DECLARE\_WITH\_NAME](#CVECTOR_DECLARE_WITH_NAME) and [CVECTOR\_WITH\_NAME](#CVECTOR_WITH_NAME) could have been a single macro, but the first one is made to be used in a header file and the second to be used in a source file. The reason is that if you have multiple source files and want them to have the same definition, you will end up having link time conflicts. That's why it's better to use the first one on a header file and the second on a source file. If you are going to use it only in one place, use both of them one after the other. In the future I can make all of them inline, so that you only need to do that once.
#### ALIGNMENT
The alignment of the size that will be `malloc`'d. Defaults to `sizeof(size_t)`.
#### CVECTOR\_DECLARE\_WITH\_NAME
##### Arguments
  - `type`: The type of the items.
  - `name`: An identifier that will be used in the start of the emited functions' names.

##### Output
  - `typedef cvector_t` the identifier you provide as argument `name`.
  - `CVECTOR_STATUS name##_init(name *vec)`:
      Initialise empty vector.
  - `CVECTOR_STATUS name##_init_with_capacity(name *vec, int cap)`:
      Initialise vector with initial capacity. Useful when you pretty much know how many items there are going to be, and you don't want performance drops because `malloc` and `realloc`.
  - `CVECTOR_STATUS name##_init_with_length(name *vec, int len)`:
      Initialise vector with a specified number of items, which are zeroed. Good when you are iterating on something and you don't want to push the items, but you want to assign them instead.
  - `CVECTOR_STATUS name##_init_with_fill(name *vec, int len, type val)`:
      Initialise vector with specified number of of items, all having the value you pass as the 3rd argument.
  - `CVECTOR_STATUS name##_push(name *vec, type val)`:
      Push item at the end of the vector. If in verbose mode and pushing the value fails, it will return 1, otherwise 0.
  - `type name##_pop(name *vec)`:
      The function prototype.
      Removes and return the last item of the vector. If the vector is empty, an empty value is returned.
  - `type *name##_at(name *vec, int idx)`:
      Works like [cvector\_at](#cvector_at).
  - `type *name##_get(name *vec, int idx)`:
      Works like [cvector\_get](#cvector_get).
  - `void name##_set(name *vec, int idx, type value)`
      Gives the item at the specified index the value you provide as the 3rd argument. Does not do bound checking, so be careful not to corrupt memory.
  - `CVECTOR_STATUS name##_set_at(name *vec, int idx, type value)`:
      The function prototype.
      Works like `name##_set`, but in verbose mode, returns o1 if out of bounds and 0 if ok.
  - `name name##_clone(name *vec)`:
      Creates and returns a new identical vector, but with the items copied on another place.
  - `name name##_deep_clone(name *vec, type (*cloner)(type *))`:
      The function prototype.
      This is more like a map function. It creates a new vector, and uses the function to clone each of the items, which are pushed to the new vector.
  - `void name##_print(name *vec)`:
      The function prototype.
      Nicely prints the vector data and it's values. The `print_item` field of the [cvector\_t](#cvector_t) struct needs to be set so it can print the items.
  - `void name##_cleanup`:
      Calls [cvector\_cleanup](#cvector_cleanup).

**NOTE**: The `name` argument will be pasted in front of the functions listed above. i.e. If you pas the identifier `Foo` as the `name` argument, the function names will be like below:
  `Foo_init`,
  `Foo_push`, etc...

#### CVECTOR\_WITH\_NAME
  - Arguments: Same as [CVECTOR\_DECLARE\_WITH\_NAME](#CVECTOR_DECLARE_WITH_NAME).
##### Output
The bodies of the function prototypes generated from [CVECTOR\_DECLARE\_WITH\_NAME](#CVECTOR_DECLARE_WITH_NAME).

### Usage
Using the macros and the functions they generate.
```c
#include <stdio.h>
#include <c-vector/lib.h>

CVECTOR_DECLARE_WITH_NAME(int, IntVector);
CVECTOR_WITH_NAME(int, IntVector);

void print_int(int *ptr) { printf("%d", *ptr); }

int main(void) {
  IntVector vec;
  vec.print_item = (void (*)(void *))print_int;
  // Initializes it as empty
  IntVector_init(&vec);
  // Initializes it with allocated capacity of 6 items
  IntVector_init_with_capacity(&vec, 6);
  // Intitalize it with the capacity and length of 9 items, all being 0
  IntVector_init_with_length(&vec, 9);
  // Initializes it with length 4 and each item has the value 853
  IntVector_init_with_fill(&vec, 4, 853);

  // Push items at the end.
  // On verbose mode return 1 if can't reallocate memory when trying to extend, otherwise 0.
  IntVector_push(&vec, 83);
  // Pop and return an item; the item is 0'd if can't pop anymore.
  IntVector_pop(&vec);

  // Set the value of an item at a specific index.
  // On verbose mode return 1 if out of bounds, otherwise 0.
  IntVector_set_at(&vec, 2, 24);
  // Get the pointer to an item; no bounds check.
  IntVector_at(&vec, 8);
  // Get the pointer to an item; NULL if out of bounds.
  IntVector_at(&vec, 2);

  // Print the vector; if field print_item is not set, it will not try to print the items
  IntVector_print(&vec);
  /*
   * Will print as below:
   * [IntVector][cvector_t(int)] {
   *   .len: 4
   *   .cap: 8
   *   .data: <some pointer>
   *   .items: [853, 853, 24, 853]
   * }
   */

   // You can clone the vector. They will have the same data, but modifying any of them won't affect the other, because they're separate. 
   // You can create as much clones as your memoru fits.
   IntVector vec_clone = InVector_clone(&vec);
   // Be sure to clean up all the vectors after you finish working with them, or you'll have memory leaks.
   IntVector_cleanup(&vec);
   IntVector_cleanup(&vec_clone);

  return 0;
}
```
## Fat-pointer mode
This mode is activated if you `#define CVECTOR_FATPOINTER`.
In this mode a pointer is used instead of a struct, which points at the data. The metadata, like the length and the capacity are stored right as a header right before the address where the pointer points to. This mode is more suited for efficiency and speed, as oposed to [struct mode](#Struct-mode) which is more suited to practical and general purpose use. Also you can use the `[]` syntax to access the items of the vector.

### Functions and structs

#### cvector\_header: struct
The struct that holds the metadata of the vector, embeded before the vector's data.
  - `int cap`: The capacity of the vector.
  - `int len`: The number of items the vector has.

#### cvector\_get\_header
  - Arguments:
    - `void *vec`: The vector.
  - Return(`cvector_header *`): The pointer to the vector's metadata.

#### cvector\_expand
  - Arguments:
    - `void *vec`: The vector.
  - Return: The same as [cvector\_expand of struct-mode](#cvector_expand).

#### cvector\_shrink
  - Arguments:
    - `void *vec`: The vector.
  - Return: The same as [cvector\_shrink of struct-mode](#cvector_shrink).

#### cvector\_len
  - Arguments:
    - `void *vec`: The vector.
  - Return(`int`): The length of the vector.

#### cvector\_cap
  - Arguments:
    - `void *vec`: The vector.
  - Return(`int`): The capacity of the vector.

### Macros

#### CVECTOR\_WITH\_NAME
##### Arguments
  - `type`: The type of the items.
  - `name`: The name of the vector type which is also used to identify which functions are for which type of vector, just like in [struct-mode](#Struct-mode).

##### Output
  - `typedef type* name;` if `CVECTOR_NO_TYPEDEF` is not defined, otherwise nothing.
  - `name name##_new()`:
      Allocates a vector with 0 items and capacity (empty). Will return `NULL` if the allocation fails.
  - `name name##_with_capacity(int cap)`:
      Allocates and initializes a vector with an initial capacity.
  - `name name##_with_length(int len)`:
      Allocates and initializes a vector with initial capacity and length. All the items are `0`'d.
  - `name name##_with_fill(int len, type val)`:
      Like `name##_with_length`, except you specify the initial value of all the items instead of them being `0`'d.
  - `int name##_len(name vec)`:
      Calls and returns the return of [cvector\_len](#cvector_len).
  - `int name##_cap(name vec)`:
      Calls and returns tbe return of [cvector\_cap](#cvector_cap).
  - `int name##_expand_if_needed(name *vec)`:
      Works like [cvector\_expand\_if\_needed from struct-mode](#cvector_expand_if_needed). It is a double pointer (remember,  `name = type *`, so `name * = type **`) because it points to the variable that holds the address of the vector's data because it might reallocate and if it doesn't modify the address variable, you would end up with an invalid pointer. You don't need to worry since you don't need or have to use it.
  - `int name##_shrink_if_needed(name *vec)`:
      Works likr [cvector\_shrink\_if\_needed from struct-mode](#cvector_shrink_if_needed), but will shrink if if the length is less than 1/3 of the capacity.
  - `type *name##_at(name vec, int idx)`:
      Works like `name##_at` from struct-mode.
  - `CVECTOR_STATUS name##_set(name vec, int id, type val)`:
      Works like `name##_set_at` from struct-mode.
  - `CVECTOR_STATUS name##_push(name *vec, type val)`:
      Works like `name##_push` from struct-mode. It takes a double pointer(rember that `name = type *`, so `name * = type **`) because it might reallocate the vector. In that case it will set the variable to point to the new address.
  - `type name##_pop(name *vec)`:
      Works like `name##_pop` from struct-mode, but just like `name##_push`, it might reallocate(the vector can be shrinked) so it needs the pointer to the vector variable.
  - `void name##_cleanup(name vec)`:
      Will free the vector.
