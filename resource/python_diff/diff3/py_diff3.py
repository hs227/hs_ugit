import argparse
import difflib
from difflib import SequenceMatcher
from collections import namedtuple


"""Read the content of a file."""
def read_file(filepath):
  with open(filepath,'r',encoding='utf-8') as f:
    return f.readlines()



""" three-way merge """
def merge(v1_path,base_path,v2_path,labels,arg_empty='n'):
  lines1= [] if (arg_empty=='l') else read_file(v1_path)
  lines2= [] if (arg_empty=='r') else read_file(v2_path)
  linesb= read_file(base_path)
  matcherb1=SequenceMatcher(None,linesb,lines1)
  matcherb2=SequenceMatcher(None,linesb,lines2)

  if lines1 and lines1[-1][-1] != '\n':
    lines1[-1]+='\n'
  if lines2 and lines2[-1][-1] != '\n':
    lines2[-1]+='\n'
  if linesb and linesb[-1][-1] != '\n':
    linesb[-1]+='\n'
  print(f'lines1:{lines1}')
  print(f'lines2:{lines2}')
  print(f'linesb:{linesb}')

  # 创建 SequenceMatcher 对象
  matcher=difflib.SequenceMatcher(None,lines1,lines2,linesb)

  # Initialize lists to hold the merged lines
  merged_lines=[]
  block1=[]
  block2=[]
  # Iterate over the differences
  for tag, i1, i2, j1,j2 in matcher.get_opcodes():
    body=[]
    print(f'{tag}:{i1}:{i2}:{j1}:{j2}')
    # Handle addtions and deletions
    if tag == 'delete':
       #print(f'tag:{linesb[i1:]}')
       merged_lines.extend(linesb[j1:j1])
    elif tag == 'insert':
       #print(f'insert:{linesb[i1:]}')
       merged_lines.extend(linesb[i1:i2])
    elif tag == 'replace':
       #print(f'replace:{linesb[i1:j2]}')
       if i1 == j1:
          body.append('<<<<<<\n')
          body.extend(lines1[i1:i2])
          body.append('||||||\n')
          body.extend(lines2[j1:j2])
          body.append('>>>>>>\n')
          merged_lines.extend(body)
       else:
          merged_lines.extend(linesb[i1:i2])
    elif tag == 'equal':
       #print(f'equal:{lines1[i1:i2]}:{lines2[j1:j2]}')
       merged_lines.extend(linesb[i1:i2])
  return merged_lines


  
MBlock = namedtuple ('MBlock', ['tag', 'basei1', 'basei2', 'content'])
    
def real_merge(blocks1,blocks2,base,labels):
  merge_lines=[]
  blocks1.reverse()
  blocks2.reverse()

  #return []

  while blocks1 or blocks2:
    b1= None if not blocks1 else blocks1.pop()
    b2= None if not blocks2 else blocks2.pop()
    if not b1:
      # still b2
      merge_lines.extend(b2.content)
      continue
    if not b2:
      # still b1
      merge_lines.extend(b1.content)
      continue
    # b1 b2 still need merge
    assert b1.basei1==b2.basei1, f"开始行数必须一致{b1.basei1}:{b2.basei1} "
    if b1.tag=='equal' and b2.tag=='equal':
      # two equals
      # in this case baseix==updatejx
      if b1.basei2==b2.basei2:
        merge_lines.extend(b1.content)
      elif b1.basei2>b2.basei2:
        # b1存在多余部分
        same_content=b2.content
        merge_lines.extend(same_content)

        diff_content=b1.content[b2.basei2-b2.basei1:]
        body=MBlock('equal',b2.basei2,b1.basei2,diff_content)
        blocks1.append(body)
      elif b1.basei2<b2.basei2:
        # b2存在多余部分
        same_content=b1.content
        merge_lines.extend(same_content)

        diff_content=b2.content[b1.basei2-b1.basei1:]
        body=MBlock('equal',b1.basei2,b2.basei2,diff_content)
        blocks2.append(body)
    elif b1.tag == 'equal':
      # b2 modify
      if b1.basei2 == b2.basei2:
        merge_lines.extend(b2.content)
      elif b1.basei2 > b2.basei2:
        # b1存在多余E部分
        modify_content=b2.content
        merge_lines.extend(modify_content)

        diff_content=b1.content[b2.basei2-b1.basei1:]
        body=MBlock('equal',b2.basei2,b1.basei2,diff_content)
        blocks1.append(body)
      elif b1.basei2 < b2.basei2:
        # b2 存在多余M部分
        # 合并b1和b1_next,将E块合并到M块中
        b1_next=blocks1.pop()
        next_content=b1.content+b1_next.content
        body=MBlock('modify',b1.basei1,b1_next.basei2,next_content)
        blocks1.append(body)
        blocks2.append(b2)
    elif b2.tag =='equal':
      # b1 modify
      if b1.basei2 == b2.basei2:
        merge_lines.extend(b1.content)
      elif b1.basei2 > b2.basei2:
        # b1存在多余M部分
        # 合并b2和b2_next,将E块合并到M块中
        b2_next=blocks2.pop()
        next_content=b2.content+b2_next.content
        body=MBlock('modify',b2.basei1,b2_next.basei2,next_content)
        blocks1.append(b1)
        blocks2.append(body)
      elif b1.basei2 < b2.basei2:
        # b2存在多余E部分
        modify_content=b1.content
        merge_lines.extend(modify_content)

        diff_content=b2.content[b1.basei2-b1.basei1:]
        body=MBlock('equal',b1.basei2,b2.basei2,diff_content)
        blocks2.append(body)
    else:
      # two modify
      if b1.basei2 == b2.basei2:
        # user merge
        body=[]
        # HEAD
        body.append(f'<<<<<<< {labels[0][0]}\n')
        body.extend(b1.content)
        # BASE
        body.append(f'||||||| {labels[0][1]}\n')
        body.extend(base[b1.basei1:b1.basei2])
        body.append(f'=======\n')
        # MHEAD
        body.extend(b2.content)
        body.append(f'>>>>>>> {labels[0][2]}\n')

        merge_lines.extend(body)
      elif b1.basei2 > b2.basei2:
        b2_next=blocks2.pop()
        next_content=b2.content+b2_next.content
        body=MBlock('modify',b2.basei1,b2_next.basei2,next_content)
        blocks1.append(b1)
        blocks2.append(body)
      elif b1.basei2 < b2.basei2:
        b1_next=blocks1.pop()
        next_content=b1.content+b1_next.content
        body=MBlock('modify',b1.basei1,b1_next.basei2,next_content)
        blocks1.append(body)
        blocks2.append(b2)

  return merge_lines

