import argparse
import difflib
from difflib import SequenceMatcher


"""Read the content of a file."""
def read_file(filepath):
  with open(filepath,'r',encoding='utf-8') as f:
    return f.readlines()
  
"""Compare two files and return the differences."""
def compare_files(file1,file2,output_format='unified',arg_empty='n'):
  lines1= [] if (arg_empty=='l') else read_file(file1)
  lines2= [] if (arg_empty=='r') else read_file(file2)
  if lines1 and lines1[-1][-1] != '\n':
    lines1[-1]+='\n'
  if lines2 and lines2[-1][-1] !='\n':
    lines2[-1]+='\n'
  # print("lines1:")
  # print(lines1)
  # print("lines2:")
  # print(lines2)

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

""" two-way merge """
def merge_files(file1,file2,merge_name='',merge_auto=True,arg_empty='n'):
  lines1= [] if (arg_empty=='l') else read_file(file1)
  lines2= [] if (arg_empty=='r') else read_file(file2)
  matcher=SequenceMatcher(None,lines1,lines2)
  if lines1 and lines1[-1][-1] != '\n':
   lines1[-1]+='\n'
  if lines2 and lines2[-1][-1] !='\n':
   lines2[-1]+='\n'
  # print("lines1:")
  # print(lines1)
  # print("lines2:")
  # print(lines2)

  merged_lines=[]
  for op in matcher.get_opcodes():
    tag,i1,i2,j1,j2=op
    body=[]
    if tag=='equal':
      # 如果两端相等，直接添加
      merged_lines.extend(lines1[i1:i2])
    elif tag=='replace':
      # 如果需要替换，用file2中的内容替换file1的内容
      if merge_auto:
        body.extend(lines2[j1:j2])
      else:
        body.append(f'#ifndef {merge_name}\n')
        body.extend(lines1[i1:i2])
        body.append(f'#else /* {merge_name} */\n')
        body.extend(lines2[j1:j2])
        body.append(f'#endif /* ! {merge_name} */\n')

      merged_lines.extend(body)
    elif tag=='delete':
      # 如果需要删除，从结果中删除file1的内容
      if merge_auto:
        pass
      else:
        body.append(f'#ifdef {merge_name}\n')
        body.extend(lines1[i1:i2])
        body.append(f'#endif /* {merge_name} */\n')
        merged_lines.extend(body)

    elif tag=='insert':
      # 如果需要插入，则插入file2的内容
      if merge_auto:
        body.extend(lines2[j1:j2])
      else:
        body.append(f'#ifndef {merge_name}\n')
        body.extend(lines2[j1:j2])
        body.append(f'#endif /* ! {merge_name} */\n')

      merged_lines.extend(body)

  return merged_lines



def main():
  parser=argparse.ArgumentParser(description="A simple command-line tool to compare")
  parser.add_argument("file1",help="Path to the first input file")
  parser.add_argument("file2",help="Path to the second input file")
  parser.add_argument("-o","--output",help="Path to the output file (optional)")
  parser.add_argument("-f","--format",choices=['unified','context'],default='unified',help="Output format (unified or context)")
  parser.add_argument("-D","--DHEAD",help="Merge rather than diff")
  parser.add_argument("-u","--user",action="store_false",help="Only useful in merge,to decide if user-need",)
  parser.add_argument("-e","--empty",choices=['n','l','r'],default='n',help="whether one filename should be treated as empty")
  args=parser.parse_args()

  if args.DHEAD:
    diff=merge_files(args.file1,args.file2,args.DHEAD,args.user,args.empty)
  else:
    # Compare the files
    diff=compare_files(args.file1,args.file2,args.format,args.empty)

  # Output the difference to file or stdout
  if args.output:
    with open(args.output,'w',encoding='utf-8') as f:
      f.writelines(diff)
  else:
    for line in diff:
      print(line,end='')

if __name__ == "__main__":
  main()







