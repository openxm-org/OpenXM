# gfan の出力ファイルから gfan_groebnercone の入力ファイルを生成
filename = ARGV[0]
f = open(filename)
ring = f.gets
str = f.read
str.gsub!("\n", "")
str.gsub!("{{", "")
str.gsub!("}}", "")
t = str.split("},{")
f.close()

internalP = "[";
for i in 0..t.length-1
	# wfilename = filename + i.to_s + ".gbc"
	wfilename = filename + ".gbc"
	wf = open(wfilename, "w")
	wf.puts(ring)
	wf.puts("{" + t[i] + "}")
	wf.close()

	resultfile = wfilename + ".result"
	system("gfan_groebnercone <" + wfilename  + ">" + resultfile)
	# relative interior point を取り出し
	ff = open(resultfile)
	while ((ff.gets =~ /^RELATIVE/) == nil) 
	end
	points = ff.gets
	points.gsub!("\n", "")
	points.gsub!(" ", ",")	
	points = "[" + points + "],"
	internalP = internalP + points
end
internalP[internalP.length-1] = "]"
wfilename = filename + "_weight.rr"
wf = open(wfilename, "w")
wf.puts("WeightV = " + internalP + ";")
wf.puts("end$")
wf.close()

