# Caribou Language
# Copyright © 2011, Jeremy Tregunna, All Rights Reserved.
#

module Caribou
  class Trait
    attr_reader :slots

    def initialize(slots = Containers::Trie.new)
      @slots = slots
    end

    def implements?(name)
      @slots.has_key?(name)
    end
  end
end
