# c-vector
----------
Dynamic size arrays made easy in C.
Who needs fancy generics like C++ for `std::vector` when you have the good old preprocessor macros.

## Configure with `#define`
This library relies heavily on macros to replicate what templates and generics in C++ do. Although you can modify the output of the macros using some other macros as configurations. Due to not being able to nest `#ifdef` inside `#define`, other macros will be defined according to the `#define`d configuration macros. Remember not to nest configurations, as the inner one will erase all the others. Everytime you change you configuration macros, make sure to re`#include <c-vector/lib.h>`. Also be sure to reconfigure and reinclude after using other libraries that use c-vector.

## Silent mode
Activated if you `#define CVECTOR_SILENT`.
Specific functions will by default return a status integer to indicate success of the function. `0` for ok and `1` if it didn't work. This is defined by the macros `CVECTOR_STATUS` and `CVECTOR_RETURN(expr)`, which by default will translate to `int` and `{ return expr; }`. This is called verbose mode. If `CVECTOR_SILENT` is defined, they will be translated to `void` and `{ expr; return; }`. This is called silent mode. It evaluates `expr` because sometimes it will return the status from another function call, which will also be silent, but the call needs to be done.

## Struct mode
This mode is the default. In this mode, a struct is used to store the data. This mode ia faster to push and pop and the struct allows a more general purpose usage. It is also good if you want to print the items of the vector.
All the functions take the pointer of the `cvector_t` struct. It's a pointer to prevent memory copying.

### Functions and structs

#### cvector\_t: struct
A generic struct representing the vector data structure.
 - `int c`: The capacity of the vector.
 - `int l`: The number of items the vector has.
 - `void *d`: The pointer to the allocated memory.

#### cvector\_init
Initializes the vector.
  - Arguments:
    - `cvector_t *vec`: -
    - `int item_size`: The size of the item's type.
    - `int cap`: If you know how big the array might get, to prevent continous reallocations. If `0`, no memory is allocated.
  - Return
    - Verbose mode(`int`): `0` if allocation succeded, otherwise `1`.
    - Silent mode(`void`)

#### cvector\_at
Gets the item at the specified index with bounds check.
  - Arguments:
    - `cvector_t *vec`: -
    - `int idx`: The index of the item.
    - `int item_size`: The size of the item's type.
  - Return(`void *`): The pointer to the item, `NULL` if the index is out of bounds.

#### cvector\_get
Gets the item at the specified index without bounds check.
**WARNING**: Use this only when you are sure the index isn't out of bounds but don't want the overhead of the check.
  - Arguments:
    - `cvector_t *vec`: -
    - `int idx`: The index of the item.
    - `int item_size`: The size of the item's type.
  - Returns(`void *`): The calculated pointer to the item.

#### cvector\_expand
`realloc`ates more memory for more items.
  - Arguments:
    - `cvector_t *vec`: -
    - `int item_size`: The size of the item's type.
  - Return(`int`): `0` if the reallocation succeded, othewise `1`.

#### cvector\_shrink
Shrinks the allocated memory by half.
  - Arguments:
    - `cvector_t *vec`: -
    - `int item_size`: The size of the item's type.
  - Return(`int`): `0` if the reallocation succeded, othewise `1`.

