filename = ARGV[0]
f = open(filename)
ring = f.gets
str = f.read
str.gsub!("\n", "")
str.gsub!("{{", "")
str.gsub!("}}", "")
t = str.split("},{")
output = "Output = ["
output2 = "Output2 = ["
for i in 0..t.length-1
	output += "[" + t[i] + "],"
	polys = t[i].split(",")
	# p "polys :"
	# p polys 
	hts = "["
	for j in 0..polys.length-1
		p = polys[j]
		tt = p.split(/[\+-]/)
		# p "tt :"
		# p tt
		ht = tt[0]
		hts += ht + ","
	end
	hts[hts.length - 1] = "]"
	output2 += hts + ","
end
output[output.length - 1] = "]"
output += "$"
output2[output2.length - 1] = "]"
output2 += "$"
# p output
f.close()

wfilename = filename + ".rr"
wf = open(wfilename, "w")
wf.puts(output)
wf.puts(output2)
wf.puts("end$")
wf.close()
#p filename + " --> " + wfilename
