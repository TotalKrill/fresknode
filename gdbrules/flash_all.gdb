
set confirm off
set pagination off

python
def my_ignore_errors(arg):
  try:
    gdb.execute("print \"" + "Executing command: " + arg + "\"")
    gdb.execute (arg)
  except:
    gdb.execute("print \"" + "ERROR: " + arg + "\"")
    pass

my_ignore_errors("targ ext 130.240.234.198:3333")
my_ignore_errors("load")
my_ignore_errors("monitor reset")

my_ignore_errors("targ ext 130.240.234.199:3333")
my_ignore_errors("load")
my_ignore_errors("monitor reset")

my_ignore_errors("targ ext 130.240.234.200:3333")
my_ignore_errors("load")
my_ignore_errors("monitor reset")

my_ignore_errors("targ ext 130.240.234.201:3333")
my_ignore_errors("load")
my_ignore_errors("monitor reset")

gdb.execute("quit")
