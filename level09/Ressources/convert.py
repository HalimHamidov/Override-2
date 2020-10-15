import sys

if len(sys.argv) == 1:
    print "Pass one argument for conversion"
    exit()

result = ""
passed = sys.argv[1]

if (passed[:2] == "0x"):
    passed = passed[2:]
else:
    result = "\\x" + passed[:2]
    passed = passed[:2]

while(len(passed)):
    num = "\\x" + passed[:2]
    passed = passed[2:]
    result = num + result

print result
