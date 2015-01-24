#!/usr/bin/env ruby
require 'complex'

f = eval "lambda{|z| #{ARGV.first} }"

###  
#  Let's try to plot the transfer function h = 1 / 1 - Z**-1
h = (1..22050).map do |f0|
    w = 2 * Math::PI * (f0 / 44100.0)
    z = Complex.polar(1, w)
    f[z]
end


`rm *.dat *.svg *.html`

File.open('frequency_response.dat', 'w') do |fp|
    h.each_with_index do |z, index|
        fp.puts "#{index}\t#{z.abs}"
    end
end
`gnuplot -e 'set term svg; set output "frequency_response.svg"; plot "frequency_response.dat" using 1:2;'`


File.open('phase_response.dat', 'w') do |fp|
    h.each_with_index do |z, index|
        fp.puts "#{index}\t#{z.phase rescue 0}"
    end
end
`gnuplot -e 'set term svg; set output "phase_response.svg"; plot "phase_response.dat" using 1:2;'`

File.open('plot.html', 'w') do |fp|
    fp.puts "<html><head><title>Response of #{ARGV.first}</title><body>"
    fp.puts "<div>Response of #{ARGV.first}</div>"
    fp.puts "<img src=\"frequency_response.svg\" />"
    fp.puts "<img src=\"phase_response.svg\" />"
    fp.puts "</body></html>"
end

`open plot.html`
