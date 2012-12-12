filename = ARGV[0]
f = open(filename)
line = f.gets
t = line.split(" ")
rowsize = t[0].to_i
colsize = t[1].to_i
#p rowsize
#p colsize
output = "Output = ["
while line = f.gets
	if line != "\n"
		line.gsub!("\n", "")
		line.gsub!(/^\ +/, "")
		line.gsub!(/\ +$/, "")
		line.gsub!(/\ +/, ",")
		output += "[" + line + "],"
	end
end
if rowsize > 0
	output[output.length - 1] = "]$"
else
	output += "]$"
end
#p output
f.close()

wfilename = filename + ".rr"
wf = open(wfilename, "w")
wf.puts(output)
wf.puts("end$")
wf.close()
#p filename + " --> " + wfilename
