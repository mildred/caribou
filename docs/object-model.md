# Caribou's Object Model

Very similar to that of Acute, Caribou employs a slightly different model.

## Traits

Traits are the foundation of our compositional model. Traits are just other objects. We are not going to use the strict definition of traits whereby we can only store behaviour, we will store behaviour and state, similar to how Acute objects do.

If you try and use two traits which provide the same method signature, an exception will be thrown. These must be overridden before you use the second trait.

We use traits because they are compositional, and we won't be employing mechanisms like inheritance to build up objects.

## Objects

Objects are simple structures, containing the following properties at a minimum:

1. A queue to receive messages (FIFO)
2. An array of traits

## Messages

Messages are the fundamental object by which communication happens. It's impossible to call a method on an object without using a message. (Well, technically not, but it damned well isn't easy.)

They contain the following properties, and should not contain more:

1. A name
2. A list of messages as arguments

## Lookup

The VM will implement a basic lookup mechanism. This basic algorithm might look like this:

1. Pop an item off the message queue
2. Scan the list of traits, looking for a matching name in a slot table.
3. If no slot is found, throw an exception.
4. If it found the original slot, pass it to perform.

