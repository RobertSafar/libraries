# Disclaimer
This is a JSON parser implementation in C made for learning and personal use purposes. It may contain bugs, and may have some limitations.

## Documentation

### 2 Different Parsers
There are 2 JSON parsers implemented, Cson and rsJSON. son is the first JSON parser I impplemented. It is a recursive parser with the dependency on my string and vector implementations (rsString.h and rsVector.h). I wrote rsJSON as the replacement for Cson. rsJSON is itterative, with no dependencies on any of my other libraries. 
Using both parser is more or less the same.

### Loading Data

To load the data, you call the appropriate loadData function. In Cson this is the Cson_loadData, and it takes a string represented by RSStr struct as the argument, in rsJSON this is RSon_loadDataFromString and despite what the name might suggest, it takes a simple char pointer as the argument (not a RSStr struct). 

Both implementations provide 5 functions for printing the data. 

#### Important
As of now, rsJSON only stores strings (keys) as refferences to the char pointer passed to the RSon_loadDataFromString function. The data of this pointer must not change or the AST representing the JSON becomes invalid. I plan on continuing the work on rsJSON but for now, because I didn't want the dependency in dynamic String implementation, and I because this implementation fulfilled my needs so far, I haven't gotten around to implementing functionality that would copy the strings somewhere.

### Data Retrieval

- #### Cson
To retrieve data from Cson, you only have the Cson_get function, this function takes the root and a char pointer as the arguments. The char pointer must contain the path to the desired node. I tried making it simple but in the end I just made it very limited. When accessing an array element, you use a C array style indexing, and when accessing a key inside an object, you jsut type the key. Each index/key is separated by a full stop.

Exaples of usage:
- Cson_get(*root, "[0]") -> get the first array element
- Cson_get(*root, "name") -> get the value associated with the key "name"
- Cson_get(*root, "employees.[0].name") -> get the value associated with the key "employees", get the first array element, get the value associated with the key "name"

the downside of this apporach is having keys containing a full stop. I haven't implemented any escape logic, and so when there is a key that contains a full stop, for example "www.google", there is no way to search for this. Trying Cson_get(*root, "www.google) will try to look for the key "www", and if it is found it will then look for the key "google".

- #### rsJSON

To retrieve data from rsJSON you have 2 options, RSon_getPath which works the same way as Cson_get, and RSon_get. RSon_get works differently than Cson_get, as it uses va_args. To use RSon_get, you first pass the pointer to the root of the AST, and then, using the 3 Macros RSON_INDEX(), RSON_KEY() nad RSON_END, you build the path. 

Examples of usage:
- RSon_get(*root, RSON_INDEX(0), RSON_END) -> get the first array element
- RSon_get(*root, RSON_KEY("name"), RSON_END) -> get the value associated with the key "name"
- RSon_get(*root, 
RSON_KEY("employees"), RSON_INDEX(0), RSON_KEY("name"), RSON_END) -> get the value associated with the key "employees", get the first array element, get the value associated with the key "name"

This implementation of the get function, although more verbouse, doesn't have the same downside of keys including delimeters as the previous get implementation. Now we can easily search for any key, even keys like "www.google".

### Destruction

Both implementations require the AST to be freed using the Cson_free or the RSon_free functions.