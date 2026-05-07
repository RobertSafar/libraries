# Disclaimer
This is a Vector/list implementation in C made for learning and personal use purposes. It may contain bugs, and may lack some functions I haven't thought about implementing yet.

## Documentation

This vector implementation offers 2 different structures to represent dynamic arrays, Vector and CVector. Vector only contains the size (number of elements), capacity (total capacity), element size(size of each element) and data (the actual dynamic memory). It is capable of storing and managing any data type that doesn't have a dynamic memory of it's own. If the elements point to dynaic memory, then destroying the Vector without freeing the memory the elements point to first will result in memory leaks. CVector is the same structure as Vector but has 2 additional attributs -> destroy (function pointer to the destroy function) and copy (function pointer to the copy function). When saving elements into the CVector or destroying the CVector, these function pointers will be used to correctly copy or free the elements. There is 1 copy and one destroy function implemented by default. These make the CVector behave as a 2D array (each element is treated as being of type Vector). Setting these function pointers to NULL will make it bahave as Vector. Other destoy and copy functions have to be implemented depending on your needs.


