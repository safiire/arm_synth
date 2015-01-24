#!/usr/bin/env ruby
require './scripts/audio.rb'

exit(1) unless ARGV.size == 1

## 
#  Grab the #define'd values out of my header file
macros = get_c_macros
inverse_of_fundamental_frequency = ARGV.first.to_i / macros['Fs']

midi_map_equal_tempered.each_with_index do |note, i| 
    rate = note[:frequency] * inverse_of_fundamental_frequency
    #  Make this into Q16.16 Fixed Point value
    rate = (rate * (2**16)).to_i
    print "0x%x" % rate
    puts "," unless i == 127
end
