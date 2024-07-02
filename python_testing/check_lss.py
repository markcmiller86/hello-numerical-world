import sys
import math

def main():
    if len(sys.argv) < 2:
        print("Specify input file as 1st argument")
        sys.exit(1)
    
    rdfile = sys.argv[1]
    errbnd = 1e-7
    relerr = False

    if len(sys.argv) > 2:
        errbnd = float(sys.argv[2])
        if errbnd < 0:
            errbnd = -errbnd
            relerr = True

    Len = 1
    bc0 = 0
    bc1 = 1

    if len(sys.argv) > 5:
        if len(sys.argv) < 6:
            print("Missing boundary condition arguments")
            sys.exit(1)
        Len = float(sys.argv[3])
        bc0 = float(sys.argv[4])
        bc1 = float(sys.argv[5])

    try:
        with open(rdfile, 'r') as file:
            for line in file:
                if '#' in line:
                    continue

                parts = line.split()
                if len(parts) < 2:
                    continue

                xval = float(parts[0])
                yval = float(parts[1])

                if xval == 0:
                    if abs(bc0 - yval) > errbnd:
                        print(f"Check failed at x={xval} y={yval} yexp={bc0}, diff={abs(bc0 - yval)}")
                        sys.exit(1)
                    continue

                exact_yval = bc0 + (bc1 - bc0) * xval / Len
                if relerr:
                    diffr = abs((yval - exact_yval) / exact_yval)
                    diffl = diffr <= errbnd
                else:
                    diffr = abs(yval - exact_yval)
                    diffl = diffr <= errbnd

                if not diffl:
                    print(f"Check failed at x={xval} y={yval} yexp={exact_yval}, diff={diffr}")
                    sys.exit(1)
    except FileNotFoundError:
        print(f"File not found: {rdfile}")
        sys.exit(1)

if __name__ == "__main__":
    main()
