set confirm off

python
def my_ignore_errors(arg):
  try:
    gdb.execute("print \"" + "Executing command: " + arg + "\"")
    gdb.execute (arg)
  except:
    gdb.execute("print \"" + "ERROR: " + arg + "\"")
    pass

gdb.execute("targ ext rpi1:3333")
gdb.execute("load")
gdb.execute("monitor reset")

gdb.execute("targ ext rpi2:3333")
gdb.execute("load")
gdb.execute("monitor reset")

gdb.execute("targ ext rpi3:3333")
gdb.execute("load")
gdb.execute("monitor reset")

gdb.execute("quit")
