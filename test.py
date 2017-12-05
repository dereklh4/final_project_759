import subprocess
import commands
import sys

if __name__ == "__main__":

	if len(sys.argv) != 2:
		print("USAGE: python test.py <times>")
	num_times = int(sys.argv[1])

	black_wins = 0
	white_wins = 0
	ties = 0

	for i in xrange(num_times):
		print("Running for " + str(i))
		tmp = commands.getoutput("bash local_run.sh")
		lines = tmp.split("\n")
		lines = lines[-20:]

		#find the count
		for x in xrange(19,0,-1):
			try:
				line = lines[x]
			except:
				print("ERROR:")
				print(len(lines))
				print(x)
			if line.find("[Server] Black:") != -1:
				comps = line.split(":")
				black_count = int(comps[1][:3])
				white_count = int(comps[2])
				break
		results = str(black_count) + " to " + str(white_count)
		if black_count > white_count:
			print("B wins " + results)
			black_wins += 1
		elif white_count > black_count:
			print("W wins " + results)
			white_wins += 1
		else:
			print("Tie" + results)
			ties += 1
		print(str(black_wins) + "-" + str(white_wins) + "-" + str(ties))
