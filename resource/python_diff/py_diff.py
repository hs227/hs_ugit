import argparse
import difflib


"""Read the content of a file."""
def read_file(filepath):
  with open(filepath,'r',encoding='utf-8') as f:
    return f.readlines()
  
"""Compare two files and return the differences."""
def compare_files(file1,file2,output_format='unified'):
  lines1=read_file(file1)
  lines2=read_file(file2)

  # Compute the difference
  differ=difflib.Differ()
  diff=list(differ.compare(lines1,lines2))

  if output_format=='unified':
    # Generate unified diff
    diff=difflib.unified_diff(lines1,lines2,fromfile=file1,tofile=file2,n=3)
  elif output_format=='context':
    # Generate context diff
    diff=difflib.context_diff(lines1,lines2,fromfile=file1,tofile=file2,n=3)
  
  return diff


def main():
  parser=argparse.ArgumentParser(description="A simple command-line tool to compare")
  parser.add_argument("file1",help="Path to the first input file")
  parser.add_argument("file2",help="Path to the second input file")
  parser.add_argument("-o","--output",help="Path to the output file (optional)")
  parser.add_argument("-f","--format",choices=['unified','context'],default='unified',help="Output format (unified or context)")

  args=parser.parse_args()

  # Compare the files
  diff=compare_files(args.file1,args.file2,output_format=args.format)

  # Output the difference
  if args.output:
    with open(args.output,'w',encoding='utf-8') as f:
      f.writelines(diff)
  else:
    for line in diff:
      print(line,end='')

if __name__ == "__main__":
  main()