""" three-way merge v1"""
def merge_v1(v1_path,base_path,v2_path,labels,arg_empty='n'):
  lines1= [] if (arg_empty=='l') else read_file(v1_path)
  lines2= [] if (arg_empty=='r') else read_file(v2_path)
  linesb= read_file(base_path)
  matcherb1=SequenceMatcher(None,linesb,lines1)
  matcherb2=SequenceMatcher(None,linesb,lines2)

  if lines1 and lines1[-1][-1] != '\n':
    lines1[-1]+='\n'
  if lines2 and lines2[-1][-1] != '\n':
    lines2[-1]+='\n'
  if linesb and linesb[-1][-1] != '\n':
    linesb[-1]+='\n'

  # each merge
  merged_lines = []
  block1=[]
  block2=[]

  # Iterator over the differences for base1
  for tag, i1, i2, j1, j2 in matcherb1.get_opcodes():
    # print(f'base1:{tag}:{i1}:{i2}:{j1}:{j2}')
    # Handle additions and deletions
    if tag == 'equal':
      content=linesb[i1:i2]
      body=MBlock(tag,i1,i2,content)
      block1.append(body)
    else:
      content=lines1[j1:j2]
      body=MBlock('modify',i1,i2,content)
      block1.append(body)
  # print(f"BLOCK1:")
  # for line in block1:
  #   print(line,end='\n')
  
  
  # Iterator over the differences for base2
  for tag, i1, i2, j1, j2 in matcherb2.get_opcodes():
    # print(f'base2:{tag}:{i1}:{i2}:{j1}:{j2}')
    # Handle additions and deletions
    if tag == 'equal':
      content=linesb[i1:i2]
      body=MBlock(tag,i1,i2,content)
      block2.append(body)
    else:
      content=lines2[j1:j2]
      body=MBlock('modify',i1,i2,content)
      block2.append(body)
  # print(f"BLOCK2:")
  # for line in block2:
  #   print(line,end='\n')

  merged_lines=real_merge(block1,block2,linesb,labels)

  return merged_lines






def main():
  parser=argparse.ArgumentParser(description="diff3 simple windows version")
  parser.add_argument("-m","--merge",action='store_true',help='Try to merge files',required=True)
  parser.add_argument("-L","--labels",action='append',help='Label for the files',required=True,nargs=3)
  parser.add_argument("version1",help="Path to the version1 file")
  parser.add_argument("base",help="the base of two versions")
  parser.add_argument("version2",help="Path to the version2 file")
  parser.add_argument("-o","--output",help="Path to the output file (optional)")
  parser.add_argument("-e","--empty",choices=['n','l','r'],default='n',help="whether one filename should be treated as empty")

  # 解析命令行参数
  args=parser.parse_args()

  #content=merge(args.version1,args.base,args.version2,args.labels,args.empty)
  content=merge_v1(args.version1,args.base,args.version2,args.labels,args.empty)

  # Output the difference to file or stdout
  if args.output:
    with open(args.output,'w',encoding='utf-8') as f:
      f.writelines(content)
  else:
    for line in content:
      print(line,end='')

if __name__ == "__main__":
  main()

