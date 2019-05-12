import shlex, subprocess

command_line = "./a.out"
args = shlex.split(command_line)
p = subprocess.Popen(args)

print("")
