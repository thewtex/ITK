#!/usr/bin/env python

import sys, os, re, glob, cStringIO


def setGroup( fname, group ):
#  print >> sys.stderr, "Processing", fname
  f = open( fname, "r" )
  out = cStringIO.StringIO()
  # load everything in memory
  fcontent = f.read()
  f.close()
  # now parse all the doxygen fields
  last = 0
  for m in re.finditer(r"/\*\*(.*?)\*/", fcontent, re.DOTALL):
    # write what is before the doxygen field to the output
    out.write(fcontent[last:m.start(1)])
    last = m.end(1)
    dcontent = m.group(1)
    # we don't care about doxygen fields not about a class
    if r"\class" in dcontent and dcontent != " \class classname ":
      # do we have a line with the expected content?
      if re.search(r"\ingroup .*"+group+"( |$)", dcontent, re.MULTILINE):
        # yes - just keep the content unchanged
        out.write(dcontent)
      else:
        # add the expected group
        if "\n" in dcontent:
          # this is a multiline content. Find the indent
          indent = re.search("( *)(\*|$)", dcontent).group(1)
          lastLine = dcontent.splitlines()[-1]
          if re.match(r'^ *$', lastLine):
            out.write(dcontent+"* \\ingroup "+group+"\n"+indent)
          else:
            out.write(dcontent.rstrip()+"\n"+indent+"* \\ingroup "+group+"\n"+indent)
        else:
          out.write(dcontent+" \\ingroup "+group+" ")
    else:
      out.write(dcontent)
  out.write(fcontent[last:])
  # we can save the content to the original file
  f = open( fname, "w" )
  f.write( out.getvalue() )
  f.close()

def checkGroup( fname, group ):
#  print >> sys.stderr, "Checking", fname
  f = open( fname, "r" )
  # load everything in memory
  fcontent = f.read()
  f.close()
  # now parse all the doxygen fields
  ret = 0
  for m in re.finditer(r"/\*\*(.*?)\*/", fcontent, re.DOTALL):
    dcontent = m.group(1)
    # we don't care about doxygen fields not about a class
    if r"\class" in dcontent and dcontent != " \class classname ":
      # do we have a line with the expected content?
      if not re.search(r"\ingroup .*"+group+"( |$)", dcontent, re.MULTILINE):
        # get class name and the line for debug output
        cname = re.search(r"\class +([^ ]*)", dcontent).group(1).strip()
        line = len(fcontent[:m.start(1)].splitlines())
        print >> sys.stderr, r'%s:%s: error: "\ingroup %s" not set in class %s.' % (fname, line, group, cname)
        ret = 1
  return ret

def main():
  # first arg is the command
  command = sys.argv[1]

  if command == "set":
    # second arg is the module name, and the rest are the files to process
    module = sys.argv[2]
    files = sys.argv[3:]
    for fname in files:
      setGroup(fname, module)
    return 0

  elif command == "massive-set":
    if len(sys.argv) >= 3:
      d = sys.argv[2]
    else:
      d = sys.path[0]+"/../.."
    cmm = os.path.abspath(d+"/*/*/*/itk-module.cmake")
    for fname in glob.glob(cmm):
      f = file(fname, "r")
      mcontent = f.read()
      f.close()
      module = re.match(r"itk_module\(([^ )]+)", mcontent).group(1)
      dname = os.path.dirname(fname)
      for fname2 in glob.glob(dname+"/include/*.h"):
        setGroup(fname2, module)
    return 0

  elif command == "check":
    # second arg is the module name, and the rest are the files to process
    module = sys.argv[2]
    files = sys.argv[3:]
    ret = 0
    for fname in files:
      ret = max( ret, checkGroup(fname, module) )
    return ret

  elif command == "massive-check":
    if len(sys.argv) >= 3:
      d = sys.argv[2]
    else:
      d = sys.path[0]+"/../.."
    cmm = os.path.abspath(d+"/*/*/*/itk-module.cmake")
    ret = 0
    for fname in glob.glob(cmm):
      f = file(fname, "r")
      mcontent = f.read()
      f.close()
      module = re.match(r"itk_module\(([^ )]+)", mcontent).group(1)
      dname = os.path.dirname(fname)
      for fname2 in glob.glob(dname+"/include/*.h"):
        ret = max( ret, checkGroup(fname2, module) )
    return ret

  else:
    print >> sys.stderr, "Unknown command", command
    return 1

if __name__ == "__main__":
  ret = main()
  sys.exit(ret)
