# Caribou Language
# Copyright © 2011, Jeremy Tregunna, All Rights Reserved.
#

module Caribou
  class Object
    attr_accessor :done_lookup, :activatable
    attr_reader :traits

    # We don't have a slot table. Traits contain the slots. The first trait (at index 0 in the array) is always the trait that is local to the object itself.
    # Please do not shift something onto the start of the array.
    def initialize(init_mt = false)
      @traits = [ Trait.new ]
      @done_lookup, @activatable = false, false
    end

    def use_trait(trait)
      traits << trait if trait.kind_of?(Caribou::Trait)
    end

    def lookup(env, sym)
      sym = sym.intern
      found = nil

      traits.each_with_index do |trait, index|
        if trait.implements?(sym)
          found = trait.slots[sym]
          break
        end
      end

      found
    end

    def perform(env)
      return env[:msg].cached_result if env[:msg].cached_result?

      env[:target] = self
      obj = lookup(env, env[:msg].name)
      if obj
        if obj.activatable
          activate = obj.lookup(env, :activate)
          return activate.call(env) if activate
        end
        return obj
      end

      forward = obj.lookup(env, :forward)
      return forward.call(env)
    end
  end
end
