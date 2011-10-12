# Caribou's Object Model

Very similar to that of Acute, Caribou employs a slightly different model.

## Traits

Traits are the foundation of our compositional model. Traits are just other objects. We are not going to use the strict definition of traits whereby we can only store behaviour, we will store behaviour and state, similar to how Acute objects do.

If you try and use two traits which provide the same method signature, an exception will be thrown. You must manually resolve this conflict in order to use either functionality.

We use traits because they are compositional, and we won't be employing mechanisms like inheritance to build up objects. If you need inheritance, our slot table can be used to store a parent, or a list of prototypes, and our object model powerful enough to allow you to override lookup semantics so you can find the slot wherever it may happen to live.

## Objects

Objects are simple structures, containing the following properties at a minimum:

1. A queue to receive messages (FIFO)
2. An array of traits
3. A slot table

## Messages

Messages are the fundamental object by which communication happens. It's impossible to call a method on an object without using a message. (Well, technically not, but it damned well isn't easy.)

They contain the following properties, and should not contain more:

1. A name
2. A list of messages as arguments

## Lookup

The VM will implement a basic lookup mechanism. This basic algorithm might look like this:

1. Pop an item off the message queue
2. Look for "lookup", if we didn't find it...
3. Scan the list of traits, looking for a matching name in a slot table.
4. If no slot is found, throw an exception.
5. If it found the original slot, pass it to perform.

If the object contains a `lookup` slot however in its slot table or any of its traits slot tables, then we will call that to find the slot which is passed to the evaluator. In this case, the algorithm is slightly modified:

1. Pop an item off the message queue
2. Look for "lookup", if we find it...
3. Invoke that method passing in our symbol name identifying the slot we want to look up
4. Pass the result to the perform method.

Keep in mind, that between step 3 and 4, user-defined behaviour occurs.

## Evaluator (perform)

The VM will implement a basic evaluator. This basic algorithm might look like this until the `Echoing` release:

1. Receive a reference to an object, and an environment
2. Check if an object should activate.
3. If it should activate, lookup its `activate` message, passing along our environment and any arguments our message had.
4. Return the result of the activation, or the object itself if it wasn't activatable.

After the Echoing release, perform will be greatly simplified, looking more like this:

1. Receive an environment
2. Check for a compiled method
3. If a compiled method isn't found, compile the method as we're evaluating it
4. If a compiled method is found, invoke it passing along the environment
5. Return the result.
