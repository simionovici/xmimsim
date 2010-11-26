#ifndef XMI_AUX_X
#define XMI_AUX_X

#include <stddef.h>

//returns NULL on error
void *xmi_memdup(const void *mem, size_t bytes);

//returns an array with at least one element. If an error occurred, this element will have a value of -1
//works similar to the IDL function carrying the same name... the array is returned contains indices!!
int *xmi_sort_idl_int(int *array,int n_elements);


#endif