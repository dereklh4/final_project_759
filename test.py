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
	times = []
	diff_times = []

	for i in xrange(num_times):
		print("Running for " + str(i))
		tmp = commands.getoutput("bash local_run.sh")
		lines = tmp.split("\n")
		lines = lines[-20:]

		black_time = 0
		white_time = 0
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
			if line.find("Black Time:") != -1:
				black_time = round(float(line.split(":")[1]),4)
			if line.find("White Time:") != -1:
				white_time = round(float(line.split(":")[1]),4)
		results = str(black_count) + " to " + str(white_count)
		if black_count > white_count:
			print("B wins " + results)
			black_wins += 1
		elif white_count > black_count:
			print("W wins " + results)
			white_wins += 1
		else:
			print("Tie " + results)
			ties += 1
		times.append((black_time,white_time))
		diff_times.append(white_time - black_time)
		print("Time Left -- Black: " + str(black_time) + " " + "White: " + str(white_time))
		print(str(black_wins) + "-" + str(white_wins) + "-" + str(ties))

	#print out average time advantage
	avg_black = sum(map(lambda x: x[0],times))/(len(times)*1.0)
	avg_white = sum(map(lambda x: x[1],times))/(len(times)*1.0)
	print("Average time left black: " + str(avg_black))
	print("Average time left white: " + str(avg_white))
	print("Average diff: " + str( sum(diff_times)/(len(diff_times) * 1.0) ))

	b = filter(lambda x: x < 0, diff_times)
	w = filter(lambda x: x > 0, diff_times)
	print("Time battle: " + (str(len(b))) + "-" + str(len(w)))