#### cvector\_expand\_if\_needed
Expands the vector if `len >= cap`.
  - Arguments:
    - `cvector_t *vec`: -
    - `int item_size`: The size of the item's type.
  - Return: The same as [cvector\_expand](#cvector_expand)

#### cvector\_shrink\_if\_needed
Calls [cvector\_shrink](#cvector_shrink) if `len < cap / 2`
  - Arguments:
    - `cvector_t *vec`: -
    - `int item_size`: The size of the item's type.
  - Return: The same as [cvector\_shrink](#cvector_shrink)

#### cvector\_cleanup
  - Arguments:
    - `cvector_t *vec`: -
`free`s the allocated memory and `0`s all the struct's fields.

### Macros

#### CVECTOR\_ALIGNMENT
The alignment of the size that will be `malloc`'d. Defaults to `sizeof(size_t)`.
#### CVECTOR\_WITH\_NAME
##### Arguments
  - `type`: The type of the items.
  - `name`: An identifier that will be used in the start of the emited functions' names.

##### Output
  - A struct similiar to `cvector_t` but the `d` field has type `type *`.
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
  - `CVECTOR_STATUS name##_pushp(name *vec, type *val_p)`:
      Works like `name##_push` but you give a pointer to the value that is going to be pushed instead.
  - `int name##_pop(name *vec)`:
      Decrements the length and returns it if it is not 0, otherwise it returns -1.
  - `type name##_pop_get(name *vec)`:
      Removes and returns the last item of the vector. If the vector is empty, an 0-ed value is returned.
  - `int name##_pop_get_into(name *vec, type *val_p)`:
      Copies the last item to the address that the second arguments points to and ecrements the length and returns it if it is not 0, otherwise it returns -1.
  - `type name##_pop_shrink_get(name *vec)`:
      Works like `name##_pop_get` but shrinks the vector when the length gets smaller than half of the capacity.
  - `int name##_pop_shrink_get_into(name *vec, type *val_p)`:
      Works like `name##_pop_get_into` but shrinks the vector when the length gets smaller than half of the capacity.
  - `void name##_clear(vec *name)`:
      Sets the length to 0 and 0-s the previously occupied by items space.
  - `type *name##_at(name *vec, int idx)`:
      Works like [cvector\_at](#cvector_at).
  - `type *name##_get(name *vec, int idx)`:
      Works like [cvector\_get](#cvector_get).
  - `void name##_set(name *vec, int idx, type value)`
      Gives the item at the specified index the value you provide as the 3rd argument. Does not do bound checking, so be careful not to corrupt memory.
  - `CVECTOR_STATUS name##_set_at(name *vec, int idx, type value)`:
      Works like `name##_set`, but in verbose mode, returns o1 if out of bounds and 0 if ok.
  - `name name##_clone(name *vec)`:
      Creates and returns a new identical vector, but with the items copied on another place.
  - `name name##_deep_clone(name *vec, type (*cloner)(type const *, type *))`:
      This is more like a map function. It creates a new vector, and uses the function to clone each of the items, which are pushed to the new vector.
  - `void name##_print(name *vec, void (*print_item)(type *))`:
      Nicely prints the vector data and it's values.
  - `void name##_cleanup`:
      Calls [cvector\_cleanup](#cvector_cleanup).

**NOTE**: The `name` argument will be pasted in front of the functions listed above. i.e. If you pas the identifier `Foo` as the `name` argument, the function names will be like below:
  `Foo_init`,
  `Foo_push`, etc...

### Usage
Using the macros and the functions they generate.
```c
#include <stdio.h>
#include <c-vector/lib.h>

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
   // You can create as much clones as your memory fits.
   IntVector vec_clone = InVector_clone(&vec);
   // Be sure to clean up all the vectors after you finish working with them, otherwise you'll end up leaking memory.
   IntVector_cleanup(&vec);
   IntVector_cleanup(&vec_clone);

  return 0;
}
```
## Pointer mode
This mode is activated if you `#define CVECTOR_POINTERMODE`.
In this mode a pointer is used instead of a struct, which points at the data. The metadata, like the length and the capacity are stored right as a header right before the address where the pointer points to. This mode is more suited for practical use, like being able to use the square brackets to access items, as oposed to [struct mode](#Struct-mode).

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

#### cvectorp\_shrink
  - Arguments:
    - `void *vec`: The vector.
  - Return: The same as [cvector\_shrink of struct-mode](#cvector_shrink).

#### cvectorp\_len
  - Arguments:
    - `void *vec`: The vector.
  - Return(`int`): The length of the vector.

#### cvectorp\_cap
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
      Works like [cvector\_shrink\_if\_needed from struct-mode](#cvector_shrink_if_needed), but will shrink if if the length is less than 1/3 of the capacity.
  - `type *name##_at(name vec, int idx)`:
      Works like `name##_at` from struct-mode.
  - `CVECTOR_STATUS name##_set(name vec, int id, type val)`:
      Works like `name##_set_at` from struct-mode.
  - `CVECTOR_STATUS name##_push(name *vec, type val)`:
      Works like `name##_push` from struct-mode. It takes a double pointer(rember that `name = type *`, so `name * = type **`) because it might reallocate the vector. In that case it will set the variable to point to the new address.
  - `CVECTOR_STATUS name##_pushp(name *vec, type *val_p)`:
      Works like `name##_push` but you give a pointer to the value that is going to be pushed instead.
  - `int name##_pop(name vec)`:
      Decrements the length and returns it if it is not 0, otherwise it returns -1.
  - `type name##_pop_get(name vec)`:
      Removes and returns the last item of the vector. If the vector is empty, an 0-ed value is returned.
  - `int name##_pop_get_into(name vec, type *val_p)`:
      Copies the last item to the address that the second arguments points to and ecrements the length and returns it if it is not 0, otherwise it returns -1.
  - `type name##_pop_shrink_get(name *vec)`:
      Works like `name##_pop_get` but shrinks the vector when the length gets smaller than half of the capacity.
  - `int name##_pop_shrink_get_into(name *vec, type *val_p)`:
      Works like `name##_pop_get_into` but shrinks the vector when the length gets smaller than half of the capacity.
  - `void name##_clear(vec name)`:
      Sets the length to 0 and 0-s the previously occupied by items space.
  - `name##_clone(name vec)`:
      Works like `name##_clone` from struct-mode.
  - `name##_deep_clone(name vec, type (*cloner)(type const *, type *))`:
      Works like `name##_deep_clone` from struct-mode.
  - `void name##_cleanup(name vec)`:
      Will free the vector.

### Example
Enabling [pointer mode](#Pointer-mode) and defining a new vector type and it's functions.
```c
#define CVECTOR_POINTERMODE
#include <c-vector/lib.h>

CVECTOR_WITH_NAME(double, F64Vector);
```

#### Different ways of creating a new vector
Empty:
```c
  F64Vector vec = F64Vector_new();
```
With initial capacity:
```c
  F64Vector vec = F64Vector_with_capacity(5);
```
With initial length, all items are `0`'d:
```c
  F64Vector vec = F64Vector_with_length(8);
```
With initial length and item initial value:
```c
  F64Vector vec = F64_with_fill(3, 4.085793);
```

#### Operating on the vector
Pushing:
```c
  F64Vector_push(&vec, 3.65);
```
Popping:
```c
  printf("%f\n", F64Vector_pop_get(&vec));
```
Unchecked item access:
```c
  vec[2] = 4.546;
  printf("%f\n", vec[1]);
```
Checked item access:
```c
  // If you didn't define CVECTOR_SILENT, it will return int(0)
  assert(!F64Vector_set(vec, 2, 4.546));
  assert(F64Vector_at(vec, 1)); // Will return the pointer to that if the index is not out of bounds.
  printf("%f\n", *F64Vector_at(vec, 1));
```
Getting the length and the capacity:
```c
  printf("len: %d\n"
         "cap: %d\n", F64Vector_len(vec), F64Vector_cap(vec));
```
Clearing the vector's contents:
```c
F64Vector_clear(vec);
assert(F64Vector_len(vec) == 0);
```
And finally, freeing:
```c
  F64Vector_cleanup(vec);
```
