from sys import argv
from math import sqrt

def main(argv):
    f = open(argv[1],'r')
    lineas = f.readlines()
    
    for linea in lineas:
        ll = linea.split()
        n = ll[0]
        t = ll[1]
        
        arreglada = ""
        
        print n,
        
        for i in range(len(t)):
            arreglada += str(t[i])
            if (i+1 < len(t)):
                arreglada += ","
        
        print arreglada

if __name__ == "__main__":
    main(argv)
