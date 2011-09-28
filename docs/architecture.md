# Stack Machine Architecture

Caribou is a stack-based virtual machine architecture. It is based around a multiple stack architecture. We use a single stack for objects that live in the system, and a return stack to store the instruction pointer.

This virtual machine is designed to be used as a backend for programming languages who operate on a message sending pattern. At a high level, it also features:

* Zero operand stack architecture
* Objects (as actors and vessels for concurrency)
* Traits for behaviour
* State contracts for traits requiring state on objects
* Cheap object creation (and safe, more expensive object creation)
* Message sends are akin to function calls in other VMs

## Zero operands

Most stack architectures are either two or three operand machines. This involves an operand decoding state as well as a fetch and execute. This adds extra complexity.

## Objects

Objects are containers for state, and vessels for concurrency. They allow behaviour to be composed in via a set of traits, and for any state required to be defined on the object. All behaviour is decoupled from the object itself. Objects when created have two traits automatically installed: Primitives trait (things like setting a slot, slot lookup, evaluator, etc), and a 'local' trait. This is where any methods that you try and define on the object itself will live. It is consequently looked up first.

Lookup is performed like this:

1. Check the local trait for any matching method
2. Check the other traits (in no defined order) for matching method
3. Check primitive trait
4. Check state table.
5. Raise an error about no slot found

## Traits

Traits are the containers for behaviour, decoupled from objects. You define related behaviours inside a trait, and include those traits in objects.

## State contracts

Some pieces of behaviour may depend on some state. For instance, an implementation of an linked list may depend on an enumerable trait — providing behaviour to iterate the list. However, traits don't provide state, but it depends on state. As such we need a contract that any users of the trait, implement the state required. This is accomplished through the state contract. Should a user not implement the state, we throw an exception to the user.

The idea of a state contract does not mandate any particular implementation strategy, though we strongly advise against the use of hard names; implementors should use mappings instead, allowing the user to define their own names.

## Messages

Messages are lightweight. They contain a name, a list of arguments, and a cached result. They are the primary mechanism for communication in the virtual machine. Everything, lookups for state or behaviour, are all done through messages. Consider them like functions in a traiditonal vm.

## Garbage Collection

Garbage collection is split up into multiple generations. One goal is to have fast object allocation, similar to the JVM; meaning, we want to be able to allocate space for an object in a few cycles.
