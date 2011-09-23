# Caribou Language
# Copyright © 2011, Jeremy Tregunna, All Rights Reserved.
#

require 'algorithms'

module Caribou
  class Symtab
    attr_reader :store

    def initialize
      @store = ::Containers::Trie.new
      @counter = 0
    end

    def add_literal(str)
      return str if store.get(str)
      @counter += 1
      store.push(str, @counter)
      @counter
    end

    def find_literal(idx)
      store.get(idx)
    end
  end
end
