# Disclaimer
This is a Vector/list implementation in C made for learning and personal use purposes. It may contain bugs, and may lack some functions I haven't thought about implementing yet.

## Documentation

This vector implementation offers 2 different structures to represent dynamic arrays, RSVec and RSManagedVec. RSVec only contains the size (number of elements), capacity (total capacity), element size(size of each element) and data (the actual dynamic memory). It is capable of storing and managing any data type that doesn't have a dynamic memory of it's own. If the elements point to dynaic memory, then destroying the RSVec without freeing the memory the elements point to first will result in memory leaks. RSManagedVec is the same structure as RSVec but has 2 additional attributs -> destroy (function pointer to the destroy function) and copy (function pointer to the copy function). When saving elements into the RSManagedVec or destroying the RSManagedVec, these function pointers will be used to correctly copy or free the elements. There is 1 copy and one destroy function implemented by default. These make the RSManagedVec behave as a 2D array (each element is treated as being of type RSVec). Setting these function pointers to NULL will make it bahave as RSVec. Other destoy and copy functions have to be implemented depending on your needs.


