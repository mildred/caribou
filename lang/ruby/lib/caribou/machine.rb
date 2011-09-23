# Caribou Virtual Machine
# Copyright © 2011, Jeremy Tregunna, All Rights Reserved.
#

module Caribou
  class Machine
    attr_reader :istack, :dstack, :memory, :symtab
    attr_accessor :ip

    def initialize
      @istack, @dstack, @memory = [], [], []
      @ip = 0
      @symtab = Symtab.new
    end

    # Pushes a value onto the data stack
    # i.e.,
    # Before:   After:
    #  ___       ___
    # | A |     | B |
    #  ---      |---|
    #           | A |
    #            ---
    def push(val)
      dstack.push val
    end

    # Pushes the instruction pointer onto the instruction stack
    def puship
      istack.push ip
    end

    # Pops the top of the stack.
    # i.e.,
    # Before:   After:
    #  ___       ___
    # | A |     | B |
    # |---|      ---
    # | B |
    #  ---
    def pop
      dstack.pop
    end

    # Return a value to the caller.
    # Like pop, but also pops the instruction stack.
    def ret
      r = dstack.pop
      self.ip = istack.pop
      r
    end

    # Swaps the two top items on the data stack
    # i.e.
    # Before:   After:
    #  ___       ___
    # | A |     | B |
    # |---|     |---|
    # | B |     | A |
    #  ---       ---
    def swap
      a, b = pop, pop
      push a
      push b
    end

    # Duplicates the top of the stack
    # i.e.,
    # Before:   After:
    #  ___       ___
    # | A |     | A |
    #  ---      |---|
    #           | A |
    #            ---
    def dup
      a = pop
      push a
      push a
    end

    # Loads some item from memory and pushes it onto the stack
    def load
      a = pop
      push memory[a]
    end

    # Stores an item into memory at a specific index
    def store
      a = pop
      memory[a] = pop
    end

    # Adds a symbol to the global symbol table
    def add_literal
      symbol = pop
      symtab.add_literal(symbol)
    end

    # Find a constant
    def find_const
      symbol = pop
      symtab.find_literal(symbol)
    end

    # Create an object and push it onto the stack
    # Consumes the top of the stack — A list of traits or nil
    def create_object
      traits = pop
      push setup_object(traits)
    end

    def set
    end

    def build_message
    end

    def lookup
    end

    def send
    end

    private

    def next
      self.ip += 1
    end

    def setup_object(traits = [ Trait.new ])
      # Logic to create an object here
    end
  end
end
