#!/usr/bin/env python
import sys, csv
import os

def getCandidates(filename):
  ret = dict()
  header = ""
  nNotValid = 0
  with open(filename) as csvfile:
      readCSV = csv.reader(csvfile, delimiter=',')
      for row in readCSV:
          if row[0].startswith('#'):
            # print(','.join(row))
            header = ','.join(row)
            continue
          run = int(row[1])
          subrun = int(row[2])
          event = int(row[3])
          weight = float(row[4])
          status = int(row[5])
          presel = int(row[6])
          numu = int(row[7])
          nue = int(row[8])
          Ereco = float(row[9])
          Evis = float(row[10])
  
          if presel==1 and status==0:
              nNotValid += 1
              continue
          ret[(run,subrun,event)] = row
  print("# of failure events: %d" %nNotValid)
  return (header, ret)


if __name__=="__main__":
  filename1 = sys.argv[1]
  filename2 = sys.argv[2]
  
  header1, cand_1 = getCandidates(filename1)
  header2, cand_2 = getCandidates(filename2)
  
  field_1 = set(cand_1.keys())
  field_2 = set(cand_2.keys())
  
  common = field_1.intersection(field_2)

  basename1 = os.path.basename(filename1) + ".comm"
  with open(basename1,'w') as f1:
    f1.write(header1+'\n')
    for RSE in common:
      f1.write(','.join(cand_1[RSE])+'\n')

  basename2 = os.path.basename(filename2) + ".comm"
  with open(basename2,'w') as f2:
    f2.write(header2+'\n')
    for RSE in common:
      f2.write(','.join(cand_2[RSE])+'\n')
