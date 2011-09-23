# Caribou Virtual Machine
Copyright © 2011, Jeremy Tregunna, All Rights Reserved.

## Overview

Caribou is a prototype based object-oriented language virtual machine built on a message sending architecture. It implements an object model and messaging systems heavily inspired from Io, and Self.

## What's different?

The architecture. While closer to Self than Io, the model for Caribou was developed independent of Self, and descended from Io.

Io implements a message sending architecture where messages are dispatched to an object, whereby it looks it up locally, then through its protos list in a first match wins. It delegates the message send to the first item in the protos, then to its protos first object and so on until it runs out; then it goes back up one level, tries another proto. Should that fail, it keeps going back up the tree until it exhausts all protos and raises an exception. Caribou is slightly different.

Objects are containers for concurrency. They hold state which is local to the object, a mailbox (a queue where messages are received). It contains also a list of traits; it does not employ any type of inheritance. Each object will contain at a minimum, two traits: A trait defining primitive operations all objects need to understand (such as slot lookup, evaluator, etc) which has the lowest lookup priority; and a trait defining slots that are local only to this object. All the other traits are considered foreign.

Object concurrency is handled by the Actor model. This allows us some great deal of flexibility in how the underlying architecture works. Merely placing "common sense" requirements on top of a flexible base.

Caribou is implemented as a stack-based virtual machine.

## License

My express wish is that you use the code as you see fit. Keep my copyright notice on any files I have written, and that you grant others the same rights I've granted you when you distribute it.

### Legal version

Copyright (c) 2011, Jeremy Tregunna, All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Contributing

I'd love to see many people inteterested in helping build this project, and contribute to it ongoing. From documentation to code and even whole subsystems, whatever you can and want to offer, by all means, please let us know.

There are a few things we'd like you to respect though:

1. Develop your feature in a separate branch that I can pull into my repo. Don't clobber the `master` branch. It's the stable branch;
2. We want to keep a uniform license. As such, by contributing code to us, you are granting us license to distribute your changes under the terms of the MIT license, as found above; and
3. If your code isn't completely done yet, it's ok to open a pull request. Just be sure to clearly note that it is incomplete and shouldn't be merged into the mainline. I test these things out beforehand, but still; be respectful of others who may want your feature now.
4. Please include tests.
