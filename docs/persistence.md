# Persistence in Caribou

Caribou supports a form of stack persistence which allows us to replay revisions to reproduce the stack as it was at some point in time.

The idea is we store revisions for every stack operation. So when you push an item onto the stack, we generate a revision out of that. This revision contains enough information to replay this at some future point, and contains a unique identifier (probably the address on the heap). As an example, let's examine a continuations implementation that uses this mechanism.

Our program executes and creates a continuation. Let's call this original stack `A`. At some point in the future, we want to restore the stack, which has branched effectively from `A`. Let's call this stack `B`. All we need to do to restore the stack, is rewind until we get to the revision `A` was at. Additionally, if we created a continuation at some point in the future, lets say just before `B` was rewound, and we invoke that; what we do is simply replay the stack revisions up to that point.

This is still a work in progress idea, but it will involve a few items:

* Revision object, which represents the data in the stack and the version number.
* Rewrite continuations to make use of this. This should avoid large copying.

With this subsystem in place and working, it should then be possible to enhance the basic activation records promoting them to be first class contexts and garbage collected objects.
