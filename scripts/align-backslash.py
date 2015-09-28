#!/usr/bin/env python
import sys

if len(sys.argv) < 3:
	sys.exit('usage: {} <in> <out>'.format(sys.argv[0]))
	
fin = sys.argv[1]
fout = sys.argv[2]

with open(fin) as lines_in, open(fout, 'w') as lines_out:
		for line in lines_in:
			line_out = line
			if line.rstrip().endswith('\\'): 
				idx = line.rindex('\\')
				strip = line[:idx].rstrip()
				w = len(strip)
				for col in xrange(80, 220, 10):
					if w < col:
						pad = ' '*(col - w - 1)
						line_out = strip + pad + '\\\n'
						break
			lines_out.write(line_out)
			
				
