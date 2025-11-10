There are a lot of things I wanted to explore but I did not have the time.

I tried to have a more compact code since the goal is for demonstrationg and not for advanced functionalities.

I like how the structure is layed out in main, it allow for a tree visualization of all nodes, what I don't like is the internals of the data structure:

I wanted all modules, functions and blocks to be part of the Compiler data structure as shared_ptr and each module, function and block would "link" to the inner nodes via weak_ptr. This meant that I could check if the "target" node was still alive before accessing it. Since we are in C++ and have access to constructors, desctructors, getters and setters I wanted to play more with having the data structuring keeping itself validated without external calls.

The code outputed for graphviz is also not the best: the formating is something to improve as well as the node naming and ordering.

The current design suports labels per function, but since I couldn't get graphviz to output labels per function, the blocks names are more complex than necessary.
