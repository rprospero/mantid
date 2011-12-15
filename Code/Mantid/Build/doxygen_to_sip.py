"""Script that will grab doxygen strings from a 
.cpp file and stuff them in the corresponding
.sip file under a %Docstring tag"""
import os
import sys
from optparse import OptionParser

#----------------------------------------------------------
def findInSubdirectory(filename, subdirectory=''):
    if subdirectory:
        path = subdirectory
    else:
        path = os.getcwd()
    for root, dirs, names in os.walk(path):
        if filename in names:
            return os.path.join(root, filename)
    raise 'File not found'

def find_cpp_file(basedir, classname):
    return findInSubdirectory(classname + ".cpp", basedir) 



#----------------------------------------------------------
def grab_doxygen(cppfile, method):
    """Grab the DOXYGEN documentation string from a .cpp file
    cppfile :: full path to the .cpp file
    method :: method definition to look for
    """
    lines = open(cppfile, 'r').read().split('\n')
    #print method
    out = []
    for i in range(len(lines)):
        line = lines[i].strip()
        if line.startswith(method):
            # Go backwards above the class to grab the doxygen
            for j in range(i-1, -1, -1):
                out.insert(0, lines[j])
                # Stop when you reach the start of the comment "/**"
                if lines[j].strip().startswith('/**'):
                    break
            # OK return the lines
            return out
    print "WARNING: Could not find method %s" % method
    return None
    
    
#----------------------------------------------------------
def doxygen_to_docstring(doxygen, method):
    """ Takes an array of DOXYGEN lines, and converts
    them to a more pleasing python docstring format
    @param doxygen :: list of strings contaning the doxygen
    @param method :: method declaration string """
    out = []
    if doxygen is None:
        return out
    out.append("%Docstring")
    out.append(method)
    out.append('-' * len(method))
    for line in doxygen:
        line = line.strip()
        if line.startswith("/**"): line = line[3:]
        if line.endswith("*/"): line = line[:-2]
        if line.startswith("*"): line = line[1:]
        # Make the text indented by 4 spaces
        line = "   " + line
        out.append(line)
    out.append("%End")
    out.append("")
    return out

#----------------------------------------------------------
def process_sip(filename):
    """ Reads an input .sip file and finds methods from
    classes. Retrieves doxygen and adds them as
    docstrings """
    
    root = os.path.split(os.path.abspath(filename))[0] 
    # Read and split into a buncha lines
    lines = open(filename, 'r').read().split('\n')
    i = 0
    classname = ''
    classcpp = ''
    outlines = []
    for i in range(len(lines)):
        # Copy to output
        outlines.append(lines[i])
        
        line = lines[i].strip()
        if line.startswith("class "):
            classname = line[6:].strip()
            n = classname.find(':')
            if n > 0: classname = classname[0:n].strip()
            # Now, we look for the .cpp file
            classcpp = find_cpp_file(root, classname)
            print "Found class '%s' at %s" % (classname, classcpp)
            
        if classname != "":
            # We are within a real class
            if line.endswith(";"):
                # Within a function declaration
                n = line.find(')')
                if n > 0:
                    method = line[0:n+1]
                    n = method.find(' ')
                    # Make the string like this:
                    # "void ClassName::method(arguments)" 
                    method = method[0:n+1] + classname + "::" + method[n+1:]
                    # Now extract the doxygen
                    doxygen = grab_doxygen(classcpp, method)
                    # Convert to a docstring
                    docstring = doxygen_to_docstring(doxygen, method)
                    # And add to the output
                    outlines += docstring
    # Give back the generated lines
    return outlines
    
if __name__=="__main__":
    
    parser = OptionParser(description='Utility to delete a Mantid class from a project. ' 
                                     'Please note, you may still have more fixes to do to get compilation!')
    parser.add_option('-i', metavar='SIPFILE', dest="sipfile",
                        help='The .sip input file')
    
    parser.add_option('-o', metavar='OUTPUTFILE', dest="outputfile",
                        help='The name of the output file')

    (options, args) = parser.parse_args()
    
    print "---- Reading from %s ---- " % options.sipfile
    out = process_sip(options.sipfile)
    
    if not (options.outputfile is None):
        print "---- Writing to %s ---- " % options.outputfile
        f = open(options.outputfile, 'w')
        f.write('\n'.join(out))
        f.close()

    
    